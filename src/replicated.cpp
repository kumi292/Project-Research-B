#include "replicated.h"

#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <stdexcept>
#include <vector>

#include "common_functions.h"

namespace Replicated {

SharesType create_shares(NumType plain_num) {
  if (plain_num >= MODULUS)
    throw std::invalid_argument(
        "create_shares: input number must be positive\n");
  SharesType shares(PARTY_COUNT);
  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());
  std::uniform_int_distribution<NumType> dist(0, RANDOM_MAX);
  NumType mod_of_plain_num = mod(plain_num, MODULUS);
  NumType mod_of_shares_sum = 0;
  for (int share_i = 0; share_i < PARTY_COUNT; share_i++) {
    if (share_i + 1 == PARTY_COUNT) {
      NumType random_num = dist(engine);
      std::get<0>(shares[share_i]) =
          random_num +
          (mod_of_plain_num - (mod_of_shares_sum + mod(random_num, MODULUS)));
      std::get<1>(shares[mod(share_i + 1, PARTY_COUNT)]) =
          std::get<0>(shares[share_i]);
    } else {
      std::get<0>(shares[share_i]) = dist(engine);
      std::get<1>(shares[mod(share_i + 1, PARTY_COUNT)]) =
          std::get<0>(shares[share_i]);
      mod_of_shares_sum =
          mod(mod_of_shares_sum + mod(std::get<0>(shares[share_i]), MODULUS),
              MODULUS);
    }
  }
  return shares;
}

NumType reconstruct_from_shares(ShareType share_1, ShareType share_2) {
  std::set<NumType> shares = {std::get<0>(share_1), std::get<1>(share_1),
                              std::get<0>(share_2), std::get<1>(share_2)};
  return mod(std::reduce(shares.begin(), shares.end()), MODULUS);
}

SharesType add(std::vector<SharesType> parties_with_shares) {
  SharesType added_shares;
  for (int party_i = 0; party_i < PARTY_COUNT; party_i++) {
    added_shares.push_back({(std::get<0>(parties_with_shares[party_i][0]) +
                             std::get<0>(parties_with_shares[party_i][1])),
                            (std::get<1>(parties_with_shares[party_i][0]) +
                             std::get<1>(parties_with_shares[party_i][1]))});
  }
  return added_shares;
}

SharesType multiply(std::vector<SharesType> parties_with_shares) {
  SharesType multiplied_shares(PARTY_COUNT);
  for (int party_i = 0; party_i < PARTY_COUNT; party_i++) {
    NumType one_of_new_share =
        std::get<0>(parties_with_shares[party_i][0]) *
            std::get<0>(parties_with_shares[party_i][1]) +
        std::get<0>(parties_with_shares[party_i][0]) *
            std::get<1>(parties_with_shares[party_i][1]) +
        std::get<1>(parties_with_shares[party_i][0]) *
            std::get<0>(parties_with_shares[party_i][1]);
    std::get<0>(multiplied_shares[party_i]) = one_of_new_share;
    std::get<1>(multiplied_shares[mod(party_i + 1, PARTY_COUNT)]) =
        one_of_new_share;
    communication_cost++;
  }
  return multiplied_shares;
}

SharesType inner_product(std::vector<SharesType> party1_shares,
                         std::vector<SharesType> party2_shares,
                         std::vector<SharesType> party3_shares) {
  int vec_size = party1_shares[0].size();
  SharesType calculated_shares = {{0, 0}, {0, 0}, {0, 0}};
  int party_i = 0;
  for (auto party_shares : {party1_shares, party2_shares, party3_shares}) {
    NumType one_of_calclulated_share = 0LL;
    for (int component_i = 0; component_i < vec_size; component_i++) {
      NumType multiplied_value = std::get<0>(party_shares[0][component_i]) *
                                     std::get<0>(party_shares[1][component_i]) +
                                 std::get<0>(party_shares[0][component_i]) *
                                     std::get<1>(party_shares[1][component_i]) +
                                 std::get<1>(party_shares[0][component_i]) *
                                     std::get<0>(party_shares[1][component_i]);
      one_of_calclulated_share = one_of_calclulated_share + multiplied_value;
    }
    std::get<0>(calculated_shares[party_i]) = one_of_calclulated_share;
    std::get<1>(calculated_shares[mod(party_i + 1, PARTY_COUNT)]) =
        one_of_calclulated_share;
    communication_cost++;
    party_i++;
  }

  return calculated_shares;
}

void print_all_table(std::vector<NumType> &column1,
                     std::vector<NumType> &column2, int limit, bool tail) {
  const int db_size = column1.size();
  int id_i = tail && db_size - limit >= 0 ? db_size - limit : 0;
  std::printf("+--------+--------------+--------------+\n");
  std::printf("|%3sid%3s|%4sshare1%4s|%4sshare2%4s|\n", "", "", "", "", "", "");
  std::printf("+--------+--------------+\n");
  for (int print_count = 0; id_i < db_size && print_count < limit;
       id_i++, print_count++) {
    std::printf("|%5d%3s|%12lld%2s|%12lld%2s|\n", id_i, "", column1[id_i], "",
                column2[id_i], "");
  }
  std::printf("+--------+--------------+--------------+\n");
  std::cout << "(total " << db_size << " records)" << std::endl;
}

void print_id_value_as_table(int id, NumType value) {
  std::printf("+--------+--------------+\n");
  std::printf("|%3sid%3s|%5sdata%5s|\n", "", "", "", "");
  std::printf("+--------+--------------+\n");
  std::printf("|%5d%3s|%12lld%2s|\n", id, "", value, "");
  std::printf("+--------+--------------+\n");
}

} // namespace Replicated
