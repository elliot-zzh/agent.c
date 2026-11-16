#ifndef MSG_H
#define MSG_H

#include <stdlib.h>
#include <curl/curl.h>
#include <cJSON.h>

int msg_init();
int msg_close();

// return NULL when error occurs
const char* msg_send(const char* msg); 

#endif