#include "json.hpp"
#include "zmq.hpp"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "common_functions.h"

using json = nlohmann::json;

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

    if (received_json["type"] == SHUT_DOWN ||
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
