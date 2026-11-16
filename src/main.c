#include "term.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    greetings();

    return term_loop(argc, argv);
}
