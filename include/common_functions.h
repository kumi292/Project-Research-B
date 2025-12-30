#pragma once

#include <string>

const std::string PORT = "10000";
const std::string CLIENT = "CLIENT";
const std::string SERVER_1 = "SERVER 1";
const std::string SERVER_2 = "SERVER 2";
const std::string QUERY_SELECT = "QUERY SELECT";
const std::string QUERY_INSERT = "QUERY INSERT";
const std::string QUERY_RESULT = "QUERY RESULT";
const std::string SEND_TRIPLE = "SEND TRIPLE";
const std::string EXCHANGE_TRIPLE = "EXCHANGE TRIPLE";
const std::string RETURN_VALUE = "RETURN VALUE";

using NumType = long long;
NumType mod(NumType dividend, NumType divisor);
