#include "msg.h"
#include "user.h"
#include "session.h"
#include "util.h"

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
    cJSON* json = build_session(session);

    char* body = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    return body;
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

// return non-zero on failing
// TODO: make it functional. also move msg parsing to session.c / session.h
static int append_msg(char* contents, Session* session) {
    char* end;
    cJSON* json = cJSON_ParseWithOpts(contents, &end, 1);
    if(!json || end != contents + strlen(contents)) {
        cJSON_Delete(json);
        return 1;
    }

    // find choices[0].message.content
    // find choices[0].message.tool_calls
    const cJSON* content = find_key(json, "choices");
    content = find_index(content, 0);
    content = find_key(content, "message");
    const cJSON* tool_calls_json = find_key(content, "tool_calls");
    content = find_key(content, "content");

    // parse content
    char* msg = NULL;
    if(content && !cJSON_IsNull(content)) msg = copy_string(content->valuestring);
    session->msgs[session->rounds] = (Msg) {
        .role = "assistant",
        .msg = msg,
    };

    // parse tool call
    if(tool_calls_json) {
        ToolCall* tool_calls;
        size_t tool_call_num = parse_tool_calls(tool_calls_json, &tool_calls);
        session->msgs[session->rounds].tool_call_num = tool_call_num;
        session->msgs[session->rounds].tool_calls = tool_calls;
    }

    // parse token usage stat
    cJSON* usage = find_key(json, "usage");
    if(usage) {
        session->token_stat = parse_token_stat(usage);
    }

    session->rounds++;
    cJSON_Delete(json);

    return 0;
}

// TODO: only return the response, leave appending to the caller
int msg_send(Session* session, const Config* config) {
    int return_value = 0;

    if(!config->api_key) return 1;

    CURL* curl;
    CURLcode result;
    curl = curl_easy_init();
    if(!curl) return 1;

    char url[512];
    strcpy(url, config->base_url);
    strcat(url, "v1/chat/completions");

    char api_key_header[512 + 256];
    strcpy(api_key_header, "Authorization: Bearer ");
    strcat(api_key_header, config->api_key);

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

    // handle possible redirections
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);

    // set timeout
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20L);

    // set body
    char* json_body = build_body(session);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(json_body));

    result = curl_easy_perform(curl);
    if(result != CURLE_OK) {
        return_value = 1;
        goto end;
    }

    long status_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    // TODO: better error handling
    if(status_code != 200) {
        return_value = 1;
        goto end;
    }

    if(append_msg(mem.memory, session))
        return_value = 1;

end:
    free(json_body);
    free(mem.memory);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return return_value;
}