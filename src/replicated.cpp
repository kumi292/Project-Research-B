#include "replicated.h"

#include <numeric>
#include <random>
#include <set>
#include <vector>

NumType mod(NumType dividend, NumType divisor) {
  if (divisor <= 0)
    exit(1);
  return dividend >= 0 ? dividend % divisor
                       : (dividend % divisor + divisor) % divisor;
}

namespace Replicated {

SharesType create_shares(NumType plain_num) {
  if (plain_num >= MODULUS)
    exit(1);
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
  }
  return multiplied_shares;
}

SharesType inner_product(std::vector<SharesType> party1_shares,
                         std::vector<SharesType> party2_shares,
                         std::vector<SharesType> party3_shares) {
  int vec_size = party1_shares[0].size();
  std::vector<SharesType> multiplied_shares_vec;
  for (int component_i = 0; component_i < vec_size; component_i++) {
    multiplied_shares_vec.push_back(multiply(
        {{party1_shares[0][component_i], party1_shares[1][component_i]},
         {party2_shares[0][component_i], party2_shares[1][component_i]},
         {party3_shares[0][component_i], party3_shares[1][component_i]}}));
  }

  SharesType calculated_shares = {{0, 0}, {0, 0}, {0, 0}};
  for (int component_i = 0; component_i < vec_size; component_i++) {
    calculated_shares =
        add({{calculated_shares[0], multiplied_shares_vec[component_i][0]},
             {calculated_shares[1], multiplied_shares_vec[component_i][1]},
             {calculated_shares[2], multiplied_shares_vec[component_i][2]}});
  }

  return calculated_shares;
}

} // namespace Replicated
