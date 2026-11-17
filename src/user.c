#include "user.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Config drop_config(FILE* fp, Config config, int line, const char* reason) {
    fprintf(stderr, "config: line %d %s\n", line, reason);
    fclose(fp);
    free_config(config);
    return (Config){NULL, NULL, NULL};
}

Config read_config(const char* path) {
    FILE *fp = fopen(path, "r");
    if(!fp) {
        return (struct Config){NULL, NULL, NULL};
    }

    Config config;
    config.base_url = (char*)malloc(sizeof(char) * 256);
    fgets(config.base_url, sizeof(char) * 256, fp);
    config.base_url[strcspn(config.base_url, "\r\n")] = '\0';
    if(config.base_url[strlen(config.base_url) - 1] != '/') {
        size_t len = strlen(config.base_url);
        config.base_url[len] = '/';
        config.base_url[len + 1] = '\0';
    }

    config.api_key = (char*)malloc(sizeof(char) * 512);
    fgets(config.api_key, sizeof(char) * 512, fp);
    config.api_key[strcspn(config.api_key, "\r\n")] = '\0';

    config.model_id = (char*)malloc(sizeof(char) * 64);
    fgets(config.model_id, sizeof(char) * 64, fp);
    config.model_id[strcspn(config.model_id, "\r\n")] = '\0';

    if(strncmp(config.base_url, "http://", 7) && strncmp(config.base_url, "https://", 8)) {
        return drop_config(fp, config, 1, "must start with http:// or https://");
    }

    size_t end = strlen(config.base_url);
    while(end > 0 && config.base_url[end - 1] == '/') end--;
    if(end >= 2 && config.base_url[end - 2] == 'v' && config.base_url[end - 1] == '1') {
        return drop_config(fp, config, 1, "can't end with 'v1'");
    }

    if(strchr(config.api_key, ' ')) {
        return drop_config(fp, config, 2, "can't contain a space");
    }

    if(config.model_id[0] == '\0') {
        return drop_config(fp, config, 3, "can't be empty");
    }

    fclose(fp);
    return config;
}

void free_config(Config config) {
    free(config.api_key);
    free(config.base_url);
    free(config.model_id);
}