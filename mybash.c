#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin.h"
#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "prompt.h"

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;
    bool quit = false;

    input = parser_new(stdin);
    while (!quit) {
        show_prompt();
        while(waitpid(-1, NULL, WNOHANG) > 0);
        pipe = parse_pipeline(input);
        quit = parser_at_eof(input);
        if (pipe != NULL) {
            quit = builtin_is_exit(pipeline_front(pipe));
            execute_pipeline(pipe);
            pipeline_destroy(pipe);
        }
    }
    parser_destroy(input); 
    input = NULL;
    return EXIT_SUCCESS;
}

