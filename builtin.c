#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "tests/syscall_mock.h"

#include "builtin.h"
#include "command.h"


/*
 * Indica si el comando alojado en `cmd` es un comando interno.
 */
bool builtin_is_internal(scommand cmd) {
    assert(cmd != NULL);
    /* Guargamos los nombres de los comandos implementados en un arreglo. */
    const char *list_of_bi_cmds[] = {CMD_CD, CMD_HELP, CMD_PWD, CMD_EXIT, CMD_EE};
    bool is_internal = false;
    char *cmd_name = scommand_front(cmd);
    for (unsigned int i = 0; i < BUILTIN_AMOUNT && !is_internal; ++i) {
        if (!strcmp(list_of_bi_cmds[i], cmd_name)) {
            is_internal = true;
        }
    }
    return is_internal;
}


/*
 * Indica si el comando alojado en `cmd` es el comando 'exit'.
 */
bool builtin_is_exit(scommand cmd) {
    assert(cmd != NULL);
    char *cmd_name = scommand_front(cmd);
    bool is_exit = !strcmp(CMD_EXIT, cmd_name);
    return is_exit;
}


/*
 * Indica si el pipeline tiene un solo elemento y si este corresponde
 * a un comando interno.
 */
bool builtin_alone(pipeline p) {
    assert(p != NULL);
    return pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p));
}


/*
 * Cambia el directorio en el que trabaja el proceso de la terminal.
 */
static void change_dir(char *dir) {
    if (dir == NULL) {
        /* Caso 'cd' sin argumentos -> cambia a HOME */
        char *home = getenv("HOME");
        chdir(home);
    } else {
        chdir(dir);
    }
}


/*
 * Imprime un mensaje con información sobre la terminal.
 */
static void print_help(void) {
    const char *msg = MSG_HELP;
    printf("%s\n", msg);
    fflush(stdout);
}


/*
 * Easter Egg
 */
static void print_cab(void) {
    const char *msg = MSG_CAB;
    printf("%s\n", msg);
    fflush(stdout);
}


/*
 * Imprime el directorio en el que trabaja la terminal.
 */
static void print_working_dir(void) {
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        printf("Couldn't find current working directory.\n");
    }
    fflush(stdout);
}


/*
 * Ejecuta un comando interno.
 */
void builtin_run(scommand cmd) {
    assert(builtin_is_internal(cmd));
    
    char *command = strdup(scommand_front(cmd));
    scommand_pop_front(cmd);

    /* Ejecutamos la función correspondiente al comando especificado */
    if (!strcmp(command, CMD_CD)) {
        if (scommand_is_empty(cmd)) {
            /* Caso 'cd' sin argumentos */
            change_dir(NULL);
        } else {
            char *dir = strdup(scommand_front(cmd));
            scommand_pop_front(cmd);
            change_dir(dir);
            free(dir);
            dir = NULL;
        }
    } else if (!strcmp(command, CMD_HELP)) {
        print_help();
    } else if (!strcmp(command, CMD_EE)) {
        print_cab();
    } else if (!strcmp(command, CMD_PWD)) {
        print_working_dir();
    }
    free(command);
    command = NULL;
}