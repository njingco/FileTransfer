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

        if ((pid[currentClient] = fork()) == -1)
        {
            fprintf(stderr, "Error creating client process\n");
        } else if (pid[currentClient] == 0) {
            break;
        }
        currentClient++;
    }
    fprintf(stdout, "Child #%d with pid: %d\n",currentClient, getpid());

    return 1;
}
