#include "server.h"

pid_t pid[CLIENT_MAX];
int control_sockets[CLIENT_MAX];

int main(int argc, char** argv)
{
    int listen_sd, control_sd, data_sd;
    int listen_port = SERVER_LISTEN_PORT;
    int data_port;

    struct sockaddr_in server, client;
    socklen_t client_len;

    char *bp, buffer[BUFFER_LENGTH];
    int n, bytes_to_read;

    char command[COMMAND_LENGTH];
    char* fileName;
    FILE* file;

    int currentClient = 0;

    if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error creating socket\n");
        exit(1);
    }

    bzero((char*)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(listen_port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_sd, (struct sockaddr*) &server, sizeof(server)) == -1)
    {
        perror("Error binding name to socket\n");
        exit(1);
    }

    listen(listen_sd, 5);

    while(1)
    {
        client_len = sizeof(client);
        if ((control_sd = accept(listen_sd, (struct sockaddr*) &client, &client_len)) == -1)
        {
            fprintf(stderr, "Error accepting client\n");
            exit(1);
        }
        fprintf(stdout, "Client connected with remote address: %s\n", inet_ntoa(client.sin_addr));
        control_sockets[currentClient] = control_sd;
        fprintf(stdout, "control_sd: %d\n", control_sd);

        if ((pid[currentClient] = fork()) == -1)
        {
            fprintf(stderr, "Error creating client process\n");
        } else if (pid[currentClient] == 0) {
            break;
        }
        currentClient++;
    }
    fprintf(stdout, "Child #%d with pid: %d\nChild control_sd: %d\n",currentClient, getpid(), control_sd);

    bp = buffer;
    bytes_to_read = BUFFER_LENGTH;

    //Receive transfer command + filename (+port?)
    while ((n = recv(control_sd, bp, bytes_to_read, 0)) < 18)
    {
        bp += n;
        bytes_to_read -= n;
    }

    //Parse command and filename (+port?)
    memset(command, 0, 4);
    strncpy(command, buffer, 3);
    memset(command+3, 0, 1);
    fprintf(stdout, "Command received: %s\n", command);
    //for(int i = 0; i < BUFFER_LENGTH; i++) {
    //}

    return 1;
}
