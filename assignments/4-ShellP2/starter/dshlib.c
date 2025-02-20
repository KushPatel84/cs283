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

Built_In_Cmds match_command(const char *input)
{
    // check if the command is cd and then return the built in command is returned
    if (strcmp(input, "cd") == 0) {
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
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

void process_command(char *cmd_buff, cmd_buff_t *cmd) {

    // command buffer and parse it
    int split = build_cmd_buff(cmd_buff, cmd);
    
    // check if the parsing worked
    bool has_arguments = (cmd->argc >= 1);
    bool is_parse_successful = (split == OK);

    // check if the build-in command is built-in and then execute it
    if (is_parse_successful && has_arguments) {
        if (exec_built_in_cmd(cmd) != BI_NOT_BI) {
            return;
        } 
        exec_cmd(cmd);
    } 
    clear_cmd_buff(cmd); 
}

 int exec_local_cmd_loop() {
    char *cmd_buff = (char *)malloc(SH_CMD_MAX);
    cmd_buff_t cmd;
 
    // acllocate memory for cmd buffer
    if (alloc_cmd_buff(&cmd) != OK) {
        return ERR_MEMORY;
    }
 
    while (1)
    {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL)
        {
            printf("\n");
            break;
        }
        // remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        if (strcmp(cmd_buff, EXIT_CMD) == 0)
        {
            break;
        }
        // process the command
        process_command(cmd_buff, &cmd);
    }
    // free memory
    free_cmd_buff(&cmd);
    return OK;
 }