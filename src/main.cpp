#include <cstdio>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>
using num_type = int;
int PARTY_COUNT = 2;
int MODULUS = 1 << 25;

num_type mod(num_type num_a, num_type num_b) {
  if (num_b <= 0)
    exit(1);
  return num_a >= 0 ? num_a % num_b : (num_a % num_b + num_b) % num_b;
}

std::vector<num_type> create_shares(num_type plain_num) {
  std::vector<num_type> shares(PARTY_COUNT);
  std::random_device random_generater;
  num_type mod_of_plain_num = mod(plain_num, MODULUS);
  num_type mod_of_shares_sum = 0;
  for (int share_i = 0; share_i < PARTY_COUNT; share_i++) {
    if (share_i + 1 == PARTY_COUNT) {
      num_type random_num = random_generater();
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

int main() {
  num_type plain_num;
  do {
    std::cout << "Enter number ( < " << MODULUS << " ): ";
    std::cin >> plain_num;
  } while (plain_num >= MODULUS);
  std::vector<num_type> shares_of_a = create_shares(plain_num);
  std::printf(
      "input num ≡ %d, sum of shares ≡ %d\n", mod(plain_num, MODULUS),
      mod(std::accumulate(shares_of_a.begin(), shares_of_a.end(), 0), MODULUS));
  for (auto share : shares_of_a) {
    std::printf("share: %d\n", share);
  }
  return 0;
}
