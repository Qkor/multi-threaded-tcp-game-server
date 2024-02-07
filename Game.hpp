#ifndef GAME
#define GAME
#include <string>

class Game{
public:
  std::string gameController(int clientFd, std::string data);
};

#endif