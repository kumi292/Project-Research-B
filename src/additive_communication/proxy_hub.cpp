#include "json.hpp"
#include "zmq.hpp"
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>

#include "beaver_triple.h"
#include "common_functions.h"

using json = nlohmann::json;

std::pair<json, json> generate_beaver_triple() {
  std::random_device seed_gen;
  std::mt19937 engine(seed_gen());
  std::uniform_int_distribution<NumType> dist(0, TRIPLE_MAX);
  json triple_1, triple_2;

  NumType triple_a = dist(engine);
  NumType triple_b = dist(engine);
  SharesType triple_a_shares = BT::create_shares(triple_a);
  SharesType triple_b_shares = BT::create_shares(triple_b);
  SharesType triple_c_shares = BT::create_shares(triple_a * triple_b);

  triple_1["triple_a_share"] = (triple_a_shares[0]);
  triple_2["triple_a_share"] = (triple_a_shares[1]);
  triple_1["triple_b_share"] = (triple_b_shares[0]);
  triple_2["triple_b_share"] = (triple_b_shares[1]);
  triple_1["triple_c_share"] = (triple_c_shares[0]);
  triple_2["triple_c_share"] = (triple_c_shares[1]);
  return {triple_1, triple_2};
}

void send_msg(zmq::socket_t &router, std::string destination,
              std::string body) {
  zmq::message_t dest_msg(destination);
  zmq::message_t body_msg(body);

  // 通信が発生するたびに遅延を入れる
  std::this_thread::sleep_for(std::chrono::milliseconds(LATENCY_MILS));
  router.send(dest_msg, zmq::send_flags::sndmore);
  router.send(body_msg, zmq::send_flags::none);
}

int main() {
  zmq::context_t ctx(1);
  zmq::socket_t router(ctx, ZMQ_ROUTER);
  router.bind("tcp://*:" + PORT);
  std::cout << "Proxy hub started." << std::endl;
  std::cout << "Listening on tcp://*:" + PORT + "..." << std::endl;

  while (true) {
    zmq::message_t sender_msg;
    zmq::message_t content_msg;
    auto ret = router.recv(sender_msg, zmq::recv_flags::none);
    ret = router.recv(content_msg, zmq::recv_flags::none);
    if (!ret) {
      std::cout << "[LOG] " << RED << "ERROR, Can't Receive Message Correctly."
                << NO_COLOR << std::endl;
      continue;
    }

    std::string sender = sender_msg.to_string();
    std::string content = content_msg.to_string();
    std::cout << "[LOG] " << GREEN << "From: " << NO_COLOR << sender << NO_COLOR
              << std::endl;
    std::cout << "[LOG] " << GREEN << "Received: \n"
              << NO_COLOR << content << std::endl;

    auto received_json = json::parse(content);

    if (received_json["type"] == SEND_TRIPLE) {
      auto [triple_1, triple_2] = generate_beaver_triple();
      triple_1.push_back({"type", SEND_TRIPLE});
      triple_2.push_back({"type", SEND_TRIPLE});
      send_msg(router, SERVER_1, triple_1.dump(2));
      std::cout << "[LOG] " << GREEN << "Beaver Triple is sent to DB server 1."
                << NO_COLOR << std::endl;
      send_msg(router, SERVER_2, triple_2.dump(2));
      std::cout << "[LOG] " << GREEN << "Beaver Triple is sent to DB server 2."
                << NO_COLOR << std::endl;

    } else if (received_json["type"] == SHUT_DOWN ||
               received_json["type"] == QUERY_TRUNCATE) {
      send_msg(router, SERVER_1, received_json.dump(2));
      send_msg(router, SERVER_2, received_json.dump(2));

      if (received_json["type"] == SHUT_DOWN) {
        std::cout << "[LOG] " << " System will shut out." << std::endl;
        break;
      }

    } else {
      std::string destination = received_json["to"];
      send_msg(router, destination, content);
      std::cout << "----> " << BLUE << "Sent to: " << NO_COLOR << destination
                << std::endl;
    }
  }

  return 0;
}
