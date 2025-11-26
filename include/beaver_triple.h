#pragma once

#include <vector>

using NumType = long long;
using SharesType = std::vector<NumType>;
constexpr int PARTY_COUNT = 2;
constexpr NumType MODULUS = 1LL << 28;

NumType mod(NumType dividend, NumType divisor);

namespace BT {

SharesType create_shares(NumType plain_num);
NumType reconstruct_from_shares(SharesType shares);
SharesType add(std::vector<SharesType> parties_with_shares);

} // namespace BT
