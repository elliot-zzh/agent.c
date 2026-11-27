#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <cJSON.h>

char* copy_string(const char* src) {
    if(!src) return NULL;
    char* dst = (char*)malloc(strlen(src) + 1);
    if(!dst) return NULL;
    strcpy(dst, src);
    return dst;
}

const cJSON* find_key(const cJSON* node, const char* key) {
    if(!node) return NULL;
    // we assume *node is an Object
    for(cJSON* i = node->child; i; i = i->next) {
        if(!strcmp(i->string, key)) return i;
    }
    return NULL;
}

const cJSON* find_index(const cJSON* node, size_t idx) {
    if(!node) return NULL;
    node = node->child;
    for(size_t i = 0; node && i < idx; i++) node = node->next;
    return node;
}

