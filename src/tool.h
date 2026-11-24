#ifndef TOOL_H
#define TOOL_H

#include <cJSON.h>

typedef struct Tool {
    char* name;
    char* description;
    char* parameters; // json string. naively no need to be parsed by the program. just sent to the server
    int (*tool_call)(const char* argument, char** content);
} Tool;

typedef struct ToolCall {
    char* name;
    char* id;
    char* argument;
} ToolCall;

typedef struct ToolResult {
    // ToolResult never own name or id
    const char* name;
    const char* id;
    char* content;
} ToolResult;

void init_tool_list(void);
// return non-zero if failing to register a tool
int register_tool(Tool tool);
ToolResult tool_call(ToolCall tool_call);
cJSON* build_tool_calls(ToolCall* tool_calls, size_t n);
cJSON* build_tool_msg(ToolResult tool_result);

#endif