#include <cstdio>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "beaver_triple.h"
#include "common_functions.h"

int database_size = 1000;
std::vector<NumType> database(database_size);
NumType share_a, share_b;

void insert_values_into_db() {
  for (int i = 0; i < database_size; i++) {
    database[i] = i + 1000;
  }
}

int execute_query(int id_to_select) {
  std::vector<int> search_vector(database_size);
  search_vector[id_to_select] = 1;

  std::vector<NumType> search_vector_share_for_server_a(database_size),
      search_vector_share_for_server_b(database_size);
  for (int id_i = 0; id_i < database_size; id_i++) {
    SharesType id_i_shares = BT::create_shares(search_vector[id_i]);
    search_vector_share_for_server_a[id_i] = id_i_shares[0];
    search_vector_share_for_server_b[id_i] = id_i_shares[1];
  }

  NumType share_of_result_from_server_a, share_of_result_from_server_b;
  share_of_result_from_server_a =
      std::inner_product(database.begin(), database.end(),
                         search_vector_share_for_server_a.begin(), 0LL);
  share_a = share_of_result_from_server_a;
  share_of_result_from_server_b =
      std::inner_product(database.begin(), database.end(),
                         search_vector_share_for_server_b.begin(), 0LL);
  share_b = share_of_result_from_server_b;
  int query_result = BT::reconstruct_from_shares(
      {share_of_result_from_server_a, share_of_result_from_server_b});

  return query_result;
}

int main() {
  insert_values_into_db();
  std::string selector;
  int id_to_select;
  int result;
  do {
    std::cout << "SELECT data FROM db WHERE id = ";
    std::cin >> id_to_select;
    result = execute_query(id_to_select);
    std::cout << "value from db server A: " << share_a << std::endl;
    std::cout << "value from db server B: " << share_b << std::endl;
    std::cout << "----------\n";
    std::cout << "|  data  |\n";
    std::cout << "----------\n";
    std::printf("|%6d  |\n", result);
    std::cout << "----------\n";
    std::cout << "Enter 'c' to execute query, the others to stop: ";
    std::cin >> selector;
  } while (selector == "c");
  return 0;
}
