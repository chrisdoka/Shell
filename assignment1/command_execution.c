#include "command_execution.h"
#include "parser.h"

int inStream=-1;
int outStream=-1;
int input_stream_reader = -1;
int input_stream_writer = -1;
int output_stream_reader = -1;
int output_stream_writer = -1;

void executeCommand(char *command){
    cmd cmdType = SIMPLE_COMMAND;
    isPipeCommand(command,&cmdType);
    getRedirectionType(command,&cmdType);
    isSemicolonCommand(command,&cmdType);
    if(cmdType == REDIRECTION_COMMAND_0) {
        execute_redirection0(command);
    }
    else if(cmdType==SEQ_COMMAND){
        execute_semicol(command);
    }
    else if(cmdType == REDIRECTION_COMMAND_1) {
         execute_redirection1(command);
    }
    else if(cmdType == REDIRECTION_COMMAND_2) {
        execute_redirection2(command);
    }
    else if(cmdType==PIPE_COMMAND){
        execute_pipe_command(command);
    }
    else if(cmdType==SIMPLE_COMMAND){
        executeSimpleCommand(command);
    }
}

void getRedirectionType(char *command,cmd *cmdType){
    for(int i=0;i<strlen(command);i++){
        if(command[i]=='<'){
            *cmdType=REDIRECTION_COMMAND_0;
        }
        else if(command[i]=='>'){
            *cmdType=REDIRECTION_COMMAND_1;
        }
        else if(command[i]=='>'&& command[i+1]=='>'){
            *cmdType=REDIRECTION_COMMAND_2;
        }
    }
     
}

void isPipeCommand(char *command,cmd *cmdType){
    for(int i=0;i<strlen(command);i++){
        if(command[i]=='|'){
            *cmdType=PIPE_COMMAND;
        } 
    }

}

void isSemicolonCommand(char *command,cmd *cmdType){
  for(int i=0;i<strlen(command);i++){
        if(command[i]==';'){
            *cmdType=SEQ_COMMAND;
        } 
    }
    
}




void executeSimpleCommand(char *command){
    int commandCount = 0;
    char **parsedCommand=decomposeString(command, " ", &commandCount);

    if(strcmp("cd",parsedCommand[0])==0){
        char *trimmedPath = strtok(parsedCommand[1], " \t\n");
        if (chdir(trimmedPath) != 0) 
            perror("chdir");
        else 
            return;
    }
    else if(commandCount == 1){
        char *trimmedCommand = strtok(command, " \t\n");
        if(strcmp(trimmedCommand, "quit") == 0) {
            printf("you are exiting shell !!\n");
            exit(1);
        }
    }

    pid_t pid = fork();

    if(pid==0){
        char firstCommand[CMDSIZE]={0};
        char commandPath[CMDSIZE]="/usr/bin/";

        // pipes
        if(outStream==STDOUT_FILENO){
            close(output_stream_reader);
            dup2(output_stream_writer,1);
            close(output_stream_writer);
        }
        if(inStream==STDIN_FILENO){
            close(input_stream_writer); 
            dup2(input_stream_reader,0); 
            close(input_stream_reader);
        }

        strcpy(firstCommand,parsedCommand[0]);
        strcat(commandPath,firstCommand);
        parsedCommand[0]=commandPath;
        execv(commandPath, parsedCommand);

        strcpy(commandPath,"/bin/");
        strcat(commandPath,firstCommand);
        parsedCommand[0]=commandPath;
        execv(commandPath,parsedCommand);

    }
    else if(pid<0){
        perror("fork");
        exit(1);
    }
    wait(NULL);
}


void execute_semicol(char *command){
    int commandCount = 0;
    char **parsedCommand = decomposeString(command, ";", &commandCount);
    for(int i=0;i<commandCount;i++){
        executeSimpleCommand(parsedCommand[i]);
        
    }

}




void execute_redirection0(char *command){
    int commandCount = 0;
    char **parsedCommand = decomposeString(command, "<", &commandCount);
    char *cmd = parsedCommand[0];
    char *file_name = parsedCommand[1];
    int inputFile = open(file_name, O_RDONLY);
    if(dup2(inputFile, STDIN_FILENO) < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    executeSimpleCommand(cmd);
    close(inputFile);
    resetStdin();
}

void execute_redirection1(char *command){
    int commandCount = 0;
    char **parsedCommand = decomposeString(command, ">", &commandCount);
    char *cmd = parsedCommand[0];
    char *file_name = parsedCommand[1];
    int outFile = open(file_name, O_WRONLY);
    if(dup2(outFile, STDOUT_FILENO) < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    executeSimpleCommand(cmd);
    close(outFile);
    resetStdout();
}


void execute_redirection2(char *command){
     int commandCount = 0;
    char **parsedCommand = decomposeString(command, ">>", &commandCount);
    char *cmd = parsedCommand[0];
    char *file_name = parsedCommand[1];
    int outFile = open(file_name, O_WRONLY | O_APPEND);

    // Move the file pointer to the end of the file
    if (lseek(outFile, 0, SEEK_END) == -1) {
        perror("lseek");
        close(outFile);
        exit(EXIT_FAILURE);
    }

    if (outFile < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (dup2(outFile, STDOUT_FILENO) < 0) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    executeSimpleCommand(cmd);
    close(outFile);
    resetStdout();
}

void resetStdout() {
    int originalStdout = open("/dev/tty", O_WRONLY);
    if (originalStdout == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (dup2(originalStdout, STDOUT_FILENO) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    close(originalStdout);
}

void resetStdin() {
    int originalStdin = open("/dev/tty", O_RDONLY);
    if (originalStdin == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (dup2(originalStdin, STDIN_FILENO) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }

    close(originalStdin);
}


    
void execute_pipe_command(char *command) {
    int commandCount = 0;
    char **parsedCommand = decomposeString(command, "|", &commandCount);
    int **fdArray = allocate_pipes(commandCount - 1);

    //first pipe command
    output_stream_reader=fdArray[0][0];
    output_stream_writer=fdArray[0][1];
    outStream=STDOUT_FILENO;
    close(fdArray[0][1]);
    executeSimpleCommand(parsedCommand[0]);

    //last pipe command
    input_stream_reader=fdArray[0][0];
    input_stream_writer=fdArray[0][1];
    inStream=STDIN_FILENO;
    close(fdArray[0][0]);
    executeSimpleCommand(parsedCommand[1]);
     
    deallocate_pipes(fdArray, 2);
}

int** allocate_pipes(int num_of_pipes) {
    // Allocate memory for an array of pointers to pipes
    int **pipe_pointers = (int **)malloc(sizeof(int *) * num_of_pipes);
    if (pipe_pointers == NULL) {
        perror("Error allocating memory for pointer to pipes");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for each pipe and initialize them
    for (int i = 0; i < num_of_pipes; i++) {
        pipe_pointers[i] = (int *)malloc(sizeof(int) * 2);
        if (pipe_pointers[i] == NULL) {
            perror("Error allocating memory for pipe");
            exit(EXIT_FAILURE);
        }

        // Initialize the pipe
        if (pipe(pipe_pointers[i]) == -1) {
            perror("Error creating pipe");
            exit(EXIT_FAILURE);
        }
    }

    return pipe_pointers;
}

void deallocate_pipes(int **pipe_pointers, int num_of_pipes) {
    // Deallocate memory for pipes
    for (int i = 0; i < num_of_pipes; i++) {
        free(pipe_pointers[i]);
    }

    // Deallocate memory for the array of pointers to pipes
    free(pipe_pointers);
}


