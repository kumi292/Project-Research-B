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
  zmq::context_t ctx(1);
  zmq::socket_t router(ctx, ZMQ_ROUTER);
  router.bind("tcp://*:" + PORT);
  std::cout << "Proxy hub started." << std::endl;
  std::cout << "Listening on tcp://*:" + PORT + "..." << std::endl;

  while (true) {
    zmq::message_t identity;
    zmq::message_t payload;

    auto ret = router.recv(identity, zmq::recv_flags::none);
    ret = router.recv(payload, zmq::recv_flags::none);

    if (!ret) {
      std::cout << "[LOG] ERROR, Can't Receive Massage Correctly." << std::endl;
      continue;
    }

    std::string sender_id = identity.to_string();
    std::string body_str = payload.to_string();
    std::cout << "[LOG] From: " << sender_id << std::endl;
    std::cout << "[LOG] Body: \n" << body_str << std::endl;

    try {
      auto received_json = json::parse(body_str);
      std::string destination = received_json["to"];

      if (received_json["type"] == SEND_TRIPLE) {
        int db_size = 10;
        auto [triple_1, triple_2] = generate_beaver_triple(db_size);
        triple_1.push_back({"type", SEND_TRIPLE});
        triple_2.push_back({"type", SEND_TRIPLE});
        send_msg(router, SERVER_1, triple_1.dump());
        std::cout << "[LOG] Beaver Triple is sent to DB server 1.";
        send_msg(router, SERVER_2, triple_2.dump());
        std::cout << "[LOG] Beaver Triple is sent to DB server 2.";

      } else if (received_json["type"] == QUERY_SELECT) {
        send_msg(router, destination, body_str);

      } else {
        send_msg(router, destination, body_str);
        std::cout << "[LOG] Forwarded to: " << sender_id << std::endl;
      }
    } catch (std::exception &e) {
      std::cout << "[!] JSON Parse Error: " << e.what() << std::endl;
    }
  }

  return 0;
}
