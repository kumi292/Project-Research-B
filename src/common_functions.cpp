#include "common_functions.h"

#include <stdexcept>
#include <vector>

NumType mod(NumType dividend, NumType divisor) {
  if (divisor <= 0)
    throw std::invalid_argument("mod: divisor must be positive\n");
  return dividend >= 0 ? dividend % divisor
                       : (dividend % divisor + divisor) % divisor;
}

void print_table(std::vector<NumType> &table, int limit, bool reverse) {
  const int db_size = table.size();
  int id_i = reverse && db_size - limit >= 0 ? db_size - limit : 0;
  std::printf("+--------+--------------+\n");
  std::printf("|%3sid%3s|%5sdata%5s|\n", "", "", "", "");
  std::printf("+--------+--------------+\n");
  for (int print_count = 0; id_i < db_size && print_count < limit;
       id_i++, print_count++) {
    std::printf("|%5d%3s|%12lld%2s|\n", id_i, "", table[id_i], "");
  }
  std::printf("+--------+--------------+\n");
}
