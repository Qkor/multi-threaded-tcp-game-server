#include "Server.hpp"

void Server::handleError(int error){
  switch(error){
    case 0: printf("could not create socket\n");break;
    case 1: printf("fcntl failed\n");break;
    case 2: printf("setsockopt failed\n");break;
    case 3: printf("bind failed\n");break;
    case 4: printf("listen failed\n");break;
    case 5: printf("accept failed\n");break;
    default:printf("something went wrong\n");break;
  }
  closeServer();
}

void Server::setupServer(int port){
  // setting up socket
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sfd<0) handleError(0);
  // if(fcntl(sfd, F_SETFL, O_NONBLOCK)) handleError(1);
  // reusing address
  const int one = 1;
	if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0) handleError(2);
  // bind & listen
  sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
  if(bind(sfd, (sockaddr*) &serverAddr, sizeof(serverAddr))) handleError(3);
  if(listen(sfd, 7)) handleError(4);
}

void Server::closeServer(){
  running = false;
  printf("Closing server\n");
  close(sfd);
  for(int clientFd : clientFds)
		close(clientFd);
	exit(0);
}

void Server::run(){
  running = true;
  // accept connections
  sockaddr_in clientAddr{0};
  socklen_t clientAddrSize = sizeof(clientAddr);
  while(1){
    int clientFd = accept(sfd, (sockaddr*)&clientAddr, &clientAddrSize);
    if(clientFd<0){
      handleError(5);
    }
    printf("new client: %d\n", clientFd);
    // add client descriptor to set and run thread to handle connection
    clientMutex.lock();
    clientFds.insert(clientFd);
    clientMutex.unlock();
    std::thread(handleClient, this, clientFd).detach();
  }
}

void Server::handleClient(Server* server, int fd){
  // communication with client
  while(server->running){
    //reading message size
    uint16_t msgSize;
    if(recv(fd, &msgSize, sizeof(uint16_t), MSG_WAITALL) != sizeof(uint16_t)) 
      break;
    msgSize = ntohs(msgSize);
    // reading message
    char data[msgSize+1]{};
    if(recv(fd, data, msgSize, MSG_WAITALL) != msgSize) 
      break;
    std::string dataString = data;
    // passing message to the game
    dataString = server->game.gameController(fd, dataString);
    // if game returns 'exit' then disconect the client
    if(dataString == "exit")
      break;
    // else send respone to the cient
    msgSize = htons(dataString.length());
    write(fd, &msgSize, sizeof(uint16_t));
    write(fd, dataString.c_str(), dataString.length());
  }
  //disconnect the client
  std::string dataString = "disconnecting\n";
  uint16_t msgSize = htons(dataString.length());
  write(fd, &msgSize, sizeof(uint16_t));
  write(fd, dataString.c_str(), dataString.length());
  close(fd);
  printf("client %d left\n",fd);
  server->clientMutex.lock();
  server->clientFds.erase(fd);
  server->clientMutex.unlock();
}