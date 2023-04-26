#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "tests/syscall_mock.h"
#include "builtin.h"
#include "command.h"
#include "execute.h"
#include "strextra.h"


/*
 * Almacena en un arreglo los parámetros de un scommand.
 * Devuelve nueva memoria, que queda a cargo del llamador.
 */
static char **set_args(scommand cmd, unsigned int length) {
    char **args = calloc(length,sizeof(char *));
    args[0] = strdup(scommand_front(cmd));
    scommand_pop_front(cmd);
    for (unsigned int i = 1; i < length-1;i++) {
        args[i] = strdup(scommand_front(cmd));
        scommand_pop_front(cmd);
    }
    return args;
}


/*
 * Libera la memoria dinámica de un arreglo de argumentos de un determinado comando.
 */
static char **free_args(char **args, unsigned int length) {
    for (unsigned int i = 0; i < length; i++) {
        free(args[i]);
        args[i] = NULL;
    }
    free(args);
    args = NULL;
    return args;
}


/*
 * Redirecciona la entrada de un proceso.
 */
static void setup_redir_in(scommand cmd) {
    int in_fd = open(scommand_get_redir_in(cmd), O_RDONLY, 0);
    if (in_fd < 0) {
        fprintf(stderr, "Open failed: file or directory not found.");
    }
    dup2(in_fd, STDIN_FILENO);
    close(in_fd);
}


/*
 * Redirecciona la salida de un proceso
 */
static void setup_redir_out(scommand cmd) {
    int out_fd = open(scommand_get_redir_out(cmd), 
                  O_CREAT | /* Crea el archivo si no existe */
                  O_WRONLY | /* Permite solo lectura */
                  O_TRUNC, /* Si existe, sobreescribe */
                  S_IRWXU /* Permiso de edición y ejecución */
                  | 0);
    if (out_fd < 0) {
        fprintf(stderr, "Open failed: file or directory not found.");
    }
    dup2(out_fd, STDOUT_FILENO);
    close(out_fd);
}


/*
 * Ejecuta un comando simple, realizando las redirecciones necesarias y
 * las llamadas a execvp.
 */
static void execute_scommand(scommand cmd) {
        unsigned int init_length = scommand_length(cmd)+1;
        char **args = set_args(cmd, init_length);
        if (scommand_get_redir_in(cmd)!=NULL) {
            setup_redir_in(cmd);
        }
        if (scommand_get_redir_out(cmd)!= NULL) {
            setup_redir_out(cmd);
        }
        if (execvp(args[0],args)<0) {
            printf("Invalid command.\n"); 
        }
        /* Salimos del proceso hijo y liberamos la memoria */ 
        args = free_args(args, init_length);
        exit(EXIT_FAILURE);
    }


/*
 * Ejecuta un pipeline de dos o más comandos.
 */
static void execute_long_pipeline(pipeline apipe){
    scommand cmd;
    unsigned int pipe_len = pipeline_length(apipe);
    int *pids = calloc(pipe_len, sizeof(int));
    int (*pipefds)[2]; 
    pipefds = calloc(pipe_len-1, sizeof(int));
    for (unsigned int i = 0; i < pipe_len; i++) {
        if (i == 0) { /* Primer comando */
            pipe(pipefds[i]);
            cmd = pipeline_front(apipe);
            pids[i] = fork();
            if (pids[i] < 0) {
                fprintf(stderr,"Fork failed.\n");
            } else if (pids[i] == 0) {
                close(pipefds[i][0]);
                dup2(pipefds[i][1], STDOUT_FILENO);
                close(pipefds[i][1]);
                execute_scommand(cmd);
            }
        } else if (i == pipe_len-1) { /* Último comando */
            cmd = pipeline_front(apipe);
            pids[i] = fork();
            if (pids[i] < 0) {
                fprintf(stderr,"Fork failed.\n");
            } else if (pids[i] == 0) {
                close(pipefds[i-1][1]);
                dup2(pipefds[i-1][0], STDIN_FILENO);
                close(pipefds[i-1][0]);
                execute_scommand(cmd);
            }
        } else { /* Comandos intermedios */
            pipe(pipefds[i]);
            cmd = pipeline_front(apipe);
            pids[i] = fork();
            if (pids[i] < 0) {
                fprintf(stderr,"Fork failed.\n");
            } else if (pids[i] == 0) {
                close(pipefds[i-1][0]);
                dup2(pipefds[i-1][1], STDOUT_FILENO);
                close(pipefds[i-1][1]);
                close(pipefds[i][1]);
                dup2(pipefds[i][0], STDIN_FILENO);
                close(pipefds[i][0]);
                execute_scommand(cmd);
            }
        }
        pipeline_pop_front(apipe);
    }
    for (unsigned int j = 0; j < pipe_len-1; j++) {
        close(pipefds[j][0]);
        close(pipefds[j][1]);
    }
    for (unsigned int k = 0; k < pipe_len; k++) {
        waitpid(pids[k], NULL, 0);
    }
    free(pipefds);
    free(pids);
}


/*
 * Ejecuta un pipeline, realizando los fork necesarios y configurando
 * toda la estructura para el pipe entre dos o más comandos.
 */
void execute_pipeline(pipeline apipe) {
    assert(apipe != NULL);
    scommand cmd;
    if (builtin_alone(apipe)) {
        builtin_run(pipeline_front(apipe));
    } else {
        if (pipeline_length(apipe)==1) {
            /* Comandos simples */
            cmd = pipeline_front(apipe);
            pid_t pid = fork();
            if (pid < 0) {
                fprintf(stderr,"Fork failed.\n");
            } else if (pid == 0) {/* child */
                execute_scommand(cmd);
            }
            if (pipeline_get_wait(apipe)) {
                /* parent */      
                waitpid(pid,NULL,0);

            }
        } else if (pipeline_length(apipe) >= 2) {
            execute_long_pipeline(apipe);
        } 
    }
}

