/* Private header: shared only between this application's translation units. */
#ifndef AGENT_AGENT_H
#define AGENT_AGENT_H

#include <stddef.h>

/* Keep in sync with project(VERSION ...) in CMakeLists.txt. */
#define AGENT_VERSION_STRING "0.1.0"

/* Returns the application version string. The pointer has static lifetime. */
const char *agent_version(void);

/*
 * Writes "Hello, <name>!" into dst, always NUL-terminating when dst_size > 0.
 * Behaves like snprintf: returns the length the greeting would have had, or a
 * negative value if name or dst is NULL or dst_size is 0. A return value
 * >= (int)dst_size means the greeting was truncated.
 */
int agent_greet(const char *name, char *dst, size_t dst_size);

#endif /* AGENT_AGENT_H */
