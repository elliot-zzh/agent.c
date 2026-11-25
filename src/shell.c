#include "shell.h"
#include <stdio.h>
#include <string.h>
#include "util.h"

#ifdef __APPLE__
#define DEFAULT_SHELL "/bin/zsh"
#endif
#ifdef __linux__
#define DEFAULT_SHELL "/bin/bash"
#endif

#if defined(__APPLE__) || defined(__linux__)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int run_shell(const char* command, char** res, size_t* res_len) {
    // TODO: support timeout
    int fds[2];
    if(pipe(fds) == -1) return -1;

    pid_t pid = fork();
    if(pid == -1) return -1;

    if(pid == 0) { // child process
        close(fds[0]);
        dup2(fds[1], STDOUT_FILENO);
        dup2(fds[1], STDERR_FILENO);
        close(fds[1]);
        execl(DEFAULT_SHELL, DEFAULT_SHELL, "-c", command, (char*)NULL);
        _exit(127);
    }

    // main process read output from cmd
    close(fds[1]);
    const size_t block = 4096;
    // size is buffer's size and len is the size of read data
    size_t size = block, len = 0;
    char* buf = malloc(size);
    ssize_t n;
    while((n = read(fds[0], buf + len, size - len - 1)) > 0) {
        len += n;
        if(len + 1 >= size) buf = realloc(buf, size += block);
    }
    buf[len] = '\0';
    close(fds[0]);

    int status;
    waitpid(pid, &status, 0);
    *res = buf;
    *res_len = len;
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

int shell_tool_call(const cJSON* json, char** content) {
    const cJSON* command = find_key(json, "command");
    if(!command || !cJSON_IsString(command)) {
        *content = "error: wrong type of argument `command`. string expected";
        return -1;
    }
    // TODO: process timeout arg
    // TODO: handle unexpected arguments
    char* out = NULL;
    size_t out_len;
    int rc = run_shell(command->valuestring, &out, &out_len);
    if(rc < 0) {
        free(out);
        *content = copy_string("error: command failed to run or was killed by a signal");
        return rc;
    }
    char head[32];
    int n = snprintf(head, sizeof(head), "\nexit code: %d\n", rc);
    *content = (char*)malloc(sizeof(char) * (out_len + n + 1));
    memcpy(*content, out, out_len);
    memcpy(*content + out_len, head, n + 1);
    free(out);
    return rc;
}

#endif