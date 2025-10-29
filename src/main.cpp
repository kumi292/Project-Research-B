#include <iostream>

#include "additive.h"
#include "consts_and_types.h"

void set_number_from_stdin(NumType &input_num) {
  do {
    std::cout << "Enter number ( 0 ~ " << MODULUS - 1 << " ): ";
    std::cin >> input_num;
  } while (input_num >= MODULUS);
}

int main() {
  NumType plain_num_a, plain_num_b;
  std::cout << "Number A: \n";
  set_number_from_stdin(plain_num_a);
  std::cout << "Number B: \n";
  set_number_from_stdin(plain_num_b);
  SharesType shares_a = Additive::create_shares(plain_num_a);
  SharesType shares_b = Additive::create_shares(plain_num_b);
  SharesType shares_alice = {shares_a[0], shares_b[0]};
  SharesType shares_bob = {shares_a[1], shares_b[1]};
  SharesType shares_added = Additive::add({shares_alice, shares_bob});
  std::cout << "Sum of numbers (using additive secret sharing scheme) is "
            << Additive::reconstruct_from_shares(shares_added) << std::endl;
  std::cout << "Sum of numbers (not using secret sharing scheme) is "
            << plain_num_a + plain_num_b << std::endl;
  return 0;
}
