#pragma once

#include <vector>

#include "consts_and_types.h"

NumType mod(NumType dividend, NumType divisor);

namespace Replicated {

SharesType create_shares(NumType plain_num);
NumType reconstruct_from_shares(ShareType share_1, ShareType share_2);
SharesType add(std::vector<SharesType> parties_with_shares);
SharesType multiply(std::vector<SharesType> parties_with_shares);

} // namespace Replicated
