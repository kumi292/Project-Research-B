#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "beaver_triple.h"

void set_number_from_stdin(NumType &input_num, int repetition = 0) {
  do {
    if (repetition == 0)
      std::cout << "Enter number ( 0 ~ " << MODULUS - 1 << " ): ";
    std::cin >> input_num;
  } while (input_num >= MODULUS);
}

int test_addition_and_multiplication() {
  NumType plain_num_a, plain_num_b;
  std::cout << "Number A: \n";
  set_number_from_stdin(plain_num_a);
  SharesType shares_a = BT::create_shares(plain_num_a);
  std::cout << "[Generating Shares...]" << std::endl;
  std::cout << "Party 1 Share: " << shares_a[0] << std::endl;
  std::cout << "Party 2 Share: " << shares_a[1] << std::endl;
  std::cout << std::endl;

  std::cout << "Number B: \n";
  set_number_from_stdin(plain_num_b);
  SharesType shares_b = BT::create_shares(plain_num_b);
  std::cout << "[Generating Shares...]" << std::endl;
  std::cout << "Party 1 Share: " << shares_b[0] << std::endl;
  std::cout << "Party 2 Share: " << shares_b[1] << std::endl;
  std::cout << std::endl;

  SharesType shares_party_1 = {shares_a[0], shares_b[0]};
  SharesType shares_party_2 = {shares_a[1], shares_b[1]};

  SharesType shares_added = BT::add({shares_party_1, shares_party_2});
  std::cout << "[Addition]" << std::endl;
  std::cout << "Party 1: " << shares_party_1[0] << " + " << shares_party_1[1]
            << " = " << shares_added[0] << std::endl;
  std::cout << "Party 2: " << shares_party_2[0] << " + " << shares_party_2[1]
            << " = " << shares_added[1] << std::endl;
  std::cout << std::endl;
  std::cout << "Sum of numbers (using additive secret sharing scheme) is "
            << BT::reconstruct_from_shares(shares_added) << std::endl;
  std::cout << "Sum of numbers (not using secret sharing scheme) is "
            << plain_num_a + plain_num_b << std::endl;
  std::cout << std::endl;

  SharesType shares_multiplied = BT::multiply({shares_party_1, shares_party_2});
  std::cout << "[Multiplication]" << std::endl;
  std::cout << "Product of numbers (using beaver triple) is "
            << BT::reconstruct_from_shares(shares_multiplied) << std::endl;
  std::cout << "Product of numbers (not using secret sharing scheme) is "
            << plain_num_a * plain_num_b << std::endl;
  exit(0);
}

int main() {
  std::cout << "Which one to test\n"
               "a: addition and multiplication\n"
               "b: inner_product\n";
  std::string inputted_char;
  while (true) {
    std::cout << "Enter alphabet: ";
    std::cin >> inputted_char;
    if (inputted_char == "a")
      test_addition_and_multiplication();
    else if (inputted_char == "b")
      test_inner_product();
    else {
      std::cout << "Sorry, try again\n";
    }
  }
}
