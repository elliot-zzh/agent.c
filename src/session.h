#ifndef SESSION_H
#define SESSION_H

typedef struct Msg {
    char* msg;
    const char* role;
} Msg;

#define THINKING_ON 1
#define THINKING_OFF 0

typedef struct Session {
    Msg* msgs;
    const char* path; // where the session history is stored
    const char* model; // the model id
    unsigned int rounds;
    int thinking;
    char reasoning_effort[8]; // usually short, like "minimal", "low", "medium", "high", "xhigh", or "max"
} Session;


// return an empty, brand-new session
// should include a system prompt with the role "system"
Session create_session(void);

// return an empty Session if the file does not exist
Session read_session(const char* path);

void free_session(Session session);

// append a user message to the session and modify the Session struct in-place
// return 1 when error occurs
int add_msg(Session* session, const char* msg);

// utilities to change session setttings
void thinking_on(Session* session);
void thinking_off(Session* session);

void set_reasoning_effort(Session* session, const char* effort);

#endif