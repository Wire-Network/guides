#include <tipjar.hpp>

#include <tuple>

namespace {
constexpr name token_contract = "sysio.token"_n;
constexpr name active_permission = "active"_n;
const std::string tip_prefix{"tip:"};

std::string trim_spaces(std::string value) {
    auto first = value.begin();
    while (first != value.end() && *first == ' ') {
        ++first;
    }

    auto last = value.end();
    while (last != first && *(last - 1) == ' ') {
        --last;
    }

    return std::string(first, last);
}

name creator_from_memo(const std::string& memo) {
    check(memo.compare(0, tip_prefix.size(), tip_prefix) == 0, "Memo must be in the format 'tip: creator'");

    const auto creator_text = trim_spaces(memo.substr(tip_prefix.size()));
    check(!creator_text.empty(), "Tip memo must include a creator account");

    const name creator{creator_text};

    // A tip is claimable only if the memo names a real SYSIO account. This
    // check rejects the original transfer, so tokens are not trapped under a
    // misspelled or nonexistent creator name.
    check(is_account(creator), "Creator account does not exist");

    return creator;
}
} // namespace

ACTION tipjar::claimtips(name creator) {
    require_auth(creator);

    tips_table tips(get_self(), creator.value);
    auto tip_itr = tips.begin();
    check(tip_itr != tips.end(), "No tips available to claim");

    while (tip_itr != tips.end()) {
        const asset payout = tip_itr->balance;
        check(payout.is_valid(), "Stored tip balance is invalid");
        check(payout.amount > 0, "Stored tip balance must be positive");

        // Erase before sending so this action leaves no claimed balance behind.
        // If the inline transfer fails, the whole transaction rolls back.
        tip_itr = tips.erase(tip_itr);

        // The contract signs this inline transfer with its active permission.
        // sysio.token debits this contract and credits the creator.
        action(
            permission_level{get_self(), active_permission},
            token_contract,
            "transfer"_n,
            std::make_tuple(get_self(), creator, payout, std::string("tip.jar claim"))
        ).send();
    }
}

void tipjar::on_transfer(name from, name to, asset quantity, std::string memo) {
    if (from == get_self() || to != get_self()) {
        return;
    }

    check(get_first_receiver() == token_contract, "Only sysio.token transfers are accepted");

    // sysio.token already checked that the token exists, the precision matches,
    // and the transfer amount is positive. This example accepts every valid
    // token from sysio.token instead of keeping a token allowlist.
    check(quantity.is_valid(), "Invalid tip quantity");
    check(quantity.amount > 0, "Tip quantity must be positive");

    const name creator = creator_from_memo(memo);

    // Each creator gets their own table scope. Inside that scope, each token
    // symbol has one row, so daniel can accrue WIRE and LIQETH independently.
    tips_table tips(get_self(), creator.value);
    const auto sym_code = quantity.symbol.code().raw();
    const auto tip_itr = tips.find(sym_code);

    if (tip_itr == tips.end()) {
        tips.emplace(get_self(), [&](auto& tip) {
            tip.balance = quantity;
        });
    } else {
        tips.modify(tip_itr, same_payer, [&](auto& tip) {
            tip.balance += quantity;
        });
    }
}
