#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include "command.h"
#include "utils.h"

void execute_command(char **args) {
    pid_t pid;
    int status;


    pid = fork();
    if (pid == 0) {
        // Child process
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Fork failed
        perror("fork");
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

void parse_command(const char *command, char **args) {
    char *token;
    int i = 0;

    token = strtok((char *)command, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i] = strdup(token);
        token = strtok(NULL, " ");
        i++;
    }
    args[i] = NULL;
}