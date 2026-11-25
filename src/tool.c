#include "tool.h"
#include "util.h"
#include "shell.h"

#include <stddef.h>
#include <stdlib.h>
#include <cJSON.h>
#include <string.h>

// TODO: use hash table
static Tool* tool_list;
static size_t tool_num = 0;

#define MAX_TOOL_NUM 1024

#define ERR_TOOL_NOT_FOUND "error: tool not found"

void tool_list_init(void) {
    tool_list = (Tool*)malloc(sizeof(Tool) * MAX_TOOL_NUM);

    // add built-in tools

    // shell command running
    register_tool((Tool) {
        .name = copy_string("shell"),
        .description = copy_string(SHELL_TOOL_DESC),
        .parameters = copy_string(SHELL_TOOL_PARAM),
        .tool_call = shell_tool_call,
    });
}

void tool_list_close(void) {
    for(int i = 0; i < tool_num; ++i) {
        free(tool_list[i].name);
        free(tool_list[i].description);
        free(tool_list[i].parameters);
    }
    free(tool_list);
}

cJSON* build_tool_list(void) {
    cJSON* array = cJSON_CreateArray();
    for(int i = 0; i < tool_num; ++i) {
        cJSON* tool = cJSON_CreateObject();
        cJSON* type = cJSON_CreateString("function");
        cJSON* function = cJSON_CreateObject();
        cJSON* name = cJSON_CreateString(tool_list[i].name);
        cJSON* description = cJSON_CreateString(tool_list[i].description);
        cJSON* parameters = cJSON_Parse(tool_list[i].parameters);
        cJSON_AddItemToObject(function, "name", name);
        cJSON_AddItemToObject(function, "description", description);
        cJSON_AddItemToObject(function, "parameters", parameters);
        cJSON_AddItemToObject(tool, "type", type);
        cJSON_AddItemToObject(tool, "function", function);
        cJSON_AddItemToArray(array, tool);
    }
    return array;
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
    cJSON* json_argument = cJSON_Parse(tool_call.arguments);
    tool->tool_call(json_argument, &content); // TODO: process rc
    cJSON_Delete(json_argument);
    return (ToolResult) {
        .name = tool->name,
        .content = content,
        .id = tool_call.id,
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
        cJSON* arguments = cJSON_CreateString(tool_calls[i].arguments);
        cJSON_AddItemToObject(function, "name", name);
        cJSON_AddItemToObject(function, "arguments", arguments);

        cJSON_AddItemToObject(item, "id", id);
        cJSON_AddItemToObject(item, "type", type);
        cJSON_AddItemToObject(item, "function", function);

        cJSON_AddItemToArray(array, item);
    }
    return array;
}

size_t parse_tool_calls(const cJSON* json, ToolCall** tool_calls) {
    // assume json is an array
    // count tool calls
    size_t tool_call_num = 0;
    for(cJSON* node = json->child; node; node = node->next) ++tool_call_num;
    *tool_calls = (ToolCall*)malloc(sizeof(ToolCall) * tool_call_num);
    size_t i = 0;
    for(cJSON* node = json->child; node; node = node->next, ++i) {
        // { "id": xxx, "function": { "name": xxx, "arguments": xxx}}
        // TODO: handle unexpected parsing
        char* id = copy_string(find_key(node, "id")->valuestring);
        const cJSON* function = find_key(node, "function");
        char* name = copy_string(find_key(function, "name")->valuestring);
        char* arguments = copy_string(find_key(function, "arguments")->valuestring);
        (*tool_calls)[i] = (ToolCall) {
            .id = id,
            .name = name,
            .arguments = arguments,
        };
    }
    return tool_call_num;
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