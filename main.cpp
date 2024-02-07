#include "Server.hpp"

Server server;
void closeServer(int){
  server.closeServer();
}

int main(int argc, char** argv){
  if(argc!=2){
    printf("Usage: %s <port>\n", argv[0]);
    return 1;
  }
  int port;  
  try{
   port=std::stoi(argv[1]);
  } catch (const std::exception& e){
    printf("Invalid port number\n");
    return 1;
  }
  if(port<1025 || port>8000){
    printf("Port number has to be in range <1025,8000>\n");
    return 1;
  }

  signal(SIGPIPE, SIG_IGN);
  signal(SIGINT, closeServer);
  server.setupServer(port); 
  server.run();
}