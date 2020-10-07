/*-----------------------------------------------------------------------------
 * SOURCE FILE:    child.c
 *
 * PROGRAM:        server
 *
 * FUNCTIONS:      	int receive_request(int*, char*, int*, char*, int*, char*)
 *					int parse_command(char*, char*)
 * 		   			int validate_command(char*)
 * 		   			int parse_port(int*, char*)
 * 		   			int send_err(int*, char*)
 * 		   			int send_oky(int*, char*)
 * 		   			int send_fnf(int*, char*)
 * 		   			int send_data(int*, char*)
 * 		   			int receive_data(int*, char*)
 * 		   			void command_get_controller(int*, char*, char*)
 *		   			void command_snd_controller(int*, char*, char*)
 *
 * DATE:           October 1st, 2020
 *
 * REVISIONS:      N/A
 *
 * DESIGNER:       Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     Tomas Quat
 *
 * NOTES:
 * This file contains the functions used by the Server child processes for
 * handling the clients requests.
* --------------------------------------------------------------------------*/
#include "child.h"
/*--------------------------------------------------------------------------
 * FUNCTION:       	receive_request
 *
 * DATE:           	October 1st, 2020
 *
 * REVISIONS:      	N/A
 *
 * DESIGNER:       	Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     	Tomas Quat
 *
 * INTERFACE:      	int receive_request(int *sd, char *bp, int *btr, char *cmd, int *port, char *fileName)
 *						int *sd: A pointer to an integer representing a socket descriptor
 *						char* bp: A pointer to a character buffer to hold the received data
 *						int* btr: A pointer to an integer used to determine the amount of data to read
 *						char* cmd: A pointer to a character buffer to hold the parsed command
 *						int* port: A pointer to an integer to hold the parsed data port value
 *						char* fileName: A pointer to a character buffer to hold the parsed filename
 *
 * RETURNS:        	int: error code
 *
 * NOTES:
 *	This function receives and validates the client transfer request
 * -----------------------------------------------------------------------*/
int receive_request(int *sd, char *bp, int *btr, char *cmd, int *port, char *fileName)
{
    int result = 0;
    int validRequest = 0;
    int n;
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
            if((strcmp(cmd, COMMAND_GET)) == 0)
            {
                strcpy(filepath, "./files/");
                strcpy(filepath+8, fileName);
                if((file = fopen(filepath, "r")) == NULL)
                {
                    perror("fopen, receive_request\n");
                    send_fnf(sd, bp);
                    memset(filepath, 0, 255);
                    success = 0;
                } else
                {
                    success = 1;
                    fclose(file);
                }
             } else
             {
                success = 1;
             }
        }
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
/*--------------------------------------------------------------------------
 * FUNCTION:       	parse_command
 *
 * DATE:           	October 1st, 2020
 *
 * REVISIONS:      	N/A
 *
 * DESIGNER:       	Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     	Tomas Quat
 *
 * INTERFACE:      	int parse_command(char *cmd, char *buffer)
 *						char* cmd: A pointer to a character buffer to hold the parsed command string
 *						char* buffer: A pointer to a character buffer containing the unparsed request
 *
 * RETURNS:        	int: error code
 *
 * NOTES:
 *	This function parses the first section of the client request to extract
 *	the command to be executed
 * -----------------------------------------------------------------------*/
int parse_command(char *cmd, char *buffer)
{
    int result = 0;

    if (strncpy(cmd, buffer, 3) == NULL)
    {
        result = -1;
    }
    return result;
}
/*--------------------------------------------------------------------------
 * FUNCTION:       	validate_command
 *
 * DATE:           	October 1st, 2020
 *
 * REVISIONS:      	N/A
 *
 * DESIGNER:       	Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     	Tomas Quat
 *
 * INTERFACE:      	int validate_command(char *cmd)
 *						char* cmd: A pointer to a character containing the command string to validate
 *
 * RETURNS:        	int: error code
 *
 * NOTES:
 *	This function checks to validate whether or not the received command
 *	is either GET or SND
 * -----------------------------------------------------------------------*/
int validate_command(char *cmd)
{
    int result = 0;
    if ((strcmp(cmd, COMMAND_GET) != 0 && strcmp(cmd, COMMAND_SEND) != 0))
    {
        result = -1;
    }
    return result;
}
/*--------------------------------------------------------------------------
 * FUNCTION:       	parse_port
 *
 * DATE:           	October 1st, 2020
 *
 * REVISIONS:      	N/A
 *
 * DESIGNER:       	Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     	Tomas Quat
 *
 * INTERFACE:      	int parse_port(int *port, char *buffer)
 *						int *port: A pointer to an integer to hold the converted port value
 *						char* buffer: A pointer to a character buffer containing the unparsed request
 *
 * RETURNS:        	int: error code
 *
 * NOTES:
 *	This function parses the second section of the client request to extract
 *	the port number, and converts it from string to integer for later use
 * -----------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------
 * FUNCTION:       	send_err
 *
 * DATE:           	October 1st, 2020
 *
 * REVISIONS:      	N/A
 *
 * DESIGNER:       	Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     	Tomas Quat
 *
 * INTERFACE:      	int send_err(int *sd, char *buffer)
 *						int *sd: A pointer to an integer containing the socket descriptor
 *						char* buffer: A pointer to a character buffer to hold the message
 *
 * RETURNS:        	int: error code
 *
 * NOTES:
 *	This function loads the code ERR into the send buffer and writes the
 *	data to the supplied socket
 * -----------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------
 * FUNCTION:       	send_oky
 *
 * DATE:           	October 1st, 2020
 *
 * REVISIONS:      	N/A
 *
 * DESIGNER:       	Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     	Tomas Quat
 *
 * INTERFACE:      	int send_oky(int *sd, char *buffer)
 *						int *sd: A pointer to an integer containing the socket descriptor
 *						char* buffer: A pointer to a character buffer to hold the message
 *
 * RETURNS:        	int: error code
 *
 * NOTES:
 *	This function loads the code OKY into the send buffer and writes the
 *	data to the supplied socket
 * -----------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------
 * FUNCTION:       	send_fnf
 *
 * DATE:           	October 1st, 2020
 *
 * REVISIONS:      	N/A
 *
 * DESIGNER:       	Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     	Tomas Quat
 *
 * INTERFACE:      	int send_fnf(int *sd, char *buffer)
 *						int *sd: A pointer to an integer containing the socket descriptor
 *						char* buffer: A pointer to a character buffer to hold the message
 *
 * RETURNS:        	int: error code
 *
 * NOTES:
 *	This function loads the code FNF into the send buffer and writes the
 *	data to the supplied socket
 * -----------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------
 * FUNCTION:       	send_fin
 *
 * DATE:           	October 1st, 2020
 *
 * REVISIONS:      	N/A
 *
 * DESIGNER:       	Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     	Tomas Quat
 *
 * INTERFACE:      	int send_fin(int *sd, char *buffer)
 *						int *sd: A pointer to an integer containing the socket descriptor
 *						char* buffer: A pointer to a character buffer to hold the message
 *
 * RETURNS:        	int: error code
 *
 * NOTES:
 *	This function loads the code FIN into the send buffer and writes the
 *	data to the supplied socket
 * -----------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------
 * FUNCTION:       	send_data
 *
 * DATE:           	October 1st, 2020
 *
 * REVISIONS:      	N/A
 *
 * DESIGNER:       	Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     	Tomas Quat
 *
 * INTERFACE:      	int send_data(int *sd, char *buffer)
 *						int *sd: A pointer to an integer containing the socket descriptor
 *						char* buffer: A pointer to a character buffer containing the data to be sent
 *
 * RETURNS:        	int: error code
 *
 * NOTES:
 *	This function sends the data in the provided buffer over the provided socket
 *	and clears the buffer
 * -----------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------
 * FUNCTION:       	receive_data
 *
 * DATE:           	October 1st, 2020
 *
 * REVISIONS:      	N/A
 *
 * DESIGNER:       	Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     	Tomas Quat
 *
 * INTERFACE:      	int receive_data(int* sd, char* buffer)
 *						int *sd: A pointer to an integer containing the socket descriptor
 *						char* buffer: A pointer to a character buffer to store the received data
 *
 * RETURNS:        	int: error code
 *
 * NOTES:
 *	This function reads the data from the specified socket and stores it into
 *	the provided buffer
 * -----------------------------------------------------------------------*/
int receive_data(int* sd, char* buffer)
{
    int btr = BUFFER_SIZE;
    int n;

    while((n = recv(*sd, buffer, btr, 0)) < BUFFER_SIZE)
    {
        if(n == -1)
        {
            return n;
        } else
        {
            buffer += n;
            btr -= n;
        }
    }
    return n;
}
/*--------------------------------------------------------------------------
 * FUNCTION:       	command_get_controller
 *
 * DATE:           	October 1st, 2020
 *
 * REVISIONS:      	N/A
 *
 * DESIGNER:       	Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     	Tomas Quat
 *
 * INTERFACE:      	void command_get_controller(int* sd, char* buffer, char* fileName)
 *						int *sd: A pointer to an integer containing the socket descriptor
 *						char* buffer: A pointer to a character buffer used throughout the GET operation
 *						char* fileName: A pointer to a character buffer containing the requested filename
 *
 * RETURNS:        	void
 *
 * NOTES:
 *	This function is the main handler of the GET operation
 * -----------------------------------------------------------------------*/
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
    fprintf(stdout, "File sent\n");

    fclose(file);
    free(filepath);
    _exit(0);
}
/*--------------------------------------------------------------------------
 * FUNCTION:       	command_snd_controller
 *
 * DATE:           	October 1st, 2020
 *
 * REVISIONS:      	N/A
 *
 * DESIGNER:       	Tomas Quat, Nicole Jingco
 *
 * PROGRAMMER:     	Tomas Quat
 *
 * INTERFACE:      	void command_snd_controller(int *sd, char *buffer, char *fileName)
 *						int *sd: A pointer to an integer containing the socket descriptor
 *						char* buffer: A pointer to a character buffer used throughout the GET operation
 *						char* fileName: A pointer to a character buffer containing the requested filename
 *
 * RETURNS:        	void
 *
 * NOTES:
 *	This function is the main handler of the SEND operation
 * -----------------------------------------------------------------------*/
void command_snd_controller(int *sd, char *buffer, char *fileName)
{
    FILE *file = malloc(sizeof(*file));
    char *filepath = malloc(255);

    strcpy(filepath, "./files/");
    strcpy(filepath+8, fileName);

    if((file = fopen(filepath, "w+")) == NULL)
    {
        perror("fopen, snd_controller\n");
    }

    char* buff = malloc(BUFFER_SIZE);

    while(recv(*sd, buffer, BUFFER_SIZE, 0) > 0)
    {
        strcpy(buff, buffer);
        if(strcmp(buffer, COMMAND_FINISH) == 0)
        {
            fclose(file);
            fprintf(stdout, "File received!\n");
            break;
        }
        if (write_file(file, buff) <= 0)
        {
            fclose(file);
            fprintf(stderr, "Error writing to file\n");
            close(*sd);
        }
        memset(buffer, 0, BUFFER_SIZE);
        memset(buff, 0, BUFFER_SIZE);
    }
    fprintf(stdout, "File received\n");

    fclose(file);
    free(filepath);
    _exit(0);
}
