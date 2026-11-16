#ifndef MSG_H
#define MSG_H

#include <stdlib.h>
#include <curl/curl.h>
#include <cJSON.h>

int msg_init(void);
int msg_close(void);

// return NULL when error occurs
char* msg_send(const char* msg); 

#endif