#include "json.hpp"
#include "zmq.hpp"
#include <fstream>
#include <iostream>
#include <string>

#include "beaver_triple.h"
#include "common_functions.h"

using json = nlohmann::json;

void send_to_proxy_hub(zmq::socket_t &sock, std::string content) {
  zmq::message_t content_msg(content);
  sock.send(content_msg, zmq::send_flags::none);
  std::cout << BLUE << "Sent: \n" << NO_COLOR << content << std::endl;
}

int retrieve_db_size() {
  std::ifstream i_file("db_server_1.json");
  json table_json;
  i_file >> table_json;
  i_file.close();
  return table_json["size"];
}

void send_query_insert(zmq::socket_t &sock) {
  int db_size = retrieve_db_size();
  std::cout << "Enter value to insert (next id: " << db_size << "): ";
  NumType value_to_insert;
  std::cin >> value_to_insert;
  SharesType value_shares = BT::create_shares(value_to_insert);
  json json_to_send_server_1 = {{"from", CLIENT},
                                {"to", SERVER_1},
                                {"type", QUERY_INSERT},
                                {"value", value_shares[0]}};
  json json_to_send_server_2 = {{"from", CLIENT},
                                {"to", SERVER_2},
                                {"type", QUERY_INSERT},
                                {"value", value_shares[1]}};
  send_to_proxy_hub(sock, json_to_send_server_1.dump(2));
  send_to_proxy_hub(sock, json_to_send_server_2.dump(2));
}

void send_query_truncate(zmq::socket_t &sock) {
  json generate_truncate_query = {{"type", QUERY_TRUNCATE}};
  send_to_proxy_hub(sock, generate_truncate_query.dump(2));
}

void send_request_to_generating_triple(zmq::socket_t &sock) {
  json generate_triple_request = {{"type", SEND_TRIPLE}};
  send_to_proxy_hub(sock, generate_triple_request.dump(2));
}

void send_query_select(zmq::socket_t &sock) {}

void send_request_to_shut_down(zmq::socket_t &sock) {
  json generate_shutdown_request = {{"type", SHUT_DOWN}};
  send_to_proxy_hub(sock, generate_shutdown_request.dump(2));
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
    std::cout << "1. INSERT\n2. SELECT\n3. TRUNCATE\n4. SHUT DOWN\n";
    std::cout << "Enter number: ";
    std::cin >> inputted_str;

    if (inputted_str == "1") {
      send_query_insert(sock);

    } else if (inputted_str == "2") {
      send_request_to_generating_triple(sock);
      send_query_select(sock);

    } else if (inputted_str == "3") {
      send_query_truncate(sock);

    } else if (inputted_str == "4") {
      std::cout << "The program will be finished.\n";
      send_request_to_shut_down(sock);
      break;

    } else {
      std::cout << "Sorry, try again.\n";
    }
  }
  return 0;
}
