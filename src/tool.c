#include "tool.h"

#include <stddef.h>
#include <cJSON.h>

ToolCall tool_call(Tool tool, char* id, char* argument) {
    char* content;
    tool.tool_call(argument, &content);
    return (ToolCall) {
        .name = tool.name,
        .argument = argument,
        .content = content,
        .id=id,
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

cJSON* build_tool_msg(ToolCall tool_call) {
    cJSON* json = cJSON_CreateObject();
    cJSON* role = cJSON_CreateString("tool");
    cJSON* content = cJSON_CreateString(tool_call.content);
    cJSON* tool_call_id = cJSON_CreateString(tool_call.id);
    cJSON_AddItemToObject(json, "role", role);
    cJSON_AddItemToObject(json, "content", content);
    cJSON_AddItemToObject(json, "tool_call_id", tool_call_id);
    return json;
}