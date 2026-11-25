#include "term.h"
#include "msg.h"
#include "user.h"
#include "tool.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    int err = msg_init();
    if(err != 0) return err;

    tool_list_init();

    greetings();

    err = term_loop(argc, argv);

    msg_close();

    tool_list_close();

    return err;
}
