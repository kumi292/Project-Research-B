#pragma once

#include <string>
#include <tuple>
#include <vector>

#include "common_functions.h"

using ShareType = std::tuple<NumType, NumType>;
using SharesType = std::vector<ShareType>;
constexpr int PARTY_COUNT = 3;
constexpr int ACCESS_COUNT = 2;
constexpr NumType MODULUS = 1LL << 28;
constexpr NumType RANDOM_MAX = 1LL << 29;
const std::string DB_FILE_1 = "src/replicated_communication/db_server_1.json";
const std::string DB_FILE_2 = "src/replicated_communication/db_server_2.json";
const std::string DB_FILE_3 = "src/replicated_communication/db_server_3.json";

namespace Replicated {

inline int communication_cost = 0;
SharesType create_shares(NumType plain_num);
NumType reconstruct_from_shares(ShareType share_1, ShareType share_2);
SharesType add(std::vector<SharesType> parties_with_shares);
SharesType multiply(std::vector<SharesType> parties_with_shares);
SharesType inner_product(std::vector<SharesType> party1_shares,
                         std::vector<SharesType> party2_shares,
                         std::vector<SharesType> party3_shares);

} // namespace Replicated
