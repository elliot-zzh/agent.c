#ifndef MSG_H
#define MSG_H

#include "session.h"
#include "user.h"

#include <stdlib.h>
#include <curl/curl.h>
#include <cJSON.h>

int msg_init(void);
int msg_close(void);

// send user message and parse the response
// does not include saving the record
// return NULL when error occurs
int msg_send(const Session* session, const Config* config, Msg* msg, TokenStat* token_stat); 

#endif