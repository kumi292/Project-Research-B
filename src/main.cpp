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
  SharesType shares_a = Additive::create_shares(plain_num_a);
  std::cout << "[Generating Shares...]" << std::endl;
  std::cout << "Party 1 Share: (" << std::get<0>(shares_a[0]) << ", "
            << std::get<1>(shares_a[0]) << ")" << std::endl;
  std::cout << "Party 2 Share: (" << std::get<0>(shares_a[1]) << ", "
            << std::get<1>(shares_a[1]) << ")" << std::endl;
  std::cout << "Party 3 Share: (" << std::get<0>(shares_a[2]) << ", "
            << std::get<1>(shares_a[2]) << ")" << std::endl;
  std::cout << std::endl;
  std::cout << "Number B: \n";
  set_number_from_stdin(plain_num_b);
  SharesType shares_b = Additive::create_shares(plain_num_b);
  std::cout << "[Generating Shares...]" << std::endl;
  std::cout << "Party 1 Share: (" << std::get<0>(shares_b[0]) << ", "
            << std::get<1>(shares_b[0]) << ")" << std::endl;
  std::cout << "Party 2 Share: (" << std::get<0>(shares_b[1]) << ", "
            << std::get<1>(shares_b[1]) << ")" << std::endl;
  std::cout << "Party 3 Share: (" << std::get<0>(shares_b[2]) << ", "
            << std::get<1>(shares_b[2]) << ")" << std::endl;
  std::cout << std::endl;
  SharesType shares_party_1 = {shares_a[0], shares_b[0]};
  SharesType shares_party_2 = {shares_a[1], shares_b[1]};
  SharesType shares_Party_3 = {shares_a[2], shares_b[2]};

  std::cout << "Reconstructed Number A: "
            << Additive::reconstruct_from_shares(shares_party_1[0],
                                                 shares_Party_3[0])
            << std::endl;
  std::cout << "Reconstructed Number B: "
            << Additive::reconstruct_from_shares(shares_party_2[1],
                                                 shares_party_1[1])
            << std::endl;

  // SharesType shares_added = Additive::add({shares_party_1,
  // shares_party_2});
  //  SharesType shares_multiplied =
  //      Additive::multiply({shares_party_1, shares_party_2});
  // std::cout << "[Local Addition]" << std::endl;
  // std::cout << "Party 1: " << shares_party_1[0] << " + " <<
  // shares_party_1[1]
  //          << " = " << shares_added[0] << std::endl;
  // std::cout << "Party 2: " << shares_party_2[0] << " + " <<
  // shares_party_2[1]
  //          << " = " << shares_added[1] << std::endl;
  // std::cout << std::endl;
  // std::cout << "[Local Multiplication]" << std::endl;
  // std::cout << "Party 1: " << shares_party_1[0] << " * " <<
  // shares_party_1[1]
  //          << " = " << shares_multiplied[0] << std::endl;
  // std::cout << "Party 2: " << shares_party_2[0] << " * " <<
  // shares_party_2[1]
  //          << " = " << shares_multiplied[1] << std::endl;
  // std::cout << std::endl;

  // std::cout << "Sum of numbers (using additive secret sharing scheme) is
  // "
  //           << Additive::reconstruct_from_shares(shares_added) <<
  //           std::endl;
  // std::cout << "Sum of numbers (not using secret sharing scheme) is "
  //           << plain_num_a + plain_num_b << std::endl;
  // std::cout << "Product of numbers (using additive secret sharing scheme)
  // is
  // "
  //           << Additive::reconstruct_from_shares(shares_multiplied)
  //           << std::endl;
  // std::cout << "Product of numbers (not using secret sharing scheme) is "
  //           << plain_num_a * plain_num_b << std::endl;

  return 0;
}
