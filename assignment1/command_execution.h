#ifndef COMMAND_EXECUTION_H
#define COMMAND_EXECUTION_H


#include "shell.h"
#include <string.h>
#include "parser.h"

void executeCommand(char *command);
void setCommandType(char *command, cmd *cmdType);
void executeSimpleCommand(char *command);
void getRedirectionType(char *command,cmd *cmdType);
void isPipeCommand(char *command,cmd *cmdType);
void isSemicolonCommand(char *command,cmd *cmdType);
void execute_redirection0(char *command);
void resetStdin();
void execute_redirection1(char *command);
void resetStdout();
void execute_redirection2(char *command);
void execute_semicol(char *command);
void execute_pipe_command(char *command);
int** allocate_pipes(int num_of_pipes);
void deallocate_pipes(int **pipe_pointers, int num_of_pipes);

#endif