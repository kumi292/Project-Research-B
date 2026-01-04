#pragma once

#include <string>
#include <vector>

#include "common_functions.h"

using SharesType = std::vector<NumType>;
constexpr int PARTY_COUNT = 2;
constexpr NumType MODULUS = 1LL << 28;
constexpr NumType RANDOM_MAX = 1LL << 29;
constexpr NumType TRIPLE_MAX = 1LL << 13;
const std::string DB_FILE_1 = "src/additive_communication/db_server_1.json";
const std::string DB_FILE_2 = "src/additive_communication/db_server_2.json";
const std::string SEND_TRIPLE = "SEND TRIPLE";
const std::string PORT = "10000";

namespace BT {

inline int communication_cost = 0;
SharesType create_shares(NumType plain_num);
NumType reconstruct_from_shares(SharesType shares);
SharesType add(std::vector<SharesType> parties_with_shares);
SharesType multiply(std::vector<SharesType> parties_with_shares);
SharesType inner_product(std::vector<SharesType> party1_shares,
                         std::vector<SharesType> party2_shares);
void print_all_table(std::vector<NumType> &table, int limit, bool tail = false);
void print_id_value_as_table(int id, NumType value);

} // namespace BT
