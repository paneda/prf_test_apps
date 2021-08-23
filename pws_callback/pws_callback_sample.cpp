#include <iostream>
#include <csignal>
#include "pws_api.h"

bool left, right, turn = false;

static void sigHandler(int signum) {
  std::cout << "Caught ctrl-c\n";
  stop();
  exit(0);
}

void toggleRight() {
  right = !right;
  std::cout << "Right: " << (right ? "ON" : "OFF") << std::endl;
}

void toggleLeft() {
  left = !left;
  std::cout << "Left: " << (left ? "ON" : "OFF") << std::endl;
}

void toggleTurn() {
  turn = !turn;
  std::cout << "Turn: " << (turn ? "ON" : "OFF") << std::endl;
}

int main() {
  //Create a way to end the program gracefully catch ctrl-c
  struct sigaction sigIntHandler{};
  sigIntHandler.sa_handler = sigHandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, nullptr);

  //create events that trigger callback functions
  PwsEvent eventLeft{0, 1626704570, false, toggleLeft};
  PwsEvent eventRight{1, 1626704570, false, toggleRight};
  PwsEvent eventTurn{2, 1626704570, false, toggleTurn};

  //register events
  registerPwsEvent(eventLeft);
  registerPwsEvent(eventRight);
  registerPwsEvent(eventTurn);

  //start listening for events (this is single threaded)
  start();
  return 0;
}