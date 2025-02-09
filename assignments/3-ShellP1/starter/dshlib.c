#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
static void removeSpaces(char *str)
{
    int start_index = 0;
    int end_index = strlen(str) - 1;

    // Find the first non-space character
    while (str[start_index] != '\0' && isspace((unsigned char)str[start_index]))
    {
        start_index++;
    }

    // If there's only spaces, reset to an empty string
    if (start_index > end_index) {
        str[0] = '\0';
        return;
    }

    // Find the last non-space character
    while (end_index >= start_index && isspace((unsigned char)str[end_index]))
    {
        end_index--;
    }

    // get new length
    int new_length = end_index - start_index + 1;

    // Shift trimmed to the front
    for (int i = 0; i < new_length; i++)
    {
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
        char *pipe_position = strchr(cmd_start, '|');

        // If there is no pipe character, set the pipe position to the end of the string
        if (pipe_position == NULL)
        {
            pipe_position = cmd_start + strlen(cmd_start);
        }

        // Save the character at the pipe position and null-terminate the string
        char saved_char = *pipe_position;
        *pipe_position = '\0';

        int result = parseCommand(cmd_start, &clist->commands[num_parsed]);

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