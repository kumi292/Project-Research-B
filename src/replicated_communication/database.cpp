#include "json.hpp"
#include "zmq.hpp"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "common_functions.h"
#include "replicated.h"

std::string MY_SERVER;
std::string SERVER_TO_SEND_SHARE;
std::string SERVER_TO_RECEIVE_SHARE;
std::string DB_FILE;
std::vector<NumType> table_share_1;
std::vector<NumType> table_share_2;

void init_table() {
  table_share_1 = {};
  table_share_2 = {};
  json init_table_json = {{"size", 0},
                          {"records", {{"share1", {}}, {"share2", {}}}}};
  std::ofstream o_file(DB_FILE);
  o_file << init_table_json.dump(2);
  o_file.close();
}

void load_table() {
  std::ifstream i_file(DB_FILE);
  if (!i_file.is_open()) {
    init_table();
    return;
  }
  json table_json;
  i_file >> table_json;
  if (table_json["size"] != 0) {
    table_json["records"]["share1"].get_to(table_share_1);
    table_json["records"]["share2"].get_to(table_share_2);
    Replicated::print_all_table(table_share_1, table_share_2, 10);
  } else {
    std::cout << "No records in the table." << std::endl;
  }
  i_file.close();
}

void save_table() {
  json table_json_to_save;
  table_json_to_save["size"] = table_share_1.size();
  table_json_to_save["records"]["share1"] = table_share_1;
  table_json_to_save["records"]["share2"] = table_share_2;

  std::ofstream o_file(DB_FILE);
  o_file << table_json_to_save.dump(2);
  o_file.close();
  Replicated::print_all_table(table_share_1, table_share_2, 10, true);
}

void truncate_table() {
  std::cout << YELLOW << "TRUNCATE" << NO_COLOR << std::endl;
  init_table();
}

void exec_insert(json received_json) {
  NumType value = received_json["value"];
  std::cout << YELLOW << "INSERT " << NO_COLOR << "value: " << value
            << std::endl;
  table.push_back(value);
  save_table();
}

NumType exec_multiplication(zmq::socket_t &sock, NumType share_1,
                            NumType share_2) {
  // Beaver Tripleの生成を要求(server_1のみ)し、受信
  if (MY_SERVER == SERVER_1) {
    json request_for_beaver_triple = {{"type", SEND_TRIPLE}};
    send_to_proxy_hub(sock, request_for_beaver_triple.dump(2));
  }
  zmq::message_t content_msg;
  auto ret = sock.recv(content_msg, zmq::recv_flags::none);
  if (!ret) {
    std::cout << RED << "ERROR, Can't Receive Message Correctly." << NO_COLOR
              << std::endl;
    exit(1);
  }
  std::string content = content_msg.to_string();
  std::cout << GREEN << "Received: \n" << NO_COLOR << content << std::endl;
  auto received_json = json::parse(content);
  if (received_json["type"] != SEND_TRIPLE)
    exit(1);
  NumType triple_a_share = received_json["triple_a_share"];
  NumType triple_b_share = received_json["triple_b_share"];
  NumType triple_c_share = received_json["triple_c_share"];

  // sigmaとrhoのシェアを計算しもう片方のサーバーへ送信
  NumType sigma_share = share_1 - triple_a_share;
  NumType rho_share = share_2 - triple_b_share;
  json exchange_triple_share = {{"from", MY_SERVER},
                                {"to", OTHER_SERVER},
                                {"type", EXCHANGE_TRIPLE},
                                {"sigma_share", sigma_share},
                                {"rho_share", rho_share}};
  send_to_proxy_hub(sock, exchange_triple_share.dump(2));

  // 片方のサーバーからsigmaとrhoのシェアを受信し復元
  ret = sock.recv(content_msg, zmq::recv_flags::none);
  if (!ret) {
    std::cout << RED << "ERROR, Can't Receive Message Correctly." << NO_COLOR
              << std::endl;
    exit(1);
  }
  content = content_msg.to_string();
  std::cout << GREEN << "Received: \n" << NO_COLOR << content << std::endl;
  received_json = json::parse(content);
  if (received_json["type"] != EXCHANGE_TRIPLE)
    exit(1);
  NumType received_sigma_share = received_json["sigma_share"];
  NumType received_rho_share = received_json["rho_share"];
  NumType sigma =
      BT::reconstruct_from_shares({sigma_share, received_sigma_share});
  NumType rho = BT::reconstruct_from_shares({rho_share, received_rho_share});

  // 掛け算を実行
  NumType product_share =
      rho * triple_a_share + sigma * triple_b_share + triple_c_share;
  if (MY_SERVER == SERVER_1)
    product_share += sigma * rho;

  return product_share;
}

void exec_select(zmq::socket_t &sock, json received_json) {
  std::vector<NumType> search_vector_share = received_json["value"];
  NumType calculated_result = 0LL;
  int db_size = table.size();

  // 内積を計算
  for (int id_i = 0; id_i < db_size; id_i++) {
    calculated_result +=
        exec_multiplication(sock, table[id_i], search_vector_share[id_i]);
    if (id_i % 2 == 0)
      // オーバーフロー防止
      calculated_result = mod(calculated_result, MODULUS);
  }

  // 結果を送信
  json result_value_json = {{"from", MY_SERVER},
                            {"to", CLIENT},
                            {"type", RETURN_VALUE},
                            {"value", calculated_result}};
  send_to_proxy_hub(sock, result_value_json.dump(2));

  std::cout << YELLOW << "SELECT " << NO_COLOR << "value: " << calculated_result
            << std::endl;
}

int main(int argc, char *argv[]) {
  // 引数1 → SERVER 1として起動
  // 引数2 → SERVER 2として起動
  // 引数3 → SERVER 3として起動
  if (argc != 2 || (strcmp(argv[1], "1") * strcmp(argv[1], "2") *
                    strcmp(argv[1], "3")) != 0) {
    std::cout << RED << "Invalid Booting Option" << std::endl;
    return 1;
  }
  if (strcmp(argv[1], "1") == 0) {
    MY_SERVER = SERVER_1;
    SERVER_TO_SEND_SHARE = SERVER_3;
    SERVER_TO_RECEIVE_SHARE = SERVER_2;
    DB_FILE = DB_FILE_1;
  } else if (strcmp(argv[1], "2") == 0) {
    MY_SERVER = SERVER_2;
    SERVER_TO_SEND_SHARE = SERVER_1;
    SERVER_TO_RECEIVE_SHARE = SERVER_3;
    DB_FILE = DB_FILE_2;
  } else {
    MY_SERVER = SERVER_3;
    SERVER_TO_SEND_SHARE = SERVER_2;
    SERVER_TO_RECEIVE_SHARE = SERVER_1;
    DB_FILE = DB_FILE_3;
  }

  zmq::context_t ctx(1);
  zmq::socket_t sock(ctx, ZMQ_DEALER);
  sock.set(zmq::sockopt::routing_id, MY_SERVER);
  sock.connect("tcp://localhost:" + PORT);
  std::cout << "Started connecting to tcp:://localhost:" << PORT << std::endl;
  std::cout << "Booting " << MY_SERVER << std::endl;
  load_table();

  while (true) {
    std::cout << "Waiting for a query...\n";
    zmq::message_t content_msg;
    auto ret = sock.recv(content_msg, zmq::recv_flags::none);
    if (!ret) {
      std::cout << RED << "ERROR, Can't Receive Message Correctly." << NO_COLOR
                << std::endl;
      continue;
    }

    std::string content = content_msg.to_string();
    std::cout << GREEN << "Received: \n" << NO_COLOR << content << std::endl;

    auto received_json = json::parse(content);

    if (received_json["type"] == QUERY_INSERT) {
      exec_insert(received_json);

    } else if (received_json["type"] == QUERY_SELECT) {
      exec_select(sock, received_json);

    } else if (received_json["type"] == QUERY_TRUNCATE) {
      truncate_table();

    } else if (received_json["type"] == SHUT_DOWN) {
      break;
    }
    std::cout << std::endl;
    std::cout << "-------------------------\n";
  }
  return 0;
}
