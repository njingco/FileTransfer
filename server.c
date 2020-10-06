#include "server.h"

pid_t pid[CLIENT_MAX];
int control_sockets[CLIENT_MAX];

int main(int argc, char **argv)
{
    int listen_sd, control_sd, data_sd;
    int listen_port = SERVER_LISTEN_PORT;
    int data_port;

    struct sockaddr_in server, data_server, client, data_client;
    socklen_t client_len, data_client_len;

    char *bp, buffer[BUFFER_SIZE];
    int bytes_to_read;

    char command[COMMAND_LENGTH];
    char *fileName = (char *)malloc(sizeof(char *));

    int currentClient = 0;

    if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error creating socket\n");
        exit(1);
    }

    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(listen_port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_sd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("Error binding name to socket\n");
        exit(1);
    }

    listen(listen_sd, 5);

    while (1)
    {
        client_len = sizeof(client);
        if ((control_sd = accept(listen_sd, (struct sockaddr *)&client, &client_len)) == -1)
        {
            fprintf(stderr, "Error accepting client\n");
            exit(1);
        }
        fprintf(stdout, "Client #%d connected!\n", currentClient+1);
        control_sockets[currentClient] = control_sd;

        if ((pid[currentClient] = fork()) == -1)
        {
            fprintf(stderr, "Error creating client process\n");
        }
        else if (pid[currentClient] == 0)
        {
            break;
        }
        currentClient++;
    }
    fprintf(stdout, "Process created for client #%d\n", currentClient+1);
    fflush(stdout);

    bp = buffer;
    bytes_to_read = BUFFER_SIZE;

    //Receive transfer command:port:filename)
    receive_request(&control_sd, bp, &bytes_to_read, command, &data_port, fileName);
    fprintf(stdout, "Request validated for Client #%d\nCommand: %s, Port: %d, File: %s\n", currentClient+1, command, data_port, fileName);

    //Create, bind, accept data listening socket
    if ((data_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error creating socket\n");
        exit(2);
    }
    bzero((char *)&data_server, sizeof(struct sockaddr_in));
    data_server.sin_family = AF_INET;
    data_server.sin_port = htons(data_port);
    data_server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(data_sd, (struct sockaddr *)&data_server, sizeof(data_server)) == -1)
    {
        perror("Error binding name to socket\n");
        send_err(&control_sd, buffer);
        exit(2);
    }
    listen(data_sd, 5);
    data_client_len = sizeof(data_client);
    send_oky(&control_sd, buffer);
    if ((data_sd = accept(data_sd, (struct sockaddr *)&data_client, &data_client_len)) == -1)
    {
        fprintf(stderr, "Error accepting data connection\n");
    }

    if (strcmp(command, COMMAND_GET) == 0)
    {
        command_get_controller(&data_sd, bp, fileName);
    }
    else if (strcmp(command, COMMAND_SEND) == 0)
    {
        command_snd_controller(&data_sd, bp, fileName);
    }

    while (1)
        ;

    return 1;
}
