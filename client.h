#ifndef CLIENT_H
#define CLIENT_H

#include "constants.h"

int connectToServer(char *svrIP, int port);
int createSocketDesc();
struct sockaddr_in getServerDesc(int port);
bool isConnected(int sockDesc, struct sockaddr_in server);
char *getRequestInput(int port, char *reqType, char *file, int sockDesc);
bool fileExist(char *fileName);

#endif
