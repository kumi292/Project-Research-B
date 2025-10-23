#include <iostream>
#include <random>
#include <vector>
using NumType = int;
using SharesType = std::vector<NumType>;
const int PARTY_COUNT = 2;
const int MODULUS = 1 << 25;

NumType mod(NumType dividend, NumType divisor) {
  if (divisor <= 0)
    exit(1);
  return dividend >= 0 ? dividend % divisor
                       : (dividend % divisor + divisor) % divisor;
}

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
      mod_of_shares_sum += mod(mod(shares[share_i], MODULUS), MODULUS);
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

int main() {
  NumType plain_num;
  do {
    std::cout << "Enter number ( 0 ~ " << MODULUS - 1 << " ): ";
    std::cin >> plain_num;
  } while (plain_num >= MODULUS);
  SharesType shares = create_shares(plain_num);
  for (int share_i = 0; share_i < PARTY_COUNT; share_i++) {
    std::cout << "share_" << share_i << ": " << shares[share_i] << std::endl;
  }
  std::cout << "Reconstructed number is " << reconstruct_from_shares(shares)
            << std::endl;
  return 0;
}
