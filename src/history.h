#ifndef HISTORY_H
#define HISTORY_H

#include "session.h"

int create_history();

// return an empty session if the file does not exist
Session read_history(const char* path);

int save_history(const Session* session);

#endif