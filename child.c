#include "child.h"

int receive_request(int *sd, char *bp, int *btr, char *cmd, int *port, char *fileName)
{
    int result = 0;
    int validRequest = 0;
    int n;
    int fileLength = 0;
    FILE* file = malloc(sizeof(*file));
    memset(file, 0, sizeof(*file));
    char* filepath = malloc(255);
    while (!validRequest)
    {
        int bytes_to_read = *btr;
        char* buff = bp;
        while((n = recv(*sd, bp, bytes_to_read, 0)) < BUFFER_SIZE)
        {
            buff += n;
            bytes_to_read -= n;
        }
        fprintf(stdout, "Received: %s\n", bp);
        fflush(stdout);
        int success = -1;
        if (parse_command(cmd, bp) == -1)
        {
            fprintf(stderr, "Error parsing command");
        }
        else if (validate_command(cmd) == -1)
        {
            fprintf(stdout, "Invalid command received\n");
        }
        else if (parse_port(port, bp) == -1)
        {
            fprintf(stderr, "Error parsing port");
        }
        else
        {
            memset(fileName, 0, 255);
            strcpy(fileName, bp + 9);
            strcpy(filepath, "./files/");
            strcpy(filepath+8, fileName);
            fprintf(stdout, "Filepath: %s\n", filepath);
            if((file = fopen(filepath, "r")) == NULL)
            {
                perror("fopen, receive_request\n");
                send_fnf(sd, bp);
                fprintf(stdout, "FNF sent\n");
                fflush(stdout);
                //fclose(file);
                //memset(file, 0, sizeof(*file));
                memset(filepath, 0, 255);
                fprintf(stdout, "filepath cleared: %s\n", filepath);
                fflush(stdout);
                success = 0;
            } else
            {
                fseek(file, 0, SEEK_END);
                fileLength = ftell(file);
                success = 1;
                fclose(file);
            }
        }
        fprintf(stdout, "File Length: %d\n", fileLength);
        fprintf(stdout, "success: %d\n", success);
        if (success == 1)
        {
            validRequest = 1;
        } else if (success == -1)
        {
            send_err(sd, bp);
        }
    }
    return result;
}

int parse_command(char *cmd, char *buffer)
{
    int result = 0;

    if (strncpy(cmd, buffer, 3) == NULL)
    {
        result = -1;
    }
    return result;
}

int validate_command(char *cmd)
{
    int result = 0;
    if ((strcmp(cmd, COMMAND_GET) != 0 && strcmp(cmd, COMMAND_SEND) != 0))
    {
        result = -1;
    }
    return result;
}

int parse_port(int *port, char *buffer)
{
    int result = 0;

    char tmp[5];
    if (strncpy(tmp, buffer + 4, 4) == NULL)
    {
        result = -1;
    }
    else
    {
        memset(tmp + 4, 0, 1);
        *port = atoi(tmp);
    }
    return result;
}

int send_err(int *sd, char *buffer)
{
    int result = 0;

    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, COMMAND_EROR);

    if (send(*sd, buffer, BUFFER_SIZE, 0) == -1)
    {
        result = -1;
    }

    return result;
}

int send_oky(int *sd, char *buffer)
{
    int result = 0;
    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, COMMAND_OKAY);

    if (send(*sd, buffer, BUFFER_SIZE, 0) == -1)
    {
        result = -1;
    }
    return result;
}

int send_fnf(int *sd, char *buffer)
{
    int result = 0;
    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, COMMAND_FNF);

    if(send(*sd, buffer, BUFFER_SIZE, 0) == -1)
    {
        result = -1;
    }
    return result;
}

int send_fin(int *sd, char *buffer)
{
    int result = 0;
    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, COMMAND_FINISH);

    if(send(*sd, buffer, BUFFER_SIZE, 0) == -1)
    {
        result = -1;
    }
    return result;
}

int send_data(int *sd, char *buffer)
{
    int result = 0;

    if (send(*sd, buffer, BUFFER_SIZE, 0) == -1)
    {
        result = -1;
    }
    memset(buffer, 0, BUFFER_SIZE);
    return result;
}

int receive_data(int* sd, char* buffer)
{
    int result = 0;
    int btr = BUFFER_SIZE;
    int n;

    while((n = recv(*sd, buffer, btr, 0)) < BUFFER_SIZE)
    {
        if(n == -1)
        {
            result = -1;
        } else
        {
            buffer += n;
            btr -= n;
        }
    }
    return result;
}

void command_get_controller(int* sd, char* buffer, char* fileName)
{
    FILE* file = malloc(sizeof(*file));
    char* filepath = malloc(255);
    strcpy(filepath, "./files/");
    strcpy(filepath + 8, fileName);
    if ((file = fopen(filepath, "r")) == NULL)
    {
        perror("fopen\n");
        fclose(file);
        free(filepath);
        _exit(0);
    }

    if (fseek(file, 0, SEEK_SET) == -1)
    {
        perror("Error in fseek\n");
    }

    while (read_file(file, buffer) != 0)
    {
        send_data(sd, buffer);
    }
    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, COMMAND_FINISH);
    send_fin(sd, buffer);
    fprintf(stdout, "FIN sent\n");

    fclose(file);
    free(filepath);
    _exit(0);
}

void command_snd_controller(int *sd, char *buffer, char *fileName)
{
    FILE *file = malloc(sizeof(*file));
    char *filepath = malloc(255);

    strcpy(filepath, "./files/");
    strcpy(filepath+8, fileName);

    int transfer_complete = 0;

    if((file = fopen(filepath, "w+")) == NULL)
    {
        perror("fopen\n");
    }

    while(!transfer_complete)
    {
        receive_data(sd, buffer);
        if(strcmp(buffer, COMMAND_FINISH) == 0)
        {
            transfer_complete = 1;
        } else
        {
            write_file(file, buffer);
            memset(buffer, 0, BUFFER_SIZE);
        }
    }

    fclose(file);
    free(filepath);
    _exit(0);
}
