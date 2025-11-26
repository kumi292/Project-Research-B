#include <random>
#include <vector>

#include "beaver_triple.h"

NumType mod(NumType dividend, NumType divisor) {
  if (divisor <= 0)
    exit(1);
  return dividend >= 0 ? dividend % divisor
                       : (dividend % divisor + divisor) % divisor;
}

namespace BT {

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
      shares[share_i] =
          random_num +
          (mod_of_plain_num - (mod_of_shares_sum + mod(random_num, MODULUS)));
    } else {
      shares[share_i] = dist(engine);
      mod_of_shares_sum =
          mod(mod_of_shares_sum + mod(shares[share_i], MODULUS), MODULUS);
    }
  }
  return shares;
}

NumType reconstruct_from_shares(SharesType shares) {
  NumType reconstructed_num = 0;
  for (auto share : shares) {
    reconstructed_num = mod(reconstructed_num + mod(share, MODULUS), MODULUS);
  }
  return reconstructed_num;
}

SharesType add(std::vector<SharesType> parties_with_shares) {
  SharesType added_shares;
  for (int party_i = 0; party_i < PARTY_COUNT; party_i++) {
    added_shares.push_back(parties_with_shares[party_i][0] +
                           parties_with_shares[party_i][1]);
  }
  return added_shares;
}

} // namespace BT
