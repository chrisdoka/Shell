#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define CMDSIZE 100
#define CWDSIZE 100

void printPrompt(void);
void runShell(void);

#endif

