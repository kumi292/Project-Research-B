#include "json.hpp"
#include "zmq.hpp"
#include <iostream>
#include <string>

#include "beaver_triple.h"
#include "common_functions.h"

using json = nlohmann::json;

void send_to_proxy_hub(zmq::socket_t &sock, std::string body) {
  zmq::message_t body_msg(body);
  sock.send(body_msg, zmq::send_flags::none);
  std::cout << "Sent: \n" << body << std::endl;
}

int main() {
  zmq::context_t ctx(1);
  zmq::socket_t sock(ctx, ZMQ_DEALER);
  sock.set(zmq::sockopt::routing_id, CLIENT);
  sock.connect("tcp://localhost:" + PORT);
  std::cout << "Started connecting to tcp:://localhost:" << PORT << std::endl;
  std::string inputted_str;

  while (true) {
    std::cout << "-------------------------\n";
    std::cout << "Which query to execute\n";
    std::cout << "1. INSERT\n2. SELECT\n";
    std::cout << "Enter number: ";
    std::cin >> inputted_str;

    if (inputted_str == "1") {
      // INSERT文
      std::cout << "Enter value to insert: ";
      NumType value_to_insert;
      std::cin >> value_to_insert;
      SharesType value_shares = BT::create_shares(value_to_insert);
      json json_to_send = {{{"to", SERVER_1},
                            {"type", QUERY_INSERT},
                            {"value", std::to_string(value_shares[0])}},
                           {{"to", SERVER_2},
                            {"type", QUERY_INSERT},
                            {"value", std::to_string(value_shares[1])}}};
      send_to_proxy_hub(sock, json_to_send.dump(2));

    } else if (inputted_str == "2") {
      // SELECT文
    } else {
      std::cout << "Sorry, try again.\n";
    }
  }
  return 0;
}
