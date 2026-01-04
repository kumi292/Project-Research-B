#pragma once

#include "json.hpp"
#include "zmq.hpp"
#include <string>
#include <vector>

const std::string PORT = "10000";
const std::string CLIENT = "CLIENT";
const std::string SERVER_1 = "SERVER 1";
const std::string SERVER_2 = "SERVER 2";
const std::string SERVER_3 = "SERVER 3";
const std::string QUERY_SELECT = "QUERY SELECT";
const std::string QUERY_INSERT = "QUERY INSERT";
const std::string QUERY_RESULT = "QUERY RESULT";
const std::string QUERY_TRUNCATE = "QUERY TRUNCATE";
const std::string EXCHANGE_TRIPLE = "EXCHANGE TRIPLE";
const std::string RETURN_VALUE = "RETURN VALUE";
const std::string SHUT_DOWN = "SHUT DOWN";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[36m";
const std::string NO_COLOR = "\033[m";

const int LATENCY_MILS = 30;

using NumType = long long;
using json = nlohmann::json;

NumType mod(NumType dividend, NumType divisor);
void send_msg(zmq::socket_t &router, std::string destination, std::string body);
void send_to_proxy_hub(zmq::socket_t &sock, std::string content);
json receive_json(zmq::socket_t &sock);
