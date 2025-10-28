#include <iostream>

#include "additive.h"
#include "consts_and_types.h"

int main() {
  NumType plain_num;
  do {
    std::cout << "Enter number ( 0 ~ " << MODULUS - 1 << " ): ";
    std::cin >> plain_num;
  } while (plain_num >= MODULUS);
  SharesType shares = Additive::create_shares(plain_num);
  for (int share_i = 0; share_i < PARTY_COUNT; share_i++) {
    std::cout << "share_" << share_i << ": " << shares[share_i] << std::endl;
  }
  std::cout << "Reconstructed number is "
            << Additive::reconstruct_from_shares(shares) << std::endl;
  return 0;
}
