#ifndef CLIENT_H
#define CLIENT_H

#include "constants.h"

int connectToServer(char *svrIP, int port);

int createSocketDesc();
struct sockaddr_in getServerDesc(int port);
struct hostent *getHostent(char *ip, struct sockaddr_in server);
bool setServerDesc(int sockDesc, struct sockaddr_in server);
char *getRequestInput(int port, char *reqType, char *file, int sockDesc);

#endif
