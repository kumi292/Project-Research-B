#include "common_functions.h"

#include <stdexcept>

NumType mod(NumType dividend, NumType divisor) {
  if (divisor <= 0)
    throw std::invalid_argument("mod: divisor must be positive\n");
  return dividend >= 0 ? dividend % divisor
                       : (dividend % divisor + divisor) % divisor;
}
