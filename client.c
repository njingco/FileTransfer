/*---------------------------------------------------------------------------------------
 * SOURCE FILE:	    client.c
 * 
 * PROGRAM:		    client
 * 
 * FUNCTIONS:		int connectToServer(char *svrIP, int port);
 *                  int createSocketDesc();
 *                  struct sockaddr_in getServerDesc(int port);
 *                  bool isConnected(int sockDesc, struct sockaddr_in server);
 *                  char *getRequestInput(int port, char *reqType, char *file, int sockDesc);
 *                  bool fileExist(char *fileName);
 * 
 * DATE:			October 4, 2020
 * 
 * REVISIONS:		Octoner 5, 2020 - Bug Fixing
 *                  October 6, 2020 - More Comments
 * 
 * DESIGNERS:       Nicole Jingco, Tomas Quat
 * 
 * PROGRAMMERS:		Nicole Jingco
 * 
 * Notes:
 * This file does all the client functionality
 * ---------------------------------------------------------------------------------------*/
#include "client.h"
#include "utilities.h"

/*--------------------------------------------------------------------------
 * FUNCTION:       main
 *
 * DATE:           October 4, 2020 
 *
 * REVISIONS:      October 5, Fixed Some bugs with the Request type comparison
 *                 October 6, 2020 - More Comments
 * 
 * DESIGNER:       Nicole Jingco, Tomas Quat
 *
 * PROGRAMMER:     Nicole Jingco
 *
 * INTERFACE:      int argc, char *argv[] - arguements user add in before
 *                                          executing the file. For this 
 *                                          application, it will be the 
 *                                          ip and data port number
 *
 * RETURNS:        
 *
 * NOTES:
 * This function manages the client process of connecting server, managing 
 * what the server sends back, and what the client will send to the server
 * -----------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int dataPort;
    char *svrIP;
    int socketDesc;
    char reqType[COMMAND_SIZE];
    char fileName[FILE_SIZE];
    char rcvBuf[BUFFER_SIZE];
    char sndBuf[BUFFER_SIZE];

    // get IP and port number for arguement
    switch (argc)
    {
    case 3:
        svrIP = argv[1];
        dataPort = atoi(argv[2]); // User specified port
        break;
    default:
        fprintf(stderr, "Please enter the ip and port \n");
        exit(1);
    }

    fprintf(stdout, "IP: %s\n", svrIP);
    fprintf(stdout, "Server Port: %d\n", dataPort);

    // Connect to Server Connection Port
    socketDesc = connectToServer(svrIP, SERVER_LISTEN_PORT);

    // Send Request to Server
    send(socketDesc, getRequestInput(dataPort, reqType, fileName, socketDesc), BUFFER_SIZE, 0);

    while (true)
    {
        // Wait for OKY
        recv(socketDesc, rcvBuf, BUFFER_SIZE, 0);
        fprintf(stdout, "\n(%s)\n", rcvBuf);

        // Receives an OKY
        if (strcmp(rcvBuf, COMMAND_OKAY) == 0)
        {
            close(socketDesc);
            break;
        }
        // Receives a file not found FNF
        else if (strcmp(rcvBuf, COMMAND_FNF) == 0)
        {
            fprintf(stdout, "\nFile you entered is not found (%s)\n", fileName);
            send(socketDesc, getRequestInput(dataPort, reqType, fileName, socketDesc), BUFFER_SIZE, 0);
        }
        // Receives an error ERR meaning the port is already in use
        else
        {
            fprintf(stdout, "\nError, Closing Client, Port %d already in use.\n", dataPort);
            close(socketDesc);
            exit(1);
        }
    }

    // Connect to Server again for File Transfer using the data port
    socketDesc = connectToServer(svrIP, dataPort);

    // File Directory
    char fileDir[FILE_SIZE + 8];
    sprintf(fileDir, "./files/%s", fileName);

    // GET Request - write revceived buffer to file
    if (strcmp(reqType, COMMAND_GET) == 0)
    {
        FILE *file = fopen(fileDir, "w+");
        char *buffer = malloc(sizeof(BUFFER_SIZE));
        while (recv(socketDesc, rcvBuf, BUFFER_SIZE, 0) > 0)
        {
            strcpy(buffer, rcvBuf);
            // Receives a Finish Command FIN, meaning all the file has been read
            if (strcmp(rcvBuf, COMMAND_FINISH) == 0)
            {
                fclose(file);
                fprintf(stdout, "\n\nFile Received\n");
                break;
            }

            // Write the data received to the file
            if (write_file(file, buffer) <= 0)
            {
                fclose(file);
                fprintf(stderr, "Error with writing to the file\n");
                close(socketDesc);
                exit(1);
            }

            // empty out the buffers
            memset(buffer, 0, BUFFER_SIZE);
            memset(rcvBuf, 0, BUFFER_SIZE);
        }
    }
    // SEND Request - read from file and write to buffer
    else if (strcmp(reqType, COMMAND_SEND) == 0)
    {
        memset(sndBuf, 0, BUFFER_SIZE);
        FILE *file = fopen(fileDir, "r");

        // Read the file and send to the server
        while (read_file(file, sndBuf) != 0)
        {
            send(socketDesc, sndBuf, BUFFER_SIZE, 0);
            memset(sndBuf, 0, BUFFER_SIZE);
        }
        // When its done reading, empty out the buffer, and send the FIN command
        memset(sndBuf, 0, BUFFER_SIZE);
        strcpy(sndBuf, COMMAND_FINISH);
        send(socketDesc, sndBuf, BUFFER_SIZE, 0);
    }

    // Clean up and close the socket
    close(socketDesc);
    exit(1);
    return 0;
}

/*--------------------------------------------------------------------------
 * FUNCTION:       connectToServer
 *
 * DATE:           October 4, 2020 
 *
 * REVISIONS:      October 6, 2020 - More Comments
 * 
 * DESIGNER:       Nicole Jingco, Tomas Quat
 *
 * PROGRAMMER:     Nicole Jingco
 *
 * INTERFACE:      char *svrIP - Server IP
 *                  int port   - Server Port
 *
 * RETURNS:        Returns the socket Descriptor
 *
 * NOTES:
 * This function all the fucntions requires to connect to the server
 * -----------------------------------------------------------------------*/
int connectToServer(char *svrIP, int port)
{
    int socketDesc;
    struct sockaddr_in svrDesc;
    struct hostent *hp;

    // Socket ---------------------------------------------------------------
    socketDesc = createSocketDesc();

    // Set server description -----------------------------------------------
    svrDesc = getServerDesc(port);

    // HP ------------------------------------------------------------------
    if ((hp = gethostbyname(svrIP)) == NULL)
    {
        fprintf(stderr, "Unknown server address\n");
        exit(1);
    }

    bcopy(hp->h_addr, (char *)&svrDesc.sin_addr, hp->h_length);

    // Connect -------------------------------------------------------------
    if (!isConnected(socketDesc, svrDesc))
        exit(1);

    return socketDesc;
}

/*--------------------------------------------------------------------------
 * FUNCTION:       createSocketDesc
 *
 * DATE:           October 4, 2020 
 *
 * REVISIONS:      October 6, 2020 - More Comments
 * 
 * DESIGNER:       Nicole Jingco, Tomas Quat
 *
 * PROGRAMMER:     Nicole Jingco
 *
 * INTERFACE:      NA
 *
 * RETURNS:        Returns a socket Descriptor
 *
 * NOTES:
 * Creates and returns the Socket Descriptor
 * -----------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------
 * FUNCTION:       getServerDesc
 *
 * DATE:           October 4, 2020 
 *
 * REVISIONS:      October 6, 2020 - More Comments
 * 
 * DESIGNER:       Nicole Jingco, Tomas Quat
 *
 * PROGRAMMER:     Nicole Jingco
 *
 * INTERFACE:      int port - Server Port
 *
 * RETURNS:        Returns a structure for the server description
 *
 * NOTES:
 * 
 * -----------------------------------------------------------------------*/
struct sockaddr_in getServerDesc(int port)
{
    struct sockaddr_in server;
    // Set server description
    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    return server;
}

/*--------------------------------------------------------------------------
 * FUNCTION:       setServerDesc
 *
 * DATE:           October 4, 2020 
 *
 * REVISIONS:      October 5, Fixed Some bugs with the Request type comparison
 *                 October 6, 2020 - More Comments
 * 
 * DESIGNER:       Nicole Jingco, Tomas Quat
 *
 * PROGRAMMER:     Nicole Jingco
 *
 * INTERFACE:      int sockDesc - Socket Descriptor
 *                 struct sockaddr_in server - Server struct
 *
 * RETURNS:        Returns true if it is connected to the server
 *
 * NOTES:
 * This connects to the server and returns true if connected.
 * -----------------------------------------------------------------------*/
bool isConnected(int sockDesc, struct sockaddr_in server)
{
    if (connect(sockDesc, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        fprintf(stderr, "Can't connect to server\n");
        return false;
    }
    return true;
}

/*--------------------------------------------------------------------------
 * FUNCTION:        getRequestInput
 *
 * DATE:            October 4, 2020 
 *
 * REVISIONS:       October 5, Fixed Some bugs with the Request type comparison
 *                  October 6, 2020 - More Comments
 * 
 * DESIGNER:        Nicole Jingco, Tomas Quat
 *
 * PROGRAMMER:      Nicole Jingco
 *
 * INTERFACE:       int port - Data Port 
 *                  char *reqType - Request Type pointer
 *                  char *file    - File name pointer
 *                  int sockDesc  - Socket Description
 *
 * RETURNS:        Returns the formatted request buffer 
 *
 * NOTES:
 * This will re-prompt the user if they inputted an invalid request type,
 * or file that does not exist. This also has the option exit usign EXT
 * -----------------------------------------------------------------------*/
char *getRequestInput(int port, char *reqType, char *file, int sockDesc)
{
    char *type = malloc(COMMAND_SIZE);
    char *fileName = malloc(BUFFER_SIZE - COMMAND_SIZE);
    char *buffer = malloc(BUFFER_SIZE);
    bool validType = false;

    memset(buffer, 0, BUFFER_SIZE);
    memset(file, 0, BUFFER_SIZE);

    // Get an dvalid date Request type input
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

    // If Request type is send, get input for filename and checks if
    // the file exist in their folder
    if ((strcmp(type, COMMAND_SEND)) == 0)
    {
        while (true)
        {
            // filename
            fprintf(stdout, "Enter the File Name with type (text.txt): ");
            fflush(stdout);
            fscanf(stdin, "%s", fileName);
            if (fileExist(fileName))
            {
                break;
            }
            fprintf(stdout, "File does not exist.");
        }
    }
    // if Request type is get, it will not validate the file
    else
    {
        fprintf(stdout, "Enter the File Name with type (text.txt): ");
        fflush(stdout);
        fscanf(stdin, "%s", fileName);
    }

    // Creates the formatted buffer
    sprintf(buffer, "%s:%d:%s", type, port, fileName);

    // Coppies the type and file name to the pointer
    strcpy(reqType, type);
    strcat(file, fileName);

    return buffer;
}

/*--------------------------------------------------------------------------
 * FUNCTION:       fileExist
 *
 * DATE:           October 4, 2020 
 *
 * REVISIONS:      October 5, Fixed Some bugs with the Request type comparison
 *                 October 6, 2020 - More Comments
 * 
 * DESIGNER:       Nicole Jingco, Tomas Quat
 *
 * PROGRAMMER:     Nicole Jingco
 *
 * INTERFACE:      char *fileName - file name
 *
 * RETURNS:        Returns true if the file exist in the folder
 *
 * NOTES:
 * This checks if the filename exist in the folder.
 * -----------------------------------------------------------------------*/
bool fileExist(char *fileName)
{
    FILE *file = malloc(sizeof(FILE));

    char fileDir[FILE_SIZE + 8];
    sprintf(fileDir, "./files/%s", fileName);

    if ((file = fopen(fileDir, "r")) == NULL)
    {
        return false;
    }
    fclose(file);
    return true;
}