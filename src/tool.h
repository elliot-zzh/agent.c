#ifndef TOOL_H
#define TOOL_H

#include <cJSON.h>

typedef struct Tool {
    char* name;
    char* description;
    char* parameters; // json stored in string
    int (*tool_call)(const cJSON* argument, char** content); // return non-zero on failing
} Tool;

typedef struct ToolCall {
    char* name;
    char* id;
    char* arguments;
} ToolCall;

typedef struct ToolResult {
    // ToolResult never own name or id
    const char* name;
    const char* id;
    char* content;
} ToolResult;

void tool_list_init(void);
void tool_list_close(void);

// return non-zero if failing to register a tool
int register_tool(Tool tool);
cJSON* build_tool_list(void);

ToolResult tool_call(ToolCall tool_call);
cJSON* build_tool_calls(ToolCall* tool_calls, size_t n);
size_t parse_tool_calls(const cJSON* json, ToolCall** tool_calls); // return the number of calls

cJSON* build_tool_msg(ToolResult tool_result);

#endif