#include <iostream>
#include <string>

#include "replicated.h"

void set_number_from_stdin(NumType &input_num) {
  do {
    std::cout << "Enter number ( 0 ~ " << MODULUS - 1 << " ): ";
    std::cin >> input_num;
  } while (input_num >= MODULUS);
}

std::string create_string_expression(ShareType share) {
  return "(" + std::to_string(std::get<0>(share)) + ", " +
         std::to_string(std::get<1>(share)) + ")";
}

int main() {
  NumType plain_num_a, plain_num_b;

  std::cout << "Number A: \n";
  set_number_from_stdin(plain_num_a);
  SharesType shares_a = Replicated::create_shares(plain_num_a);
  std::cout << "[Generating Shares...]" << std::endl;
  std::cout << "Party 1 Share: " << create_string_expression(shares_a[0])
            << std::endl;
  std::cout << "Party 2 Share: " << create_string_expression(shares_a[1])
            << std::endl;
  std::cout << "Party 3 Share: " << create_string_expression(shares_a[2])
            << std::endl;
  std::cout << std::endl;
  std::cout << "Number B: \n";
  set_number_from_stdin(plain_num_b);
  SharesType shares_b = Replicated::create_shares(plain_num_b);
  std::cout << "[Generating Shares...]" << std::endl;
  std::cout << "Party 1 Share: " << create_string_expression(shares_b[0])
            << std::endl;
  std::cout << "Party 2 Share: " << create_string_expression(shares_b[1])
            << std::endl;
  std::cout << "Party 3 Share: " << create_string_expression(shares_b[2])
            << std::endl;
  std::cout << std::endl;
  SharesType shares_party_1 = {shares_a[0], shares_b[0]};
  SharesType shares_party_2 = {shares_a[1], shares_b[1]};
  SharesType shares_party_3 = {shares_a[2], shares_b[2]};

  SharesType shares_added =
      Replicated::add({shares_party_1, shares_party_2, shares_party_3});
  std::cout << "[Addition]" << std::endl;
  std::cout << "Party 1: " << create_string_expression(shares_party_1[0])
            << " + " << create_string_expression(shares_party_1[1]) << " = "
            << create_string_expression(shares_added[0]) << std::endl;
  std::cout << "Party 2: " << create_string_expression(shares_party_2[0])
            << " + " << create_string_expression(shares_party_2[1]) << " = "
            << create_string_expression(shares_added[1]) << std::endl;
  std::cout << "Party 3: " << create_string_expression(shares_party_3[0])
            << " + " << create_string_expression(shares_party_3[1]) << " = "
            << create_string_expression(shares_added[2]) << std::endl;
  std::cout << std::endl;

  std::cout << "[Reconstruction]" << std::endl;
  std::cout << "Reconst(Party 1, Party 2): "
            << Replicated::reconstruct_from_shares(shares_added[0],
                                                   shares_added[1])
            << std::endl;
  std::cout << "Reconst(Party 1, Party 3): "
            << Replicated::reconstruct_from_shares(shares_added[0],
                                                   shares_added[2])
            << std::endl;
  std::cout << "Reconst(Party 2, Party 3): "
            << Replicated::reconstruct_from_shares(shares_added[1],
                                                   shares_added[2])
            << std::endl;
  std::cout << "Sum of Plain Numbers: " << plain_num_a + plain_num_b
            << std::endl;
  std::cout << std::endl;

  SharesType shares_multiplied =
      Replicated::multiply({shares_party_1, shares_party_2, shares_party_3});
  std::cout << "[Multiplication]" << std::endl;
  std::cout << "Party 1: " << create_string_expression(shares_multiplied[0])
            << std::endl;
  std::cout << "Party 2: " << create_string_expression(shares_multiplied[1])
            << std::endl;
  std::cout << "Party 3: " << create_string_expression(shares_multiplied[2])
            << std::endl;
  std::cout << std::endl;

  std::cout << "[Reconstruction]" << std::endl;
  std::cout << "Reconst(Party 1, Party 2): "
            << Replicated::reconstruct_from_shares(shares_multiplied[0],
                                                   shares_multiplied[1])
            << std::endl;
  std::cout << "Reconst(Party 1, Party 3): "
            << Replicated::reconstruct_from_shares(shares_multiplied[0],
                                                   shares_multiplied[2])
            << std::endl;
  std::cout << "Reconst(Party 2, Party 3): "
            << Replicated::reconstruct_from_shares(shares_multiplied[1],
                                                   shares_multiplied[2])
            << std::endl;
  std::cout << "Product of Plain Numbers: " << plain_num_a * plain_num_b
            << std::endl;

  return 0;
}
