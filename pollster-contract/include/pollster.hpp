#pragma once

#include <sysio/sysio.hpp>
#include <string>
#include <vector>

using namespace sysio;

CONTRACT pollster : public contract {
    public:
        using contract::contract;

    /**
     * @brief Signup a user to participate in creating polls.
     * 
     * @param username The Wire account name of the user
     * @param ethAddr The Ethereum address of the user
     * @param displayName The desired display name of the user, NOT UNIQUE.
     * @param profilePic A link to the profile picture of the user, default will be current dojo's logo.
     * @return ACTION 
     */
    ACTION signup(const name &username, const std::string &ethAddr, const std::string &displayName, const std::string &profilePic);
    
    /**
     * @brief Create a poll.
     * 
     * @param username The Wire account name of the user
     * @param name The name of the poll
     * @param description Short description of the poll
     * @param options Vector of poll options
     * @param startDate epoch timestamp of when voting begins
     * @param endDate epoch timestamp of when voting ends
     * @return ACTION 
     */
    ACTION createpoll(name username, const std::string &name, const std::string &description, const vector<string> &options, uint32_t &startDate, uint32_t &endDate);

    /**
     * @brief Create a poll.
     * 
     * @param username The Wire account name of the user
     * @param pollId ID of poll to associate vote with
     * @param optionIdx Index of associated poll option vote is in favor of
     * @return ACTION 
     */
    ACTION createvote(name username, const uint64_t& pollId, const uint64_t& optionIdx);

    // ! ALL ACTIONS BELOW ARE FOR TESTING PURPOSES ONLY. Should be ran then removed before going live.
    ACTION clearusers();
    ACTION clearpolls();
    ACTION clearvotes();

    private:
        /**
         * @brief Poll struct for individual polls, indexed by id and by creator;
         */
        TABLE poll_s {
            uint64_t id;                    // UID for the poll row
            std::string name;               // Display name of the poll
            std::string description;        // Description of the poll
            vector<string> options;         // Vector of poll options
            name username;                      // account of poll creation
            time_point_sec startDate;       // epoch timestamp for when polling starts (1732044034)
            time_point_sec endDate;         // epoch timestamp for when polling ends (1732924800)

            uint64_t primary_key() const { return id; }
            uint64_t by_username() const { return username.value; }
        };
        
        typedef multi_index<"polls"_n, poll_s, 
            indexed_by<"username"_n, const_mem_fun<polls, uint64_t, &polls::by_username>>
        > polls_t;

        /**
         * @brief Vote struct for individual votes, indexed by user and by poll_id;
         */
        TABLE vote_s {
            name username;              // Account for creator voting
            uint64_t pollId;            // UID for poll relation
            uint8_t optionIdx;         // Index for poll option selected

            uint64_t primary_key() const { return username.value };
            uint64_t by_poll() const { return pollId; }
        }

        typedef multi_index<"votes"_n, vote_s, 
            indexed_by<"pollId"_n, const_mem_fun<polls, uint64_t, &polls::by_poll>>
        > votes_t;

        /**
         * @brief Users table to store the users that have signed up to participate in creating polls.
         */
        TABLE user_s {
            name username;                  // Account name of the user
            std::string ethAddr;            // Ethereum address of the user
            std::string displayName;        // Display name of the user
            std::string profilePic;         // A link to profile picture of the user
            time_point_sec joined;          // The time when the user signed up ( UTC time )

            uint64_t primary_key() const { return username.value; }
        };

        typedef multi_index<"users"_n, user_s> users_t;
};