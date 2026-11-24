#ifndef TOOL_H
#define TOOL_H

#include <cJSON.h>

typedef struct Tool {
    char* name;
    char* description;
    int (*tool_call)(const char* argument, char** content);
} Tool;

typedef struct ToolCall {
    char* name;
    char* id;
    char* argument;
    char* content;
} ToolCall;

ToolCall tool_call(Tool tool, char* id, char* argument);
cJSON* build_tool_call(ToolCall tool_call);
cJSON* build_tool_msg(ToolCall tool_call);

#endif