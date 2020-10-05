#include "client.h"

int main(int argc, char *argv[])
{
    int reqPort;
    char *svrIP;
    int socketDesc;
    char reqType[COMMAND_SIZE];
    char fileName[FILE_SIZE];
    char rcvBuf[BUFFER_SIZE];

    // get IP and port number for arguement
    switch (argc)
    {
    case 3:
        svrIP = argv[1];
        reqPort = atoi(argv[2]); // User specified port
        break;
    default:
        fprintf(stderr, "Please enter the ip and port \n");
        exit(1);
    }

    fprintf(stdout, "IP: %s\n", svrIP);
    fprintf(stdout, "Server Port: %d\n", reqPort);

    // Connect to Server Connection Port -----------------------------------
    // socketDesc = connectToServer(svrIP, SERVER_LISTEN_PORT);
    socketDesc = connectToServer(svrIP, 7000);

    // Send Request to Server ----------------------------------------------
    while (true)
    {
        send(socketDesc, getRequestInput(reqPort, reqType, fileName, socketDesc), BUFFER_SIZE, 0);

        // Wait for OKY
        recv(socketDesc, rcvBuf, BUFFER_SIZE, 0);
        // fprintf(stdout, "Received from Server: %s\n", rcvBuf);

        if (strcmp(rcvBuf, COMMAND_OKAY) == 0)
        {
            close(socketDesc);
            break;
        }
        else if (strcmp(rcvBuf, COMMAND_FNF))
        {
            fprintf(stdout, "\nFile you entered is not found (%s)\n", fileName);
            send(socketDesc, getRequestInput(reqPort, reqType, fileName, socketDesc), BUFFER_SIZE, 0);
        }
        else
        {
            fprintf(stdout, "\nError, Closing Client.\n");
            close(socketDesc);
            exit(1);
        }
    }

    // Connect to Server for File Transfer
    socketDesc = connectToServer(svrIP, reqPort);

    if (strcmp(rcvBuf, COMMAND_GET))
    {
    }
    else if (strcmp(rcvBuf, COMMAND_SEND))
    {
        // Open file
    }

    return 0;
}

int connectToServer(char *svrIP, int port)
{
    int socketDesc;
    struct sockaddr_in svrDesc;
    struct hostent *hp;

    // Socket ---------------------------------------------------------------
    socketDesc = createSocketDesc();

    // Set server description -----------------------------------------------
    svrDesc = getServerDesc(port); // for testing

    // HP ------------------------------------------------------------------
    // hp = getHostent(svrIP, svrDesc);
    if ((hp = gethostbyname(svrIP)) == NULL)
    {
        fprintf(stderr, "Unknown server address\n");
        exit(1);
    }

    bcopy(hp->h_addr, (char *)&svrDesc.sin_addr, hp->h_length);

    // Connect -------------------------------------------------------------
    // connectToServer(socketDesc, svrDesc);
    if (!setServerDesc(socketDesc, svrDesc))
        exit(1);

    fprintf(stdout, "Connected \n");
    return socketDesc;
}
// Creates a scoket descriptor
int createSocketDesc()
{
    int sd;
    // Create the socket
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Cannot create socket");
        exit(1);
    }
    return sd;
}

// get Server Description
struct sockaddr_in getServerDesc(int port)
{
    struct sockaddr_in server;
    // Set server description
    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    return server;
}

//Get Hostent Structure
struct hostent *getHostent(char *ip, struct sockaddr_in server)
{
    struct hostent *hp;

    if ((hp = gethostbyname(ip)) == NULL)
    {
        fprintf(stderr, "Unknown server address\n");
        exit(1);
    }
    bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);
    return hp;
}

//Connect to Server
bool setServerDesc(int sockDesc, struct sockaddr_in server)
{
    if (connect(sockDesc, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        fprintf(stderr, "Can't connect to server\n");
        return false;
    }
    fprintf(stdout, "Connected\n");
    return true;
}

char *getRequestInput(int port, char *reqType, char *file, int sockDesc)
{
    char *type = malloc(COMMAND_SIZE);
    char *fileName = malloc(BUFFER_SIZE - COMMAND_SIZE);
    char *buffer = malloc(BUFFER_SIZE);
    bool validType = false;

    while (!validType)
    {
        fprintf(stdout, "\nEnter the Request Type (SND, GET, or EXT): ");
        if (fscanf(stdin, "%s", type))
        {
            if ((strcmp(type, COMMAND_SEND) == 0) || (strcmp(type, COMMAND_GET) == 0))
                validType = 1;

            else if (strcmp(type, COMMAND_EXIT) == 0)
            {
                fprintf(stdout, "Closing Client\n");
                close(sockDesc);
                exit(1);
            }
        }
    }

    // filename
    fprintf(stdout, "Enter the File Name with type (text.txt ): ");
    fflush(stdout);

    if (fscanf(stdin, "%s", fileName))
    {
        fprintf(stdout, "Types: %s \n", type);
        fprintf(stdout, "File: %s \n", fileName);
    }

    sprintf(buffer, "%s:%d:%s", type, port, fileName);

    strcpy(reqType, type);
    strcpy(file, fileName);

    return buffer;
}