#include "json.hpp"
#include "zmq.hpp"
#include <iostream>
#include <random>
#include <string>

#include "beaver_triple.h"
#include "common_functions.h"

using json = nlohmann::json;
int communication_count = 0;

std::pair<json, json> generate_beaver_triple(int db_size) {
  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());
  std::uniform_int_distribution<NumType> dist(0, TRIPLE_MAX);
  json triple_1, triple_2;

  // db_size分beaver tripleを生成
  for (int i = 0; i < db_size; i++) {
    NumType triple_a = dist(engine);
    NumType triple_b = dist(engine);
    SharesType triple_a_shares = BT::create_shares(triple_a);
    SharesType triple_b_shares = BT::create_shares(triple_b);
    SharesType triple_c_shares = BT::create_shares(triple_a * triple_b);

    triple_1["triple_a"].push_back(triple_a_shares[0]);
    triple_2["triple_a"].push_back(triple_a_shares[1]);
    triple_1["triple_b"].push_back(triple_b_shares[0]);
    triple_2["triple_b"].push_back(triple_b_shares[1]);
    triple_1["triple_c"].push_back(triple_c_shares[0]);
    triple_2["triple_c"].push_back(triple_c_shares[1]);
  }
  return {triple_1, triple_2};
}

void send_msg(zmq::socket_t &router, std::string destination,
              std::string body) {
  zmq::message_t dest_msg(destination);
  zmq::message_t body_msg(body);
  router.send(dest_msg, zmq::send_flags::sndmore);
  router.send(body_msg, zmq::send_flags::none);
  communication_count++;
}

int main() {
  return 0;
}
