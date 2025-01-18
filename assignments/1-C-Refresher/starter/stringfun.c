#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BUFFER_SZ 50

//prototypes
void usage(char * );
void print_buff(char * , int);
int setup_buff(char * , char * , int);
int count_words(char * , int, int);
int reverse_buff(char * , int);
int word_print(char * , int);

int setup_buff(char * buff, char * user_str, int len)
{
    char * end_of_buffer = buff + len;
    char * ptr_buff = buff;
    bool last_char_is_space = true;

    while ( * user_str != '\0') // process the entire user input string
    {
        if (ptr_buff >= end_of_buffer)
        {
            return -1;
        }

        if ( * user_str == ' ' || * user_str == '\t') // handle whitespace characters
        {
            if (!last_char_is_space) // add a single space if multiple spaces/tabs
            {
                * ptr_buff = ' ';
                ptr_buff++;
                last_char_is_space = true;
            }
        } else
        {
            * ptr_buff = * user_str; // copy non-whitespace characters
            ptr_buff++;
            last_char_is_space = false;
        }
        user_str++;
    }

    if (ptr_buff > buff && * (ptr_buff - 1) == ' ') // remove trailing whitespace
    {
        ptr_buff--;
    }

    while (ptr_buff < end_of_buffer) // fill buffer with '.'
    {
        * ptr_buff = '.';
        ptr_buff++;
    }
    int characters_copied = ptr_buff - buff;
    return characters_copied;
}

void print_buff(char * buff, int len)
{
    printf("Buffer:  ");
    for (int i = 0; i < len; i++)
    {
        putchar( * (buff + i));
    }
    putchar('\n');
}

void usage(char * exename)
{
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

int count_words(char * buff, int len, int user_str_len)
{
    if (user_str_len > len)
    {
        return -1;
    }
    if (user_str_len == 0)
    {
        return 0;
    }

    int word_count = 0;
    bool middle_of_word = false;
    char * end_of_buffer = buff + len;
    char * ptr_buff = buff;

    while (ptr_buff < end_of_buffer)
    {
        if ( * ptr_buff != ' ' && * ptr_buff != '.') // checks for word characters
        {
            if (!middle_of_word)
            {
                word_count++; // increase word count at word start
                middle_of_word = true;
            }
        } else
        {
            middle_of_word = false; // reset for whitespace
        }
        ptr_buff++;
    }

    return word_count;
}

int reverse_buff(char * buff, int len)
{

    if (len <= 0 || !buff)
    {
        return -1;
    }

    char * start_of_buff = buff;
    char len_of_buff = len - 1;
    char * end_of_str = buff + len_of_buff;

    while (end_of_str > buff && * end_of_str == '.') // find last needed character
    {
        end_of_str--;
    }

    while (start_of_buff < end_of_str) // reverse characters
    {
        char temp = * start_of_buff;
        * start_of_buff = * end_of_str;
        * end_of_str = temp;
        start_of_buff++;
        end_of_str--;
    }

    printf("Reversed String: ");
    for (int i = 0; i < len; i++)
    {
        if (buff[i] == '.') {
            break;
        }
        putchar(buff[i]);
    }
    putchar('\n');

    return 0;
}

int word_print(char * buff, int len)
{
    if (len <= 0 || !buff)
    {
        return -1;
    }

    char * end_of_buffer = buff + len;
    char * ptr_buff = buff;
    int word_count = 0;

    printf("Word Print\n");
    printf("----------\n");

    while (ptr_buff < end_of_buffer)
    {
        if (* ptr_buff != ' ' && * ptr_buff != '.') // find start of a word
        {
            word_count++;
            int char_count = 0;
            printf("%d. ", word_count);

            while (ptr_buff < end_of_buffer && * ptr_buff != ' ' && * ptr_buff != '.') // print word characters condition
            {
                putchar( * ptr_buff);
                ptr_buff++;
                char_count++;
            }
            printf("(%d)\n", char_count);  // print word length for each word
        } else
        {
            ptr_buff++;
        }
    }

    printf("\nNumber of words returned: %d\n", word_count);
    return 0;
}

int main(int argc, char * argv[])
{

    char * buff; //placeholder for the internal buffer
    char * input_string; //holds the string provided by the user on cmd line
    char opt; //used to capture user option from cmd line
    int rc; //used for return codes
    int user_str_len; //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /* The if statement condition argc < 2 is needed because the first part of it
    makes sure that the user in the command line has given at least 2 arguments.
    Then the second part conditional after the or part of the statement checks
    that the 2nd argument starts with a dash(-) as required. If argv[1] does not exist
    then the user has not provided an option so then the program cannot continue. It 
    displays the usage and then exits. */
    if ((argc < 2) || ( * argv[1] != '-'))
    {
        usage(argv[0]);
        exit(1);
    }

    opt = (char) * (argv[1] + 1); //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h')
    {
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    /* The if statement conditional argc < 3 is needed because it
    makes sure that user in the command line has given at least 3 arguments.
    It does not check if the 3 arguments are the program name
    which is stringfun, an operation, and a inputted string. It just checks
    if the user has input at least 3 arguments so that the program can continue. If 
    there are less than 3 arguments then the program cannot continue so it shows the usage 
    and then exits. */
    if (argc < 3)
    {
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = malloc(BUFFER_SZ * sizeof(char));
    if (!buff)
    {
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
    if (user_str_len < 0)
    {
        printf("Error setting up buffer, error = %d\n", user_str_len);
        exit(2);
    }

    switch (opt) {
    case 'c':
        rc = count_words(buff, BUFFER_SZ, user_str_len);
        if (rc < 0)
        {
            printf("Error counting words, rc = %d", rc);
            exit(2);
        }
        printf("Word Count: %d\n", rc);
        break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
    case 'r':
        rc = reverse_buff(buff, user_str_len);
        if (rc < 0)
        {
            printf("Error reversing string, rc = %d", rc);
            exit(2);
        }
        break;
    case 'w':
        rc = word_print(buff, user_str_len);
        if (rc < 0)
        {
            printf("Error printing words, rc = %d", rc);
            exit(2);
        }
        break;
    case 'x':
        if (argc < 5)
        {
            printf("Error not enough arguments.\n");
            exit(1);
        } else {
            printf("Not Implemented!\n");
            exit(3);
        }
    default:
        usage(argv[0]);
        exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff, BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that
//          the buff variable will have exactly 50 bytes?
// 
//          PLACE YOUR ANSWER HERE
            /* Giving both the buffer pointer and its length is good practice 
            even if we know the buff will have 50 bytes because it helps
            the extra helper functions work safely. Even though the buffer is always 50 bytes
            in our case, the functions should work with different buffer sizes. It also stops the 
            function from going out of bounds by mistake. The length prevents buffer overflows and
            makes sure that the functions don't write over the amount of memory we gave it.
            The code is safer and easier to manage if you give it both the pointer and the
            length. */