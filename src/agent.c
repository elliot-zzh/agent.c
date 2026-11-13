#include "agent.h"

#include <stdio.h>

const char *agent_version(void)
{
    return AGENT_VERSION_STRING;
}

int agent_greet(const char *name, char *dst, size_t dst_size)
{
    if (name == NULL || dst == NULL || dst_size == 0) {
        return -1;
    }

    return snprintf(dst, dst_size, "Hello, %s!", name);
}
