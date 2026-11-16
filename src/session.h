#ifndef SESSION_H
#define SESSION_H

typedef struct Session {
    char** msgs;
    char* path; // where the session history is stored
    unsigned int rounds;
} Session;


// return an empty, brand-new session
Session create_session(void);

// return an empty Session if the file does not exist
Session read_session(const char* path);

// append a user message to the session and modify the Session struct in-place
// return 1 when error occurs
int add_msg(Session* session, const char* msg);

#endif