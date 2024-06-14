#include "parser.h"

char** decomposeString(char* inputString, const char* delimiter, int* componentCount) {
    char *command = strtok(inputString, "\n");
    char* copy = strdup(command); // Duplicate the input string
    char* token = strtok(copy, delimiter); // Get the first token

    int count = 0;
    while (token != NULL) {
        token = strtok(NULL, delimiter); // Get the next token
        count++;
    }

    
    char** components = (char**)malloc(count * sizeof(char*));
    if (components == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    // Tokenize the input string and store components in the array
    strcpy(copy, command); // Restore the input string
    token = strtok(copy, delimiter);
    int i = 0;
    while (token != NULL) {
        components[i] = strdup(token); // Duplicate the token and store in the array
        token = strtok(NULL, delimiter);
        i++;
    }

    // Set the componentCount pointer to the number of components
    *componentCount = count;

    // Free dynamically allocated memory for the copy of the input string
    free(copy);

    return components;
}

// Function to free memory allocated for components
void freeComponents(char** components, int componentCount) {
    for (int i = 0; i < componentCount; i++) {
        free(components[i]); // Free memory for each component
    }
    free(components); // Free memory for the array of components
}

