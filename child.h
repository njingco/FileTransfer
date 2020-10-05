#ifndef CHILD_H
#define CHILD_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "constants.h"
#include "utilities.h"

int receive_request(int*, char*, int*, char*, int*, char*);
int parse_command(char*, char*);
int validate_command(char*);
int parse_port(int*, char*);

int send_err(int*, char*);
int send_oky(int*, char*);
int send_data(int*, char*);

int command_get_controller(int*, char*, char*);
int command_snd_controller(int*, char*, char*);

#endif
