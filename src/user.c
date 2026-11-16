#include "user.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config read_config(const char* path) {
    FILE *fp = fopen(path, "r");
    if(!fp) {
        return (struct Config){NULL, NULL, NULL};
    }

    Config config;
    config.base_url = (char*)malloc(sizeof(char) * 256);
    fgets(config.base_url, sizeof(char) * 256, fp);
    config.base_url[strcspn(config.base_url, "\n")] = '\0';
    if(config.base_url[strlen(config.base_url) - 1] != '/') {
        size_t len = strlen(config.base_url);
        config.base_url[len] = '/';
        config.base_url[len + 1] = '\0';
    }

    config.api_key = (char*)malloc(sizeof(char) * 512);
    fgets(config.api_key, sizeof(char) * 512, fp);
    config.api_key[strcspn(config.api_key, "\n")] = '\0';

    config.model_id = (char*)malloc(sizeof(char) * 64);
    fgets(config.model_id, sizeof(char) * 64, fp);
    config.model_id[strcspn(config.model_id, "\n")] = '\0';

    // TODO: better error handling

    return config;
}

void free_config(Config config) {
    free(config.api_key);
    free(config.base_url);
    free(config.model_id);
}