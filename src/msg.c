#include "msg.h"
#include "user.h"
#include "session.h"

#include <curl/curl.h>
#include <cJSON.h>
#include <string.h>

int msg_init() {
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if(result != CURLE_OK) return (int)result;
    return 0;
}

int msg_close() {
    curl_global_cleanup();
    return 0;
}

static char* build_body(const Session* session) {
    cJSON* json = cJSON_CreateObject();

    cJSON* model = cJSON_CreateString(session->model);
    cJSON_AddItemToObject(json, "model", model);

    cJSON* stream = cJSON_CreateBool(0);
    cJSON_AddItemToObject(json, "stream", stream);

    cJSON* reasoning_effort = cJSON_CreateString(session->reasoning_effort);
    cJSON_AddItemToObject(json, "reasoning_effort", reasoning_effort);

    // "thinking": { "type": "enabled" }
    cJSON* thinking = cJSON_CreateObject();
    cJSON* type;
    if(session->thinking == THINKING_ON) type = cJSON_CreateString("enabled");
    else type = cJSON_CreateString("disabled");
    cJSON_AddItemToObject(thinking, "type", type);
    cJSON_AddItemToObject(json, "thinking", thinking);

    // "messages": [{"role": "xxx", "content": "xxx"}]
    cJSON* messages = cJSON_CreateArray();
    for(int i = 0; i < session->rounds; ++i) {
        cJSON* item = cJSON_CreateObject();
        cJSON* role = cJSON_CreateString(session->msgs[i].role);
        cJSON* content = cJSON_CreateString(session->msgs[i].msg);
        cJSON_AddItemToObject(item, "role", role);
        cJSON_AddItemToObject(item, "content", content);
        cJSON_AddItemToArray(messages, item);
    }
    cJSON_AddItemToObject(json, "messages", messages);

    char* body = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    return body;
}

static cJSON* find_key(cJSON* node, const char* key) {
    // we assume *node is an Object
    for(cJSON* i = node->child; i; i = i->next) {
        if(!strcmp(i->string, key)) return i;
    }
}

static cJSON* find_index(cJSON* node, size_t idx) {
    node = node->child;
    for(size_t i = 0; i < idx; i++) node = node->next;
    return node;
}

struct MemoryBuffer {
    char* memory;
    size_t size;
};
static size_t write_cb(char* contents, size_t size, size_t nmemb, void* userp) {
    size_t real_size = size * nmemb;
    struct MemoryBuffer* mem = (struct MemoryBuffer*)userp;

    char* ptr = realloc(mem->memory, mem->size + real_size + 1);
    if(!ptr) {
        // out of memory
        return 0;
    }

    mem->memory = ptr;
    memcpy(mem->memory + mem->size, contents, real_size);
    mem->size += real_size;
    mem->memory[mem->size] = '\0'; // ensure 0-termination
    return real_size;
}

static void append_msg(char* contents, Session* session) {
    cJSON* json = cJSON_Parse(contents);

    // find choices[0].message.content
    cJSON* content = find_key(json, "choices");
    content = find_index(content, 0);
    content = find_key(content, "message");
    content = find_key(content, "content");

    // append reponse
    // note that content will be freed later
    char* msg = (char*)malloc(sizeof(char) * (strlen(content->valuestring) + 1));
    strcpy(msg, content->valuestring);
    session->msgs[session->rounds] = (struct Msg) { msg, "assistant" };
    session->rounds++;

    cJSON_Delete(json);
}

int msg_send(Session* session) {
    Config config = read_config("config.txt");
    if(!config.api_key) return 1;

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

    struct MemoryBuffer mem;
    mem.memory = (char*)malloc(sizeof(char) * 1);
    mem.memory[0] = '\0';
    mem.size = 0;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)(&mem));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);

    // set body
    char* json_body = build_body(session);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(json_body));

    result = curl_easy_perform(curl);
    if(result != CURLE_OK) return 1;

    append_msg(mem.memory, session);

    free(json_body);
    free(mem.memory);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    free_config(config);

    return 0;
}