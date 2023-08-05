#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shell.h"
#include "command.h"
#include "utils.h"

#define MAX_LINE_LENGTH 1024

void run_shell() {
    char line[MAX_LINE_LENGTH];
    char *args[MAX_ARGS];

    while (1) {
        printf("MyShell> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        // Remove trailing newline character
        line[strcspn(line, "\n")] = '\0';

               // 检查命令是否为空
        if (strlen(line) == 0) {
            continue;
        }


        // autocomplete(line);
        // printf("\n");

        // Parse command
        parse_command(line, args);

                    if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                // 没有指定目录参数，转到主目录
                chdir(getenv("HOME"));
            } else {
                if (chdir(args[1]) == -1) {
                    perror("chdir");
                }
            }
            continue;
        }

        // Execute command
        execute_command(args);

        // Free dynamically allocated memory
        for (int i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }
    }
}