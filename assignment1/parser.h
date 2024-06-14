#ifndef PARSER_H
#define PARSER_H

#include <fcntl.h>
#include <unistd.h>
#include "shell.h"
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

typedef enum{
    SIMPLE_COMMAND,PIPE_COMMAND,REDIRECTION_COMMAND_0,REDIRECTION_COMMAND_1,REDIRECTION_COMMAND_2,SEQ_COMMAND
}cmd;


char** decomposeString(char* inputString, const char* delimiter, int *commandCount);
void freeComponents(char** components, int componentCount);

#endif
