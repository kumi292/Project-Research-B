#include "json.hpp"
#include "zmq.hpp"
#include <iostream>
#include <string>

#include "common_functions.h"
#include "replicated.h"

int main() {
  zmq::context_t ctx(1);
  zmq::socket_t router(ctx, ZMQ_ROUTER);
  router.bind("tcp://*:" + PORT);
  std::cout << "Proxy hub started." << std::endl;
  std::cout << "Listening on tcp://*:" + PORT + "..." << std::endl;

  while (true) {
    auto received_json = receive_json_in_proxy_hub(router);

    if (received_json["type"] == SHUT_DOWN ||
        received_json["type"] == QUERY_TRUNCATE) {
      send_msg(router, SERVER_1, received_json.dump(2));
      send_msg(router, SERVER_2, received_json.dump(2));
      send_msg(router, SERVER_3, received_json.dump(2));

      if (received_json["type"] == SHUT_DOWN) {
        std::cout << "[LOG] " << " System will shut out." << std::endl;
        break;
      }
    } else {
      std::string destination = received_json["to"];
      send_msg(router, destination, received_json.dump(2));
      std::cout << "----> " << BLUE << "Sent to: " << NO_COLOR << destination
                << std::endl;
    }
  }

  return 0;
}
