#ifndef UTIL_H
#define UTIL_H

// return a heap copy of src; caller must free it
// return NULL when src is NULL or allocation fails
char* copy_string(const char* src);

#endif
