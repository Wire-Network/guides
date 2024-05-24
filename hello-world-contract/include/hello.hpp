#pragma once

#include <eosio/eosio.hpp>
#include <string>

using namespace eosio;

CONTRACT hello : public contract {
public:
    using contract::contract;

    ACTION hi(name user);
};
