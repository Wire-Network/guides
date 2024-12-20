#include <pollster.hpp>
#include <vector>

// --------- Public Actions ----------
ACTION pollster::signup(
    const name &username, 
    const std::string &ethAddr, 
    const std::string &displayName, 
    const std::string &profilePic
) {
    // Make sure the user calling this action is the user being signed up.
    check(has_auth(username), "Only the user can sign themselves up");
    
    // Get a pointer to the users table.
    users_t users_table(get_self(), get_self().value);
    
    // Search for 'username' in the users table.
    auto user = users_table.find(username.value);
    
    // If the user is not found ( == users_table.end() ), then add the user to the table.
    check(user == users_table.end(), "User already signed up");

    // Add the user to the table.
    users_table.emplace(get_self(), [&](auto &u) {
        u.username = username;
        u.ethAddr = ethAddr;
        u.displayName = displayName;
        u.profilePic = profilePic;
        u.joined = time_point_sec(current_time_point());
    });
}

ACTION pollster::createpoll(
    const name &username,
    const std::string &name,
    const std::string &description,
    const vector<string> &options,
    const std::string &startDate,
    const std::string &endDate,
) {
    check(has_auth(username), "Can only create polls for yourself");

    // Make sure startDate is a future date or today
    // ! TODO: Write logic for check
    check(true, "Poll start date must be greater than today.");

    // Check that endDate is either greater than today and greater than start date.
    // ! TODO: Write logic for check
    check(true, "Poll end date must be greater then today and poll's start date.");

    // Get polls table, unscoped.
    polls_t polls_table(get_self(), get_self().value);

    polls_table.emplace(get_self(), [&](auto &p) {
        p.id = polls_table.available_primary_key();
        p.user = username;
        p.name = name;
        p.description = description;
        p.options = options;
        p.startDate = time_point_sec(seconds(startDate));
        p.endDate = time_point_sec(seconds(endDate));
    });
}

ACTION pollster::createvote(
    const name &username,
    const uint64_t &pollId,
    const uint8_t &optionIdx,
) {
    // ! TODO: Write logic for check
    check(has_auth(username), "Can only create votes for yourself");

    // Get polls table, unscoped.
    polls_t polls_table(get_self(), get_self().value);
    auto poll = polls_table.find(pollId);

    // Make sure poll exists
    check(poll != polls_table.end(), "Poll does not exist.");

    // Votes table scoped to poll.id
    votes_t votes_table(get_self(), poll.id);
    auto vote = votes_table.find(username.value);

    // Make sure user hasn't already votes
    // ! TODO: Write logic for check
    check(vote == votes_table.end(), "You can only vote once per poll.");

    votes_table.emplace(get_self(), [&](auto &v) {
        v.user = username;
        v.pollId = pollId;
        v.optionIdx = optionIdx;
    });
}


// --------- Testing Actions ----------
// Owner only ( Should be removed before "live" )
ACTION pollster::clearusers() {
    require_auth(get_self());

    users_t users_table(get_self(), get_self().value);
    auto itr = users_table.begin();
    while (itr != users_table.end()) {
        itr = users_table.erase(itr);
    }
}

ACTION pollster::clearpolls() {
    require_auth(get_self());

    polls_t polls_table(get_self(), get_self().value);
    auto itr = polls_table.begin();
    while (itr != polls_table.end()) {
        itr = polls_table.erase(itr);
    }
}

ACTION pollster::clearvotes() {
    require_auth(get_self());

    votes_t votes_table(get_self(), get_self().value);
    auto itr = votes_table.begin();
    while (itr != votes_table.end()) {
        itr = votes_table.erase(itr);
    }
}