#ifndef USER_H
#define USER_H

typedef struct Config {
    char* base_url; // always trailed by '/'
    char* api_key;
    char* model_id;
} Config;

// config file must come in three lines
// l1: base url. e.g. `https://api.deepseek.com`, trailing `/` optional
// l2: api key. no space should be included
// l3: model id. e.g. `deepseek-flash` for the official api of deepseek v4.1 flash 1017
Config read_config(const char* path);

void free_config(Config config);

#endif