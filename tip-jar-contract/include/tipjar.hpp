#pragma once

#include <sysio/asset.hpp>
#include <sysio/sysio.hpp>

#include <string>

using namespace sysio;

CONTRACT tipjar : public contract {
    
    public:
        using contract::contract;

        /**
         * @brief Claim all tips that have been sent to a creator.
         *
         * The creator signs this action. The contract then sends each stored
         * token balance back to that creator with an inline sysio.token transfer.
         */
        ACTION claimtips(name creator);

        /**
         * @brief Handle tips sent by sysio.token transfer.
         *
         * Users tip a creator by transferring any sysio.token token to this
         * contract with a memo like: "tip: alice".
         */
        [[sysio::on_notify("sysio.token::transfer")]]
        void on_transfer(name from, name to, asset quantity, std::string memo);

    private:
        /**
         * @brief Accumulated tips for one creator and one token symbol.
         *
         * Scope this table by creator account. Inside that creator's scope, the
         * primary key is the token symbol code. For example, scope "daniel" can
         * hold one WIRE row, one LIQETH row, and more rows for other tokens.
         */
        TABLE tip {
            asset balance;

            uint64_t primary_key() const { return balance.symbol.code().raw(); }

            SYSLIB_SERIALIZE(tip, (balance))
        };

        typedef multi_index<"tips"_n, tip> tips_table;
};
