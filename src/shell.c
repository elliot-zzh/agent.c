#ifdef __APPLE__
#define DEFAULT_SHELL "/bin/zsh"
// _POSIX_C_SOURCE is required to pass compilation under strict c11
#define _POSIX_C_SOURCE 200809L
#endif
#ifdef __linux__
#define DEFAULT_SHELL "/bin/bash"
#define _POSIX_C_SOURCE 200809L
#endif

#include "shell.h"
#include "util.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(__APPLE__) || defined(__linux__)

#include <poll.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int run_shell(const char* command, char** res, size_t* res_len, int timeout_ms) {
    int fds[2];
    if(pipe(fds) == -1) return -1;

    pid_t pid = fork();
    if(pid == -1) return -1;

    if(pid == 0) { // child process
        setpgid(0, 0);
        close(fds[0]);
        dup2(fds[1], STDOUT_FILENO);
        dup2(fds[1], STDERR_FILENO);
        close(fds[1]);
        execl(DEFAULT_SHELL, DEFAULT_SHELL, "-c", command, (char*)NULL);
        _exit(127);
    }

    setpgid(pid, pid);
    // main process read output from cmd
    close(fds[1]);
    const size_t block = 4096;
    // size is buffer's size and len is the size of read data
    size_t size = block, len = 0;
    char* buf = malloc(size);
    ssize_t n;
    struct timespec start;
    if(timeout_ms > 0) clock_gettime(CLOCK_MONOTONIC, &start);
    int timedout = 0;
    for(;;) {
        if(timeout_ms > 0) {
            struct timespec now;
            clock_gettime(CLOCK_MONOTONIC, &now);
            double left = timeout_ms / 1000.0 - now.tv_sec + start.tv_sec - (now.tv_nsec - start.tv_nsec) / 1e9;
            if(left <= 0) {
                timedout = 1;
                break;
            }
            struct pollfd p = (struct pollfd) { .fd = fds[0], .events = POLLIN };
            int poll_status = poll(&p, 1, (int)(left * 1000) + 1);
            if(poll_status == 0) {
                timedout = 1;
                break;
            }
            if(poll_status < 0) {
                if(errno == EINTR) continue;
                break;
            }
        }
        n = read(fds[0], buf + len, size - len - 1);
        if(n <= 0) break;
        len += n;
        if(len + 1 >= size) buf = realloc(buf, size += block);
    }
    buf[len] = '\0';
    close(fds[0]);

    if(timedout) kill(-pid, SIGKILL);

    int status;
    waitpid(pid, &status, 0);
    *res = buf;
    *res_len = len;
    
    if(timedout) return -2;
    if(WIFEXITED(status)) return WEXITSTATUS(status);
    return -1;
}

#endif

#ifdef _WIN32

// TODO: implement shell runner for Windows
int run_shell(const char* command, char** res, size_t* res_len, int timeout_ms) {

}

#endif

#define ERR_SHELL_TIMEOUT ("\ntime out\n")

int shell_tool_call(const cJSON* json, char** content) {
    const cJSON* command = find_key(json, "command");
    if(!command || !cJSON_IsString(command)) {
        *content = copy_string("error: wrong type of argument `command`. string expected");
        return -1;
    }
    const cJSON* timeout = find_key(json, "timeout");
    int timeout_ms = timeout ? timeout->valueint : -1;
    // TODO: handle unexpected arguments
    char* out = NULL;
    size_t out_len;
    int rc = run_shell(command->valuestring, &out, &out_len, timeout_ms);
    if(rc == -1) {
        free(out);
        *content = copy_string("error: command failed to run or was killed by a signal");
        return rc;
    } else if(rc == -2) { // timeout
        size_t err_len = strlen(ERR_SHELL_TIMEOUT);
        *content = (char*)malloc(sizeof(char) * (out_len + err_len + 1));
        memcpy(*content, out, out_len);
        memcpy(*content + out_len, ERR_SHELL_TIMEOUT, err_len + 1);
    } else {
        char head[32];
        int n = snprintf(head, sizeof(head), "\nexit code: %d\n", rc);
        *content = (char*)malloc(sizeof(char) * (out_len + n + 1));
        memcpy(*content, out, out_len);
        memcpy(*content + out_len, head, n + 1);
    }
    free(out);
    return rc;
}
