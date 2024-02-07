#ifndef SERVER
#define SERVER
#include <thread>
#include <csignal>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unordered_set>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <string>
#include "Game.hpp"

class Server{
  bool running = false;
  int sfd;
  std::mutex clientMutex;
  std::unordered_set<int> clientFds;
  Game game;

  void handleError(int);
  static void handleClient(Server*, int);

public:
  void setupServer(int port);
  void closeServer();
  void run();
};

#endif