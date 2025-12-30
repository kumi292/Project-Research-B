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
  json json_to_send_server_1 = {
      {"to", SERVER_1}, {"type", QUERY_INSERT}, {"value", value_shares[0]}};
  json json_to_send_server_2 = {
      {"to", SERVER_2}, {"type", QUERY_INSERT}, {"value", value_shares[1]}};
  send_to_proxy_hub(sock, json_to_send_server_1.dump(2));
  send_to_proxy_hub(sock, json_to_send_server_2.dump(2));
}

void send_query_select(zmq::socket_t &sock) {
  int db_size = retrieve_db_size();
  if (db_size == 0) {
    std::cout << RED << "Cannot execute SELECT query in an empty table."
              << std::endl;
    return;
  }

  // 検索ベクトル作成
  std::cout << "Enter id to select: ";
  int id_to_select;
  std::cin >> id_to_select;
  if (id_to_select > db_size - 1) {
    std::cout << RED << "ERROR, inputted number is too big for the table."
              << NO_COLOR << std::endl;
    return;
  }
  std::vector<int> search_vector(db_size);
  search_vector[id_to_select] = 1;

  // 検索ベクトルを分散
  std::vector<NumType> search_vector_share_1(db_size),
      search_vector_share_2(db_size);
  for (int id_i = 0; id_i < db_size; id_i++) {
    SharesType id_i_shares = BT::create_shares(search_vector[id_i]);
    search_vector_share_1[id_i] = id_i_shares[0];
    search_vector_share_2[id_i] = id_i_shares[1];
  }

  // 検索ベクトルのシェアを送信
  json json_to_send_server_1 = {{"to", SERVER_1},
                                {"type", QUERY_SELECT},
                                {"value", search_vector_share_1}};
  json json_to_send_server_2 = {{"to", SERVER_2},
                                {"type", QUERY_SELECT},
                                {"value", search_vector_share_2}};

  // 両サーバーから応答が返ってくるまで待機
  std::cout << "Waiting for response..." << std::endl;
  zmq::message_t content_msg;
  bool is_received_from_server_1 = false;
  bool is_received_from_server_2 = false;
  json json_from_server_1, json_from_server_2;
  while (!is_received_from_server_1 || !is_received_from_server_2) {
    auto ret = sock.recv(content_msg, zmq::recv_flags::none);
    if (!ret) {
      std::cout << "[LOG] " << RED << "ERROR, Can't Receive Massage Correctly."
                << NO_COLOR << std::endl;
      return;
    }

    std::string content = content_msg.to_string();
    std::cout << "[LOG] " << GREEN << "Received: \n"
              << NO_COLOR << content << std::endl;
    auto received_json = json::parse(content);
    if (received_json["from"] == SERVER_1) {
      is_received_from_server_1 = true;
      json_from_server_1 = received_json;
    } else if (received_json["from"] == SERVER_2) {
      is_received_from_server_2 = true;
      json_from_server_2 = received_json;
    }
  }

  // 復元して値を取得
  NumType value_from_server_1 = json_from_server_1["value"];
  NumType value_from_server_2 = json_from_server_2["value"];
  std::cout << "Reconstruct from " << value_from_server_1 << " and "
            << value_from_server_2 << "." << std::endl;
  std::vector<NumType> result_table = {
      BT::reconstruct_from_shares({value_from_server_1, value_from_server_2})};
  print_table(result_table, 1, false, id_to_select);
}

void send_query_truncate(zmq::socket_t &sock) {
  json generate_truncate_query = {{"type", QUERY_TRUNCATE}};
  send_to_proxy_hub(sock, generate_truncate_query.dump(2));
}

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
