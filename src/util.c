#include "util.h"

#include <stdlib.h>
#include <string.h>

char* copy_string(const char* src) {
    if(!src) return NULL;
    char* dst = (char*)malloc(strlen(src) + 1);
    if(!dst) return NULL;
    strcpy(dst, src);
    return dst;
}
