#ifndef TERM_H
#define TERM_H

// print introduction and user guide
void greetings();

// implements a REPL
// use '>' to hint user input
// input /new to switch to a brand-new session
// input /quit or /exit to exit the app
// otherwise feed user input to the llm
int term_loop(int argc, char** argv);

#endif