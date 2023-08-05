#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include "utils.h"


void autocomplete(const char *input) {
    // 获取当前目录
    char current_directory[MAX_LINE_LENGTH];
    getcwd(current_directory, sizeof(current_directory));

    // 提取最后一个参数
    char *last_arg = strrchr(input, ' ');
    if (last_arg == NULL) {
        last_arg = (char *)input;
    } else {
        last_arg++;  // 跳过空格
    }

    // 打开当前目录
    DIR *dir = opendir(current_directory);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    // 逐个比较文件名
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, last_arg, strlen(last_arg)) == 0) {
            printf("%s ", entry->d_name);
        }
    }

    closedir(dir);
}