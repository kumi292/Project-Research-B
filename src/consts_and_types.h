#pragma once

#include <tuple>
#include <vector>

using NumType = long long;
using ShareType = std::tuple<NumType, NumType>;
using SharesType = std::vector<ShareType>;
constexpr int PARTY_COUNT = 3;
constexpr int ACCESS_COUNT = 2;
constexpr NumType MODULUS = 1LL << 28;
constexpr NumType RANDOM_MAX = 1LL << 29;
