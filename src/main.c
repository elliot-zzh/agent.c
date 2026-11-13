#include "agent.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    const char *name = (argc > 1) ? argv[1] : "world";
    char greeting[256];

    int length = agent_greet(name, greeting, sizeof greeting);
    if (length < 0) {
        fputs("agent: could not build greeting\n", stderr);
        return 1;
    }
    if ((size_t)length >= sizeof greeting) {
        fputs("agent: name is too long\n", stderr);
        return 1;
    }

    printf("agent %s: %s\n", agent_version(), greeting);
    return 0;
}
