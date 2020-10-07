/*---------------------------------------------------------------------------------------
 * SOURCE FILE:	    ulitilies.c
 * 
 * PROGRAM:		    server, client
 * 
 * FUNCTIONS:		size_t read_file(FILE *file, char *buffer);
 *                  int write_file(FILE *file, char *buffer);
 * 
 * DATE:			October 4, 2020
 * 
 * REVISIONS:		(Date and Description)
 *                  October 6, 2020 - More Comments
 * 
 * DESIGNERS:
 * 
 * PROGRAMMERS:		
 * 
 * Notes:
 * This file manages all the file reading and writing functions
 * ---------------------------------------------------------------------------------------*/
#include "utilities.h"

/*--------------------------------------------------------------------------
 * FUNCTION:       read_file
 *
 * DATE:           October 4, 2020 
 *
 * REVISIONS:      October 5, Fixed Some bugs with the Request type comparison
 *                 October 6, 2020 - More Comments
 * 
 * DESIGNER:       Nicole Jingco, Tomas Quat
 *
 * PROGRAMMER:     Tomas Quat
 *
 * INTERFACE:      FILE *file - File Name
 *                 char *buffer - Where the read data will be put in
 *
 * RETURNS:        Returns the number of characters read from the file
 *
 * NOTES:
 * 
 * -----------------------------------------------------------------------*/
size_t read_file(FILE *file, char *buffer)
{
    size_t result = 0;
    result = fread(buffer, 1, BUFFER_SIZE, file);
    return result;
}

/*--------------------------------------------------------------------------
 * FUNCTION:       write_file
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
 * INTERFACE:      FILE *file - File Name
 *                 char *buffer - Data to be written in the file
 *
 * RETURNS:        Returns the number of charaters written to the file
 *
 * NOTES:
 * 
 * -----------------------------------------------------------------------*/
int write_file(FILE *file, char *buffer)
{
    fflush(stdin);
    fflush(stdout);
    size_t result = -1;
    result = fwrite(buffer, 1, BUFFER_SIZE, file);
    return result;
}