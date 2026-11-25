#ifndef SHELL_H
#define SHELL_H

#include "tool.h"

#include <stddef.h>

#ifdef __APPLE__
#define SHELL_TOOL_DESC ("Run a zsh command on the user's MacOS machine. Return stdout/stderr and exit code.")
#elif defined(__linux__)
#define SHELL_TOOL_DESC ("Run a bash command on the user's Linux machine. Return stdout/stderr and exit code.")
#elif defined(_WIN32)
#define SHELL_TOOL_DESC ("Run a PowerShell command on the user's Windows machine. Return stdout/stderr and exit code.")
#endif

#define SHELL_TOOL_PARAM ("{" \
      "\"type\": \"object\"," \
      "\"properties\": {" \
        "\"command\": { \"type\": \"string\", \"description\": \"Command to run\" }," \
        "\"timeout\": { \"type\": \"integer\", \"description\": \"Timeout in ms, default no timeout\" }" \
      "}," \
      "\"required\": [\"command\"]" \
    "}")

// return the rc of the shell
int run_shell(const char* command, char** res, size_t* res_len, int timeout_ms);
int shell_tool_call(const cJSON* json, char** content);

#endif