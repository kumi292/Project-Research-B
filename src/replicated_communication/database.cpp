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

void exec_insert(zmq::socket_t &sock, json received_json) {
  NumType share = received_json["value"];

  // 他のサーバーに自身のシェアを送信
  json json_to_send_another_server = {{"from", MY_SERVER},
                                      {"to", SERVER_TO_SEND_SHARE},
                                      {"type", SEND_SHARE},
                                      {"value", share}};
  send_to_proxy_hub(sock, json_to_send_another_server.dump(2));
  json json_from_another_server;
  while (true) {
    json_from_another_server = receive_json(sock);
    if (json_from_another_server["type"] == SEND_SHARE &&
        json_from_another_server["from"] == SERVER_TO_RECEIVE_SHARE)
      break;
  }
  NumType share_from_another_server = json_from_another_server["value"];

  std::cout << YELLOW << "INSERT " << NO_COLOR << "value: (" << share << ", "
            << share_from_another_server << ")" << std::endl;
  table_share_1.push_back(share);
  table_share_2.push_back(share_from_another_server);
  save_table();

  json json_to_client = {
      {"from", MY_SERVER}, {"to", CLIENT}, {"type", QUERY_COMPLETED}};
  send_to_proxy_hub(sock, json_to_client.dump(2));
}

NumType exec_multiplication(ShareType share1, ShareType share2) {
  return std::get<0>(share1) * std::get<0>(share2) +
         std::get<0>(share1) * std::get<1>(share2) +
         std::get<1>(share1) * std::get<0>(share2);
}

void exec_select(zmq::socket_t &sock, json received_json) {
  SharesType search_vector_share = received_json["value"].get<SharesType>();
  NumType calculated_result = 0LL;
  int db_size = table_share_1.size();

  // 内積を計算
  for (int id_i = 0; id_i < db_size; id_i++) {
    calculated_result += exec_multiplication(
        {table_share_1[id_i], table_share_2[id_i]}, search_vector_share[id_i]);
    if (id_i % 2 == 0)
      // オーバーフロー防止
      calculated_result = mod(calculated_result, MODULUS);
  }

  // 結果を送信
  json result_value_json = {{"from", MY_SERVER},
                            {"to", CLIENT},
                            {"type", QUERY_COMPLETED},
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
    SERVER_TO_SEND_SHARE = SERVER_2;
    SERVER_TO_RECEIVE_SHARE = SERVER_3;
    DB_FILE = DB_FILE_1;
  } else if (strcmp(argv[1], "2") == 0) {
    MY_SERVER = SERVER_2;
    SERVER_TO_SEND_SHARE = SERVER_3;
    SERVER_TO_RECEIVE_SHARE = SERVER_1;
    DB_FILE = DB_FILE_2;
  } else {
    MY_SERVER = SERVER_3;
    SERVER_TO_SEND_SHARE = SERVER_1;
    SERVER_TO_RECEIVE_SHARE = SERVER_2;
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
    auto received_json = receive_json(sock);

    if (received_json["type"] == QUERY_INSERT) {
      exec_insert(sock, received_json);

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
