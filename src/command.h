#ifndef COMMAND_H
#define COMMAND_H

void parse_command(const char *command, char **args);
void execute_command(char **args);

#endif