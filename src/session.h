#ifndef SESSION_H
#define SESSION_H

#include "tool.h"

#include <cJSON.h>

typedef struct Msg {
    char* msg;
    const char* role;
    ToolCall* tool_calls;
    size_t tool_call_num;
    ToolResult tool_result;
} Msg;

typedef struct TokenStat {
    int cached_tokens;
    int total_tokens;
    float cache_hit_rate;
} TokenStat;

#define THINKING_ON 1
#define THINKING_OFF 0

typedef struct Session {
    Msg* msgs;
    const char* path; // where the session history is stored
    const char* model; // the model id
    unsigned int rounds;
    int thinking;
    char reasoning_effort[8]; // usually short, like "minimal", "low", "medium", "high", "xhigh", or "max"
    TokenStat token_stat;
} Session;

// append a user message to the session and modify the Session struct in-place
// return 1 when error occurs
int add_msg(Session* session, const char* msg);
cJSON* build_msg(const Msg* msg);
// return non-zero when error occurs
int parse_msg(const cJSON* json, Msg* msg);

TokenStat parse_token_stat(const cJSON* json);

// return an empty, brand-new session
// should include a system prompt with the role "system"
Session create_session(void);

// return an empty Session if the file does not exist
Session read_session(const char* path);

cJSON* build_session(const Session* session);

void free_session(Session session);

// utilities to change session setttings
void thinking_on(Session* session);
void thinking_off(Session* session);

void set_reasoning_effort(Session* session, const char* effort);

#endif