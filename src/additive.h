#pragma once

#include "consts_and_types.h"

NumType mod(NumType dividend, NumType divisor);

namespace Additive {

SharesType create_shares(NumType plain_num);

NumType reconstruct_from_shares(SharesType shares);

} // namespace Additive
