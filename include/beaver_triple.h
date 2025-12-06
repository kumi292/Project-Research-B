#pragma once

#include <vector>

using NumType = long long;
using SharesType = std::vector<NumType>;
constexpr int PARTY_COUNT = 2;
constexpr NumType MODULUS = 1LL << 28;
constexpr NumType RANDOM_MAX = 1LL << 29;
constexpr NumType TRIPLE_MAX = 1LL << 13;

NumType mod(NumType dividend, NumType divisor);

namespace BT {

SharesType create_shares(NumType plain_num);
NumType reconstruct_from_shares(SharesType shares);
SharesType add(std::vector<SharesType> parties_with_shares);
SharesType multiply(std::vector<SharesType> parties_with_shares);
SharesType inner_product(std::vector<SharesType> party1_shares,
                         std::vector<SharesType> party2_shares);

} // namespace BT
