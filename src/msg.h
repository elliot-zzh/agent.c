#ifndef MSG_H
#define MSG_H

#include "session.h"

#include <stdlib.h>
#include <curl/curl.h>
#include <cJSON.h>

int msg_init(void);
int msg_close(void);

// send user message and append the reponse to the seesion history
// does not include saving the record
// return NULL when error occurs
int msg_send(Session* session); 

#endif