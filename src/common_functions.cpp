#include "common_functions.h"

#include "zmq.hpp"
#include <chrono>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <thread>

NumType mod(NumType dividend, NumType divisor) {
  if (divisor <= 0)
    throw std::invalid_argument("mod: divisor must be positive\n");
  return dividend >= 0 ? dividend % divisor
                       : (dividend % divisor + divisor) % divisor;
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

json receive_json_in_proxy_hub(zmq::socket_t &router) {
  zmq::message_t sender_msg;
  zmq::message_t content_msg;
  auto ret = router.recv(sender_msg, zmq::recv_flags::none);
  ret = router.recv(content_msg, zmq::recv_flags::none);
  if (!ret) {
    std::cout << "[LOG] " << RED << "ERROR, Can't Receive Message Correctly."
              << NO_COLOR << std::endl;
    exit(1);
  }

  std::string sender = sender_msg.to_string();
  std::string content = content_msg.to_string();
  std::cout << "[LOG] " << GREEN << "From: " << NO_COLOR << sender << NO_COLOR
            << std::endl;
  std::cout << "[LOG] " << GREEN << "Received: \n"
            << NO_COLOR << content << std::endl;

  return json::parse(content);
}

void send_to_proxy_hub(zmq::socket_t &sock, std::string content) {
  zmq::message_t content_msg(content);
  sock.send(content_msg, zmq::send_flags::none);
  std::cout << BLUE << "Sent: \n" << NO_COLOR << content << std::endl;
}

json receive_json(zmq::socket_t &sock) {
  zmq::message_t content_msg;
  auto ret = sock.recv(content_msg, zmq::recv_flags::none);
  if (!ret) {
    std::cout << RED << "ERROR, Can't Receive Message Correctly." << NO_COLOR
              << std::endl;
    exit(1);
  }
  std::string content = content_msg.to_string();
  std::cout << GREEN << "Received: \n" << NO_COLOR << content << std::endl;
  return json::parse(content);
}
