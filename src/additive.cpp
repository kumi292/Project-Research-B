#include "additive.h"

#include <numeric>
#include <random>
#include <set>
#include <vector>

#include "consts_and_types.h"

NumType mod(NumType dividend, NumType divisor) {
  if (divisor <= 0)
    exit(1);
  return dividend >= 0 ? dividend % divisor
                       : (dividend % divisor + divisor) % divisor;
}

namespace Additive {

SharesType create_shares(NumType plain_num) {
  if (plain_num >= MODULUS)
    exit(1);
  SharesType shares(PARTY_COUNT);
  std::random_device random_generater;
  NumType mod_of_plain_num = mod(plain_num, MODULUS);
  NumType mod_of_shares_sum = 0;
  for (int share_i = 0; share_i < PARTY_COUNT; share_i++) {
    if (share_i + 1 == PARTY_COUNT) {
      NumType random_num = random_generater();
      std::get<0>(shares[share_i]) =
          random_num +
          (mod_of_plain_num - (mod_of_shares_sum + mod(random_num, MODULUS)));
      std::get<1>(shares[mod(share_i + 1, PARTY_COUNT)]) =
          std::get<0>(shares[share_i]);
    } else {
      std::get<0>(shares[share_i]) = random_generater();
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

} // namespace Additive
