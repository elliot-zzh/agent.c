#include "tool.h"
#include "util.h"

#include <stddef.h>
#include <stdlib.h>
#include <cJSON.h>
#include <string.h>

static Tool* tool_list;
static size_t tool_num = 0;

#define MAX_TOOL_NUM 1024

#define ERR_TOOL_NOT_FOUND "error: tool not found"

void init_tool_list() {
    tool_list = (Tool*)malloc(sizeof(Tool) * MAX_TOOL_NUM);
}

int register_tool(Tool tool) {
    if(tool_num == MAX_TOOL_NUM) return -1;
    tool_list[tool_num++] = tool;
    return 0;
}

ToolResult tool_call(ToolCall tool_call) {
    char* content;
    Tool* tool = NULL;
    for(int i = 0; i < tool_num; ++i) {
        if(!strcmp(tool_call.name, tool_list[i].name)) {
            tool = tool_list + i;
            break;
        }
    }
    if(!tool){
        // tool not found
        content = copy_string(ERR_TOOL_NOT_FOUND);
        return (ToolResult) {
            .id = tool_call.id,
            .name = tool_call.name,
            .content = content,
        };
    }
    tool->tool_call(tool_call.argument, &content);
    return (ToolResult) {
        .name = tool->name,
        .content = content,
        .id=tool_call.id,
    };
}

cJSON* build_tool_calls(ToolCall* tool_calls, size_t n) {
    cJSON* array = cJSON_CreateArray();

    // "tool_calls": [{"id": xxx, "type": "function", "function": {xxx}}]
    for(int i = 0; i < n; ++i) {
        cJSON* item = cJSON_CreateObject();
        cJSON* id = cJSON_CreateString(tool_calls[i].id);
        cJSON* type = cJSON_CreateString("function");

        // "function": { "name": xxx, "arguments": xxx }
        cJSON* function = cJSON_CreateObject();
        cJSON* name = cJSON_CreateString(tool_calls[i].name);
        cJSON* arguments = cJSON_CreateString(tool_calls[i].argument);
        cJSON_AddItemToObject(function, "name", name);
        cJSON_AddItemToObject(function, "arguments", arguments);

        cJSON_AddItemToObject(item, "id", id);
        cJSON_AddItemToObject(item, "type", type);
        cJSON_AddItemToObject(item, "function", function);

        cJSON_AddItemToArray(array, item);
    }
    return array;
}

cJSON* build_tool_msg(ToolResult tool_result) {
    cJSON* json = cJSON_CreateObject();
    cJSON* role = cJSON_CreateString("tool");
    cJSON* content = cJSON_CreateString(tool_result.content);
    cJSON* tool_call_id = cJSON_CreateString(tool_result.id);
    cJSON_AddItemToObject(json, "role", role);
    cJSON_AddItemToObject(json, "content", content);
    cJSON_AddItemToObject(json, "tool_call_id", tool_call_id);
    return json;
}