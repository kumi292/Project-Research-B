#include <random>

#include "additive.h"
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
      shares[share_i] =
          random_num +
          (mod_of_plain_num - (mod_of_shares_sum + mod(random_num, MODULUS)));
    } else {
      shares[share_i] = random_generater();
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

} // namespace Additive
