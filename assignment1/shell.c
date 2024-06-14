#include "shell.h"
#include "command_execution.h"

void printPrompt(void){
    char currentDirectory[CWDSIZE]={0};
    char *currentUser=getlogin();
    getcwd(currentDirectory,CWDSIZE);
    printf("4577-hy345sh@%s:%s ",currentUser, currentDirectory);
}

void runShell(void){
    char command[CMDSIZE]={0};
    while(1){
        printPrompt();
        fgets(command,CMDSIZE,stdin);
        executeCommand(command);
    }
}