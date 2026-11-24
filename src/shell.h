#ifndef SHELL_H
#define SHELL_H

#include <stddef.h>

// return the rc of the shell
int run_shell(const char* command, char** res, size_t* res_len);

#endif