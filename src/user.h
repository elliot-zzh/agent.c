#ifndef USER_H
#define USER_H

typedef struct Config {
    char* base_url;
    char* api_key;
    char* model_id;
} Config;

Config read_config(const char* path);

#endif