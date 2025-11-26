#pragma once

#include <vector>

#include "consts_and_types.h"

NumType mod(NumType dividend, NumType divisor);

namespace BT {

SharesType create_shares(NumType plain_num);

NumType reconstruct_from_shares(SharesType shares);

SharesType add(std::vector<SharesType> parties_with_shares);

} // namespace BT
