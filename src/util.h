#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <cJSON.h>

// return a heap copy of src; caller must free it
// return NULL when src is NULL or allocation fails
char* copy_string(const char* src);

// return NULL if not found
const cJSON* find_key(const cJSON* node, const char* key);
const cJSON* find_index(const cJSON* node, size_t idx);

#endif
