#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include<readline/history.h>
#include "shell.h"
#include "command.h"
#include "utils.h"


void run_shell() {
     char *line;
    char *args[MAX_ARGS];

    while (1) {
        line = readline("MyShell> ");

        if (line == NULL) {
            break;
        }

        // 检查命令是否为空
        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        // 添加输入到历史记录
        add_history(line);

        // Parse command
        parse_command(line, args);

        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                continue;
            } else {
                if (chdir(args[1]) == -1) {
                    perror("chdir");
                }
            }
            free(line);
            continue;
        }

        // Execute command
        execute_command(args);

        // Free dynamically allocated memory
        for (int i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }

        free(line);
    }
}