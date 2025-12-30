#include "json.hpp"
#include "zmq.hpp"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "beaver_triple.h"
#include "common_functions.h"

using json = nlohmann::json;
std::string MY_SERVER;
std::string OTHER_SERVER;
std::string DB_FILE;
std::vector<NumType> table;

void send_to_proxy_hub(zmq::socket_t &sock, std::string content) {
  zmq::message_t content_msg(content);
  sock.send(content_msg, zmq::send_flags::none);
  std::cout << "Sent: \n" << content_msg << std::endl;
}

void init_table() {
  json init_table_json = {{"size", 0}, {"records", {}}};
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
    table_json["records"].get_to(table);
    print_table(table, 10);
  } else {
    std::cout << "No records in the table." << std::endl;
  }
  i_file.close();
}

void save_table() {
  json table_json_to_save;
  table_json_to_save["size"] = table.size();
  table_json_to_save["records"] = table;

  std::ofstream o_file(DB_FILE);
  o_file << table_json_to_save.dump(2);
  o_file.close();
  print_table(table, 10, true);
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

void exec_select(json received_json) {
  std::vector<NumType> search_vector_share = received_json["value"];
  std::cout << YELLOW << "SELECT" << NO_COLOR << std::endl;
}

int main(int argc, char *argv[]) {
  // 引数1 → SERVER 1として起動
  // 引数2 → SERVER 2として起動
  if (argc != 2 || (strcmp(argv[1], "1") * strcmp(argv[1], "2")) != 0) {
    std::cout << RED << "Unvalid Booting Option" << std::endl;
    return 1;
  }
  if (strcmp(argv[1], "1") == 0) {
    MY_SERVER = SERVER_1;
    OTHER_SERVER = SERVER_2;
    DB_FILE = "db_server_1.json";
  } else {
    MY_SERVER = SERVER_2;
    OTHER_SERVER = SERVER_1;
    DB_FILE = "db_server_2.json";
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
      std::cout << "[LOG] " << RED << "ERROR, Can't Receive Massage Correctly."
                << NO_COLOR << std::endl;
      continue;
    }

    std::string content = content_msg.to_string();
    std::cout << "[LOG] " << GREEN << "Received: \n"
              << NO_COLOR << content << std::endl;

    auto received_json = json::parse(content);

    if (received_json["type"] == QUERY_INSERT) {
      exec_insert(received_json);
    } else if (received_json["type"] == SEND_TRIPLE) {

    } else if (received_json["type"] == QUERY_TRUNCATE) {
      truncate_table();

    } else if (received_json["type"] == SHUT_DOWN) {
      break;
    }
    std::cout << "-------------------------\n";
  }
  return 0;
}
