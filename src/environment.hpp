#pragma once
#include <cstdint>

namespace environment {



uint64_t FindEnv();

static bool IsClient() { return (FindEnv() != 4); }
static bool IsServer() { return (FindEnv() == 4); }

}// namespace environment