#include <iostream>
#include <numeric>
#include <string>

#include "replicated.h"

void set_number_from_stdin(NumType &input_num, int repetition = 0) {
  do {
    if (repetition == 0)
      std::cout << "Enter number ( 0 ~ " << MODULUS - 1 << " ): ";
    std::cin >> input_num;
  } while (input_num >= MODULUS);
}

std::string create_string_expression(ShareType share) {
  return "(" + std::to_string(std::get<0>(share)) + ", " +
         std::to_string(std::get<1>(share)) + ")";
}

int test_addition_and_multiplication(void) {
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

  exit(0);
}

int test_inner_product() {
  int vec_size;
  std::cout << "Vector size: ";
  std::cin >> vec_size;
  std::vector<NumType> vector_a(vec_size), vector_b(vec_size);
  std::cout << "Enter Vectors (use space for separator)\n";
  std::cout << "Vector A: \n";
  for (int component_i = 0; component_i < vec_size; component_i++) {
    set_number_from_stdin(vector_a[component_i], component_i);
  }
  std::cout << "Vector B: \n";
  for (int component_i = 0; component_i < vec_size; component_i++) {
    set_number_from_stdin(vector_b[component_i], component_i);
  }
  std::cout << std::endl;

  std::vector<SharesType> party1_shares(vec_size), party2_shares(vec_size),
      party3_shares(vec_size);
  for (int component_i = 0; component_i < vec_size; component_i++) {
    SharesType veca_componenti_shares =
        Replicated::create_shares(vector_a[component_i]);
    SharesType vecb_componenti_shares =
        Replicated::create_shares(vector_b[component_i]);
    party1_shares[0].push_back(veca_componenti_shares[0]);
    party1_shares[1].push_back(vecb_componenti_shares[0]);
    party2_shares[0].push_back(veca_componenti_shares[1]);
    party2_shares[1].push_back(vecb_componenti_shares[1]);
    party3_shares[0].push_back(veca_componenti_shares[2]);
    party3_shares[1].push_back(vecb_componenti_shares[2]);
  }

  SharesType shares_inner_product =
      Replicated::inner_product(party1_shares, party2_shares, party3_shares);
  std::cout << "[Inner Product]" << std::endl;
  std::cout << "calculated shares: {"
            << create_string_expression(shares_inner_product[0]) << ", "
            << create_string_expression(shares_inner_product[1]) << ", "
            << create_string_expression(shares_inner_product[2]) << "}\n";
  std::cout << "Inner Product of vectors (using secret sharing) is "
            << Replicated::reconstruct_from_shares(shares_inner_product[0],
                                                   shares_inner_product[1])
            << std::endl;
  std::cout << "Inner Product of vectors (not using secret sharing) is "
            << std::inner_product(vector_a.begin(), vector_a.end(),
                                  vector_b.begin(), 0LL)
            << std::endl;
  std::cout << "Communication Cost: " << Replicated::communication_cost
            << std::endl;
  exit(0);
}

int main() {
  std::cout << "Which one to test\n"
               "a: addition and multiplication\n"
               "b: inner product\n";
  std::string inputted_char;
  while (true) {
    std::cout << "Enter alphabet: ";
    std::cin >> inputted_char;
    if (inputted_char == "a") {
      std::cout << std::endl;
      test_addition_and_multiplication();
    } else if (inputted_char == "b") {
      std::cout << std::endl;
      test_inner_product();
    } else {
      std::cout << "Sorry, try again\n";
    }
  }
}
