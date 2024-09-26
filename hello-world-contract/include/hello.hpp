#pragma once

#include <sysio/sysio.hpp>
#include <string>

using namespace sysio;

CONTRACT hello : public contract {
public:
    using contract::contract;

    ACTION hi(name user);
};
