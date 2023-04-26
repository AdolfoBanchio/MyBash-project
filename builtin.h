#ifndef _BUILTIN_H_
#define _BUILTIN_H_

#include <stdbool.h>

#include "command.h"

#define BUILTIN_AMOUNT 5 /* Cantidad de comandos BI actualmente implementados */
#define MAX_PATH 1024 /* Longitud máxima del path */
/* Constantes para los "nombres" de los comandos BI */
#define CMD_HELP "help"
#define CMD_EXIT "exit"
#define CMD_CD   "cd"
#define CMD_PWD  "pwd"
#define CMD_EE   "miñoneta"
/* Mensajes a imprimir en las funciones builtin */
#define MSG_HELP "MyBash -- Implemented by \033[0;38;5;45m'La Miñoneta'\033[0m\n"\
                 "-> Banchio, Adolfo\n"\
                 "-> Gimenez, Julieta\n"\
                 "-> Viglianco, Agustín\n\n"\
                 "You can use the following internal commands:\n"\
                 "> cd <new_dir>: change current directory.\n"\
                 "> exit: exit the terminal.\n"\
                 "> help: print information about the shell and its usage.\n"\
                 "> pwd: print current working directory.\n"\

#define MSG_CAB "                                     %%%%%%                                     \n"\
                "                          %%%%%%%%%%%%*. ./%%%%%%%%%%%%                         \n"\
                "                     %%%%%%                           %%%%%%                    \n"\
                "                 %%%%#      /%  %% ,%     %%   ,%          %%%%%                \n"\
                "              %%%%     %    %%   %  %     %%   %%     %% %     %%%%             \n"\
                "           %%%%        %%    %%                      %  /   %%    %%%%          \n"\
                "         %%%.     %%    %%    \033[0;38;5;45m*///////////////////*\033[0m   ,%  %%%       (%%%        \n"\
                "       %%%    %%    %%%  \033[0;38;5;45m.//////*      */.      ///////\033[0m   #,  %%      *%%%      \n"\
                "      %%%       *%(   \033[0;38;5;45m/////,           */.           /////*\033[0m  %          %%%     \n"\
                "    %%%   %#        \033[0;38;5;45m////         ,///////.    *          ////\033[0m   /%    %   %%*   \n"\
                "   %%%      %%    \033[0;38;5;45m////       /////,    */.    */////       ////\033[0m  #         %%%  \n"\
                "  %%%         # \033[0;38;5;45m.///      ////         */.    *///////*      ///\033[0m      *%%   %%# \n"\
                "  %%           \033[0;38;5;45m////     ,///           */.    */////////      ///*\033[0m , %%%%%   %% \n"\
                " %%%  /  %/%# \033[0;38;5;45m,///     ///      //.    */.    *///////////     ///\033[0m  %        %%%\n"\
                " %%      #%%  \033[0;38;5;45m///     ///     *///.    */.    *///////////     ////\033[0m     ,%%   %%\n"\
                "%%%   %%%%#  \033[0;38;5;45m.///     //      ////.    */.    *//////////     /////\033[0m  %%%      %%\n"\
                "%%%        % \033[0;38;5;45m////     //               */.                 ////////.\033[0m (        %%\n"\
                "%%%   .,*/   \033[0;38;5;45m.///     //     .////.    */.    */////////*     /////\033[0m     %%    %%\n"\
                " %%        /  \033[0;38;5;45m///     ///     ////.    */.    *///////////     ////\033[0m  %%%(,    %%\n"\
                " %%%  (%%%,   \033[0;38;5;45m*///     ///      //.    */.    *///////////     ///\033[0m   %%(     %%%\n"\
                "  %%         % \033[0;38;5;45m////     //// /////.    */.    */////////.     ////\033[0m %     %   %% \n"\
                "  %%%   %   %%  \033[0;38;5;45m*///      ///////////////.    *////////      ///.\033[0m   %%  %   %%# \n"\
                "   %%%            \033[0;38;5;45m////       ////////////.    */////       ////\033[0m            %%%  \n"\
                "    %%%             \033[0;38;5;45m////         ////////.    */         ////\033[0m             %%*   \n"\
                "      %%%     %%%     \033[0;38;5;45m/////            */.           ./////\033[0m      .%%    %%%     \n"\
                "       %%%*              \033[0;38;5;45m///////       */.      .//////*\033[0m              #%%%      \n"\
                "         %%%(       %%   %    \033[0;38;5;45m/////////////////////\033[0m      %%%        %%%%        \n"\
                "           %%%%     %    #%   %                    %/ %   %  /    %%%(          \n"\
                "              %%%%       %    %% % %% %   % .%    % %% %% (    %%%%             \n"\
                "                 %%%%%     /%/  %/%(  %   %# %    %  .     %%%%%                \n"\
                "                     %%%%%%           ..              %%%%%%                    \n"\
                "                          #%%%%%%%%%%%%%%%%%%%%%%%%%%%                          \n\n"                     


bool builtin_is_internal(scommand cmd);
/*
 * Indica si el comando alojado en `cmd` es un comando interno
 *
 * REQUIRES: cmd != NULL
 *
 */

bool builtin_is_exit(scommand cmd);
/*
 * Indica si el comando alojado en `cmd` es el comando `exit`
 *
 * REQUIRES: cmd != NULL
 *
 */


bool builtin_alone(pipeline p);
/*
 * Indica si el pipeline tiene solo un elemento y si este se corresponde a un
 * comando interno.
 *
 * REQUIRES: p != NULL
 *
 * ENSURES:
 *
 * builtin_alone(p) == pipeline_length(p) == 1 &&
 *                     builtin_is_internal(pipeline_front(p))
 *
 *
 */

void builtin_run(scommand cmd);
/*
 * Ejecuta un comando interno
 *
 * REQUIRES: {builtin_is_internal(cmd)}
 *
 */

#endif

