#include "session.h"
#include "tool.h"
#include "util.h"

#include <cJSON.h>
#include <string.h>

static const char* SYSTEM_PROMPT =
    "You are a helpful AI assistant that completes tasks for the user.\n"
    "\n"
    "Core behavior:\n"
    "- Be clear, concise, and direct. Avoid filler and unnecessary preamble.\n"
    "- Answer the question that was asked. If the request is ambiguous, state your\n"
    "  assumption and proceed, or ask one focused clarifying question.\n"
    "- Prefer concrete, actionable answers over generalities.\n"
    "\n"
    "Accuracy and honesty:\n"
    "- Do not invent facts, citations, or results. If you don't know or can't verify\n"
    "  something, say so.\n"
    "- Distinguish between what you know, what you infer, and what you're guessing.\n"
    "- If you make a mistake, correct it plainly.\n"
    "\n"
    "Safety:\n"
    "- Do not take destructive or irreversible actions (deleting data, sending\n"
    "  messages, making purchases) without explicit user confirmation.\n"
    "- Refuse harmful requests briefly and explain why, then offer a safe alternative\n"
    "  if possible.\n"
    "\n"
    "Style:\n"
    "- Match the user's level of detail and tone.\n"
    "- Use formatting (lists, code blocks, headings) only when it improves\n"
    "  readability.\n";

Session create_session() {
}

Session read_session(const char* path) {
}

void free_session(Session session) {
}

int add_msg(Session* session, const char* msg) {
}

TokenStat parse_token_stat(const cJSON* json) {
    // "usage": {
    //   "prompt_tokens": xxx,
    //   "completion_tokens": xxx,
    //   "total_tokens": xxx,
    //   "completion_tokens_details": { "reasoning_tokens": xxx },
    //    "prompt_tokens_details": { "cached_tokens": xxx }
    // }
    const cJSON* prompt_tokens = find_key(json, "prompt_tokens");
    const cJSON* total_tokens = find_key(json, "total_tokens");
    const cJSON* cached_tokens = find_key(json, "prompt_tokens_details");
    cached_tokens = find_key(cached_tokens, "cached_tokens");
    return (TokenStat) {
        .cached_tokens = cached_tokens->valueint,
        .total_tokens = total_tokens->valueint,
        .cache_hit_rate = prompt_tokens->valueint ? (float)(cached_tokens->valueint) / prompt_tokens->valueint : 0,
    };
}

cJSON* build_msg(const Msg* msg) {
    if(!strcmp(msg->role, "tool")) return build_tool_msg(msg->tool_result);
    cJSON* item = cJSON_CreateObject();
    cJSON* role = cJSON_CreateString(msg->role);
    cJSON* content = msg->msg ? cJSON_CreateString(msg->msg) : cJSON_CreateNull();
    cJSON_AddItemToObject(item, "role", role);
    cJSON_AddItemToObject(item, "content", content);
    if(msg->tool_call_num > 0) {
        cJSON_AddItemToObject(item, "tool_calls", build_tool_calls(msg->tool_calls, msg->tool_call_num));
    }
    return item;
}

int parse_msg(const cJSON* json, Msg* msg) {
    // find json.content
    // find json.tool_calls
    const cJSON* tool_calls_json = find_key(json, "tool_calls");
    const cJSON* content = find_key(json, "content");

    // parse content
    char* msg_str = NULL;
    if(content && !cJSON_IsNull(content)) msg_str = copy_string(content->valuestring);
    *msg = (Msg) {
        .role = "assistant",
        .msg = msg_str,
    };

    // parse tool call
    if(tool_calls_json) {
        ToolCall* tool_calls;
        size_t tool_call_num = parse_tool_calls(tool_calls_json, &tool_calls);
        msg->tool_call_num = tool_call_num;
        msg->tool_calls = tool_calls;
    }
    return 0;
}

cJSON* build_session(const Session* session) {
    cJSON* json = cJSON_CreateObject();

    cJSON* model = cJSON_CreateString(session->model);
    cJSON_AddItemToObject(json, "model", model);

    cJSON* stream = cJSON_CreateBool(0);
    cJSON_AddItemToObject(json, "stream", stream);

    cJSON* reasoning_effort = cJSON_CreateString(session->reasoning_effort);
    cJSON_AddItemToObject(json, "reasoning_effort", reasoning_effort);

    // "thinking": { "type": "enabled" }
    cJSON* thinking = cJSON_CreateObject();
    cJSON* type;
    if(session->thinking == THINKING_ON) type = cJSON_CreateString("enabled");
    else type = cJSON_CreateString("disabled");
    cJSON_AddItemToObject(thinking, "type", type);
    cJSON_AddItemToObject(json, "thinking", thinking);

    // "messages": [{"role": "xxx", "content": "xxx"}]
    cJSON* messages = cJSON_CreateArray();
    for(int i = 0; i < session->rounds; ++i) {
        cJSON_AddItemToArray(messages, build_msg(session->msgs + i));
    }
    cJSON_AddItemToObject(json, "messages", messages);

    cJSON_AddItemToObject(json, "tools", build_tool_list());

    return json;
}