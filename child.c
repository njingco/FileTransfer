#include "child.h"

int receive_request(int* sd, char* bp, int* btr, char* cmd, int* port, char* fileName)
{
    int result = 0;
    int validRequest = 0;
    int n;
    while(!validRequest)
    {
        while((n = recv(*sd, bp, *btr, 0)) < BUFFER_SIZE)
        {
            bp += n;
            btr -= n;
        }

        int success = -1;
        if(parse_command(cmd, bp) == -1)
        {
            fprintf(stderr, "Error parsing command");
        } else if(validate_command(cmd) == -1)
        {
            fprintf(stdout, "Invalid command received\n");
        }else if(parse_port(port, bp) == -1)
        {
            fprintf(stderr, "Error parsing port");
        } else
        {
            memset(fileName, 0, 255);
            strcpy(fileName, bp+9);
            success = 1;
        }
        if (success != -1)
        {
            validRequest = 1;
        } else {
            send_err(sd, bp);
        }
    }
    return result;
}

int parse_command(char* cmd, char* buffer)
{
    int result = 0;

    if(strncpy(cmd, buffer, 3) == NULL)
    {
        result = -1;
    }
    return result;
}

int validate_command(char* cmd)
{
    int result = 0;
    if((strcmp(cmd, COMMAND_GET) != 0 && strcmp(cmd, COMMAND_SEND) !=0))
    {
        result = -1;
    }
    return result;
}


int parse_port(int* port, char* buffer)
{
    int result = 0;

    char tmp[5];
    if(strncpy(tmp, buffer+4, 4) == NULL)
    {
        result = -1;
    } else {
        memset(tmp+4, 0, 1);
        *port = atoi(tmp);
    }
    return result;
}

int send_err(int* sd, char* buffer)
{
    int result = 0;

    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, COMMAND_ERR);

    if(send(*sd, buffer, BUFFER_SIZE, 0) == -1)
    {
        result = -1;
    }

    return result;
}

int send_oky(int* sd, char*buffer)
{
    int result = 0;
    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, COMMAND_OKAY);

    if(send(*sd, buffer, BUFFER_SIZE, 0) == -1)
    {
        result = -1;
    }
    return result;
}

int send_data(int* sd, char*buffer)
{
    int result = 0;

    if(send(*sd, buffer, BUFFER_SIZE, 0) == -1)
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
    strcpy(filepath+8, fileName);
    if((file = fopen(filepath, "r")) == NULL)
    {
        perror("fopen\n");
    }

    if(fseek(file, 0, SEEK_SET) == -1)
    {
        perror("Error in fseek\n");
    }

    while(read_file(file, buffer) != 0)
    {
        send_data(sd, buffer);
    }
    memset(buffer, 0, BUFFER_SIZE);
    strcpy(buffer, COMMAND_FIN);
    send_data(sd, buffer);
    fprintf(stdout, "FIN sent\n");

    fclose(file);
    free(filepath);
    _exit(0);
}

void command_snd_controller(int* sd, char* buffer, char* fileName)
{
    FILE* file = malloc(sizeof(*file));
    char* filepath = malloc(255);
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
        if(strcmp(buffer, COMMAND_FIN) == 0)
        {
            transfer_complete = 1;
        } else
        {
            //write data to file
        }
    }

    fclose(file);
    free(filepath);
    _exit(0);
}
