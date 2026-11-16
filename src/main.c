#include "term.h"
#include "msg.h"
#include "user.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    int err = msg_init();
    if(err != 0) return err;

    greetings();

    err = term_loop(argc, argv);

    msg_close();

    return err;
}
