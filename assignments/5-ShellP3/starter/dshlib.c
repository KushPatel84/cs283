#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"

// trim whitespace
static void removeSpaces(char * str) {
    int start_index = 0;
    int end_index = strlen(str) - 1;

    // Find the first non-space character
    while (str[start_index] != '\0' && isspace((unsigned char) str[start_index])) {
        start_index++;
    }

    // If there's only spaces, reset to an empty string
    if (start_index > end_index) {
        str[0] = '\0';
        return;
    }

    // Find the last non-space character
    while (end_index >= start_index && isspace((unsigned char) str[end_index])) {
        end_index--;
    }

    // get new length
    int new_length = end_index - start_index + 1;

    // Shift trimmed to the front
    for (int i = 0; i < new_length; i++) {
        str[i] = str[start_index + i];
    }

    str[new_length] = '\0';
}

static int parseCommand(char *input, command_t *cmd) {
    removeSpaces(input);

    if (strlen(input) == 0) {
        return WARN_NO_CMDS;
    }

    // Split the command into the executable and arguments
    char *exe_name = strtok(input, " \t");
    if (!exe_name) {
        return WARN_NO_CMDS;
    }

    if (strlen(exe_name) >= EXE_MAX) {
        return ERR_CMD_OR_ARGS_TOO_BIG;
    }
    
    strcpy(cmd->exe, exe_name);

    // get the rest of tokens in argument_buffer
    char argument_buffer[ARG_MAX] = {0};
    int arg_length = 0;

    // extract tokens until none left
    while (true) {
        char *next_arg = strtok(NULL, " \t");
        if (!next_arg) {
            break;
        }

        // if we already have something in argument_buffer, we need a space before the next token
        int arg_length_for_next = strlen(next_arg);
        if (arg_length > 0) {
        
            // Check if the next argument will fit in the buffer
            if (arg_length + arg_length_for_next + 1 >= ARG_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }

            // Add a space before the next argument
            argument_buffer[arg_length] = ' ';
            arg_length++;

        } else {

            // // if argument_buffer is empty, check that next_arg fits
            if (arg_length_for_next >= ARG_MAX) {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
        }

        // Copy the next argument into the buffer
        for (int i = 0; i < arg_length_for_next; i++) {
            argument_buffer[arg_length + i] = next_arg[i];
        }
        arg_length += arg_length_for_next;
    }

    // Null-terminate the argument buffer
    argument_buffer[arg_length] = '\0';
    strcpy(cmd->args, argument_buffer);

    return OK;
}

static int splitAndParseCommands(char *copy, command_list_t *clist)
{
    int num_parsed = 0;

    char *cmd_start = copy;

    while (*cmd_start != '\0')
    {
        if (num_parsed >= CMD_MAX)
        {
            return ERR_TOO_MANY_COMMANDS;
        }

        // Find the next pipe character
        char *idx = cmd_start;
        char *pipe_position = NULL;
        bool inQuotes = false;
        char quoteChar = '\0';

        // Loop through the string until we find a pipe character
        while (*idx != '\0') {

            // If we're not in quotes and we find a quote character, we're now in quotes
            if (!inQuotes && (*idx == '"' || *idx == '\'')) {
                inQuotes = true;
                quoteChar = *idx;

            // If we're in quotes and we find the same quote character, we're no longer in quotes
            } else if (inQuotes && *idx == quoteChar) {
                inQuotes = false;

            // If we're not in quotes and we find a pipe character, we've found the pipe position
            } else if (!inQuotes && *idx == PIPE_CHAR) {
                pipe_position = idx;
                break;
            }
            idx++;
        }

        // If we didn't find a pipe character, set the pipe position to the end of the string
        if (pipe_position == NULL) {
            pipe_position = cmd_start + strlen(cmd_start);
        }

        // If there is no pipe character, set the pipe position to the end of the string
        if (pipe_position == NULL)
        {
            pipe_position = cmd_start + strlen(cmd_start);
        }

        // Save the character at the pipe position and null-terminate the string
        char saved_char = *pipe_position;
        *pipe_position = '\0';

        // Parse the command
        command_t temp;
        int result = parseCommand(cmd_start, &temp);
        *pipe_position = saved_char;

        if (result != OK) {
            return result;
        }

        if (alloc_cmd_buff(&clist->commands[num_parsed]) != OK) {
            return ERR_MEMORY;
        }

        // Combine the executable and arguments into a single string
        char combined[SH_CMD_MAX];
        memset(combined, 0, sizeof(combined));
        strcpy(combined, temp.exe);

        // Add a space if there are arguments
        if (strlen(temp.args) > 0) {
            strcat(combined, " ");
            strcat(combined, temp.args);
        }

        // Copy the combined string to the command buffer
        strcpy(clist->commands[num_parsed]._cmd_buffer, combined);

        // Restore the saved character
        *pipe_position = saved_char;
        if (result != OK)
        {
            return result;
        }

        // Move to the next command
        num_parsed++;
        if (*pipe_position == '\0')
        {
            break;
        }
        cmd_start = pipe_position + 1;
    }

    if (num_parsed == 0)
    {
        return WARN_NO_CMDS;
    }

    // Set the number of commands parsed
    clist->num = num_parsed;
    return OK;
}

int build_cmd_list(char *cmd_line, command_list_t *clist)
{

    // Clear the command list
    memset(clist, 0, sizeof(command_list_t));

    // Copy the command line to a new buffer
    char copy[SH_CMD_MAX];
    memset(copy, 0, sizeof(copy));

    strncpy(copy, cmd_line, SH_CMD_MAX - 1);
    copy[SH_CMD_MAX - 1] = '\0';

    removeSpaces(copy);

    if (strlen(copy) == 0)
    {
        return WARN_NO_CMDS;
    }

    return splitAndParseCommands(copy, clist);
}

int free_cmd_list(command_list_t *cmd_lst)
{
    memset(cmd_lst, 0, sizeof(command_list_t));
    return OK;
}

// allocate memory for the buffer
int alloc_cmd_buff(cmd_buff_t *cmd_buff)
{
    // memory for the command buffer
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);

    // check malloc works
    if (cmd_buff -> _cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    // clear the buffer
    memset(cmd_buff -> _cmd_buffer, 0, SH_CMD_MAX);
    // reset with 0 commands
    cmd_buff->argc = 0;

    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) 
{
    // check if buffer exists and free it
    if (cmd_buff -> _cmd_buffer != NULL) {
        free(cmd_buff->_cmd_buffer);
    }
    // set the pointer to null
    cmd_buff->_cmd_buffer = NULL;

    // reset with 0 commands
    cmd_buff -> argc = 0;

    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff)
{
    // make sure that command buffer has a pointer
    if (cmd_buff == NULL || cmd_buff->_cmd_buffer == NULL) {
        return ERR_MEMORY;
    }
    // clear the cmd buffer
    memset(cmd_buff -> _cmd_buffer, 0, SH_CMD_MAX);

    // reset with 0 commands
    cmd_buff -> argc = 0;

    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff)
{
    // copy the command line and remove the spaces
    clear_cmd_buff(cmd_buff);
    strncpy(cmd_buff -> _cmd_buffer, cmd_line, SH_CMD_MAX - 1);
    cmd_buff->_cmd_buffer[SH_CMD_MAX - 1] = '\0';
    removeSpaces(cmd_buff->_cmd_buffer);

    // check if command is empty
    if (strlen(cmd_buff->_cmd_buffer) == 0) {
        return WARN_NO_CMDS;
    }
    // count the number of tokens
    int i = 0;
    char *indexPtr = cmd_buff->_cmd_buffer;

    // split command string and loop through it
    while (*indexPtr != '\0') {

        // skip spaces in the beginning
        for (; *indexPtr != '\0'; indexPtr++) {
            if (!isspace((unsigned char)*indexPtr))
                break;
        }
        if (*indexPtr == '\0')
            break;

        // different cases for checking if token is quoted or not
        int tokenQuoted = 0;
        switch (*indexPtr) {
            case '"':
            case '\'':
                tokenQuoted = 1;
                break;
            default:
                tokenQuoted = 0;
                break;
        }
        char *tokenStart = indexPtr;
        // for the tokens that are quoted
        if (tokenQuoted) {

            // make sure the token starts after the quotes
            char quoteChar = *indexPtr;
            indexPtr++;
            tokenStart = indexPtr;

            // keep going until you find ending quote
            while (*indexPtr != '\0') {
                if (*indexPtr == quoteChar) {
                    break;
                }
                indexPtr++;
            }

            // if closing quote is found, end the token
            if (*indexPtr != '\0') {
                *indexPtr = '\0';
                indexPtr++;
            }
        } else {
            // for the tokens that are not quoted
            tokenStart = indexPtr;
            for (; *indexPtr != '\0'; indexPtr++) {
                if (isspace((unsigned char)*indexPtr))
                    break;
            }
            // if whitespace is found, end the token
            if (*indexPtr != '\0') {
                *indexPtr = '\0';
                indexPtr++;
            }
        }
        // save that token to the array
        cmd_buff->argv[i] = tokenStart;
        i++;
        if (i >= CMD_ARGV_MAX - 1) {
            break;
        }
    }
    // store the number of tokens
    cmd_buff -> argv[i] = NULL;
    cmd_buff -> argc = i;

    if (i > 0) {
        return OK;
    } else {
        return WARN_NO_CMDS;
    }

}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd)
{
    // need to check if command is cd
    if (strcmp(cmd->argv[0], "cd") == 0) {
        // if no directory, do nothing
        if (cmd->argc <= 1) {
            return BI_EXECUTED;
        }
        // try to change the current directory using the argument
        if (chdir(cmd->argv[1]) != 0) {
        }
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}

int exec_cmd(cmd_buff_t *cmd)
{
    // create new process using fork
    int f_result = fork();

    // if the fork fails
    if (f_result < 0) {
        return ERR_EXEC_CMD;
    }
    // execute using execvp
    if (f_result == 0) {
        execvp(cmd -> argv[0], cmd -> argv);
        exit(ERR_EXEC_CMD);
    } else {
        // wait
        int status;
        waitpid(f_result, &status, 0);
    }
    return OK;
}

int execute_pipeline(command_list_t *clist)
{
    // if only one command, execute it
    if (clist->num == 1) {
        cmd_buff_t single_cmd;
        alloc_cmd_buff(&single_cmd);

        // convert command to single cmd_buff
        char line[SH_CMD_MAX];
        memset(line, 0, sizeof(line));
        strcpy(line, clist->commands[0]._cmd_buffer);

        // build the command buffer
        build_cmd_buff(line, &single_cmd);

        // check if it is a built-in command
        if (exec_built_in_cmd(&single_cmd) != BI_NOT_BI) {
            free_cmd_buff(&single_cmd);
            return OK;
        }

        // execute the command and free the buffer
        exec_cmd(&single_cmd);
        free_cmd_buff(&single_cmd);
        return OK;
    }

    // if more than one command
    pid_t pids[CMD_MAX];
    int cmds = clist->num;

    // create pipes
    int numPipes = cmds - 1;
    int pipefds[CMD_MAX - 1][2];
    int j = 0;

    // need while to pipe because we need to create multiple pipes
    while (j < numPipes) {
        if (pipe(pipefds[j]) < 0) {
            return ERR_EXEC_CMD;
        }
        j++;
    }

    // loop through all commands
    for (int i = 0; i < cmds; i++) {
        
        // if first command, only write to pipe
        cmd_buff_t child_cmd;
        alloc_cmd_buff(&child_cmd);

        // convert command to single cmd_buff
        char line[SH_CMD_MAX];
        memset(line, 0, sizeof(line));

        // copy the command to the buffer
        strcpy(line, clist->commands[i]._cmd_buffer);
        build_cmd_buff(line, &child_cmd);

        // fork a new process for each command
        pid_t child = fork();
        if (child == 0) {

            // For non-first commands, duplicate the read of the previous pipe to STDIN
            if (i > 0) {
                dup2(pipefds[i-1][0], STDIN_FILENO);
            }

            // For non-last commands, duplicate the write of the current pipe to STDOUT
            if (i < cmds - 1) {
                dup2(pipefds[i][1], STDOUT_FILENO);
            }

            // close all pipe file descriptors, no longer needed
            int pipeIdx = 0;
            while (pipeIdx < cmds - 1) {

                // close the read end of the pipe
                close(pipefds[pipeIdx][0]);
                close(pipefds[pipeIdx][1]);
                pipeIdx++;
            }

            // execute the command
            execvp(child_cmd.argv[0], child_cmd.argv);
            exit(ERR_EXEC_CMD);
        } else {

            // save the child pid for waiting
            pids[i] = child;

            // Free the command buffer for the child command
            free_cmd_buff(&child_cmd);
        }
    }

    // parent closes pipes
    for (int i = 0; i < cmds - 1; i++) {
        close(pipefds[i][0]);
        close(pipefds[i][1]);
    }
    // wait for all
    for (int i = 0; i < cmds; i++) {
        waitpid(pids[i], NULL, 0);
    }
    return OK;
}
/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
 int exec_local_cmd_loop() {
    char *cmd_buff = (char *)malloc(SH_CMD_MAX);
    command_list_t clist;
    int lastRC = OK;

    while (1) {
        printf("%s", SH_PROMPT);
        if (!fgets(cmd_buff, SH_CMD_MAX, stdin)) {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            break;
        }

        // clear the command list
        memset(&clist, 0, sizeof(clist));
        int rc = build_cmd_list(cmd_buff, &clist);
        if (rc == OK) {

            // If build succeeded, run pipeline
            lastRC = execute_pipeline(&clist);
            free_cmd_list(&clist);
        } else if (rc == WARN_NO_CMDS) {

            // print the "no commands" warning
            printf(CMD_WARN_NO_CMD);
            lastRC = WARN_NO_CMDS;
        } else if (rc == ERR_TOO_MANY_COMMANDS) {

            // print pipe limit error
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            lastRC = ERR_TOO_MANY_COMMANDS;
        }
    }
    return lastRC;
}