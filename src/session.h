#ifndef SESSION_H
#define SESSION_H

typedef struct Msg {
    char* msg;
    char role[16]; // usually short, like "system", "developer", "user", "assistant" or tool
} Msg;

#define THINKING_ON 1
#define THINKING_OFF 0

typedef struct Session {
    char** msgs;
    char* path; // where the session history is stored
    unsigned int rounds;
    int thinking;
    char reasoning_effort[8]; // usually short, like "minimal", "low", "medium", "high", "xhigh", or "max"
} Session;


// return an empty, brand-new session
// should include a system prompt with the role "system"
Session create_session(void);

// return an empty Session if the file does not exist
Session read_session(const char* path);

// append a user message to the session and modify the Session struct in-place
// return 1 when error occurs
int add_msg(Session* session, const char* msg);

// utilities to change session setttings
void thinking_on(Session* session);
void thinking_off(Session* session);

void set_reasoning_effort(Session* session, const char* effort);

#endif