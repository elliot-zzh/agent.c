#include "msg.h"
#include "user.h"
#include "session.h"

#include <curl/curl.h>
#include <cJSON.h>
#include <string.h>

int msg_init() {
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if(result != CURLE_OK) return (int)result;
}

int msg_close() {
    curl_global_cleanup();
    return 0;
}

static char* build_body(const Session* session) {
    // TODO: to be implemented
}

static size_t append_msg(char* contents, size_t size, size_t nmemb, void* userp) {
    size_t real_size = size * nmemb;
    Session* session = (Session*)userp;
    // TODO: actually append the msg into session history
    return real_size;
}

int msg_send(Session* session) {
    Config config = read_config("config.txt");

    CURL* curl;
    CURLcode result;
    curl = curl_easy_init();
    if(!curl) return 1;

    char url[512];
    strcpy(url, config.base_url);
    strcat(url, "v1/chat/completions");

    char api_key_header[512 + 256];
    strcpy(api_key_header, "Authorization: Bearer ");
    strcat(api_key_header, config.api_key);

    // build header
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, api_key_header);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)session);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, append_msg);

    // set body
    char* json_body = build_body(session);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(json_body));

    result = curl_easy_perform(curl);
    if(result != CURLE_OK) return 1;

    free(json_body);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return 0;
}