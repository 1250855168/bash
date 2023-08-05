# 我的shell解释器



<img src="image\image-20230805162708561.png" alt="image-20230805162708561" style="zoom:80%;" />

<img src="image\image-20230805162732005.png" alt="image-20230805162732005" style="zoom:80%;" />

### 代码架构

- src/
  - main.c
  - shell.c
  - shell.h
  - command.c
  - command.h
  - utils.c
  - utils.h
- obj/
- bin/
- Makefile
- execute.sh

```makefile
CC = gcc    #选择编译器
CFLAGS = -Wall -Wextra #在-Wall的基础上添加警告信息

TARGET = myshell #可执行文件的名字

SRC_DIR = src  #.h .c文件路径
OBJ_DIR = obj  #.o 文件路径
BIN_DIR = bin  #可执行文件路径

SRCS = $(wildcard $(SRC_DIR)/*.c)  #把这个文件夹里面所有的.c文件 有一个srcs 代替
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))#把srcs里面所有的.c文件 换成.o文件 用objs代替

all: $(BIN_DIR)/$(TARGET)  #all是默认的目标，当你运行make命令时，它将被构建

$(BIN_DIR)/$(TARGET): $(OBJS)  #生成目标文件
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c   #生成.o文件
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/$(TARGET) #删除.o 和 可执行文件

.PHONY: all clean #.PHONY: all clean 声明了两个伪目标：all 和 clean
```

```shell
#!/bin/bash

# 编译
make

# 运行
./bin/myshell

# 清理
make clean
```

```shell
#!/bin/bash

# 编译
make

echo "你自己的路径/my_shell" >> ~/.bashrc

# 运行
./bin/myshell

# 清理
make clean
```

```C
#include "shell.h"

int main() {
    run_shell();
    return 0;
}
```

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include<readline/history.h>
#include "shell.h"
#include "command.h"
#include "utils.h"

/*
run_shell() 函数是Shell的主要执行函数，它包含了一个无限循环，用于接收用户输入并执行相应的命令
*/
void run_shell() {
     char *line;
    char *args[MAX_ARGS];


    while (1) {
    /*
    line = readline("MyShell> "); 用于显示提示符并读取用户输入的命令，并将其存储在 line 变量中
    */
        line = readline("MyShell> ");

/*
用于检查用户是否输入了 EOF（通常是通过按下 Ctrl + D 键），如果是，则退出循环
*/
        if (line == NULL) {
            break;
        }

        /*
         检查用户是否输入了空行，如果是，则重新进入循环等待下一个命令输入
         */
        if (strlen(line) == 0) {
            free(line);
            continue;
        }

        /*
        将用户输入的命令添加到历史记录，以便可以使用上下箭头键来访问之前的命令
        */
        add_history(line);

        /*
        parse_command(line, args); 解析用户输入的命令，将其拆分为参数，并将参数存储在 args 数组中
        */
        parse_command(line, args);

/*
一般来说，Shell程序会将用户输入的命令解析为一个可执行文件的路径和一些参数，然后调用相应的执行函数来执行该命令。但是，cd 命令是一个特殊的命令，它不是一个可执行文件，而是Shell本身提供的内置命令。因此，需要在代码中单独处理 cd 命令
*/

		/*
		检查用户输入的命令是否是 cd 命令，如果是，则处理改变当前工作目录的逻辑
		*/
        if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                // 没有指定目录参数，转到主目录
                chdir(getenv("HOME"));
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
```

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include "command.h"
#include "utils.h"


void parse_command(const char *command, char **args) {
    char *token;
    int i = 0;

    token = strtok((char *)command, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i] = strdup(token);//将拆分的子字符串复制到参数数组的当前索引位置 args[i]。strdup 函数用于复制字符串，并为复制的字符串分配新的内存空间
        token = strtok(NULL, " ");//传递 NULL 作为第一个参数时，它将继续从上一次调用的位置继续拆分剩余的子字符串 token 指向下一个拆分的子字符串
        i++;
    }
    args[i] = NULL;
}

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
        exit(EXIT_FAILURE);//waitpid 函数会暂停父进程的执行。WUNTRACED 参数用于指定在子进程暂停或终止之前，waitpid 函数应该挂起父进程的执行	
    } else {
        // Parent process
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));//WIFEXITED(status) 是一个宏，用于检查子进程是否正常退出  非0 正常
    }                                                        //WIFSIGNALED(status) 是一个宏，用于检查子进程是否被信号终止 非0 有信号
}


```

```c
#ifndef UTILS_H
#define UTILS_H

#define MAX_ARGS 64         

#endif

//辅助函数  
```

