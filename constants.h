#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVER_LISTEN_PORT 7010
#define BUFFER_SIZE 255
#define FILE_SIZE 251
#define COMMAND_SIZE 4

#define COMMAND_OKAY "OKY"
#define COMMAND_FNF "FNF"
#define COMMAND_EROR "ERR"
#define COMMAND_FINISH "FIN"

#define COMMAND_GET "GET"
#define COMMAND_SEND "SND"
#define COMMAND_EXIT "EXT"

#endif
