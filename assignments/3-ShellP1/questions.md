1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  For this program, we want to read the entire line of input that the user gives us, even if they put extra spaces up until they click entire on their keyboard. It reads one line at a time until it hits a newline. It makes sure that it won't exceed the buffer size because it reads in at most one less than size characters from stream and stores them into the buffer pointed to by s. Also a null byte is stored after the last character which means that we automatically get the entire user inputted line.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: We needed to use malloc() instead of allocating a fixed-size array because we do not know how long our user inputted line is going to be. Allocating memory dynamically with malloc() allows us to have different command sizes without having a specific fixed limit. It helps us avoid having stack overflow because if we used a fixed-size array with a max limit and the user inputted something larger, it could cause an overflow. 

3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  We have to trim leading and trailing spaces to make sure that the commands that we extract are parsed correctly. If we parsed without removing the trailing and leading spaces, then when we split the user inputted command, it would be messed up. The command after the split will not be recognized as a valid command if there was a space before or after it. For example, if we had ' cd ', and we didn't trim the spaces, it would not recognize the command as it is supposed to change directory.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: 3 examples of redirection that we should include are: echo "Hello" > output.txt, echo "World" >> output.txt, wc < input.txt. The first symbol '>' redirects stdout to a file and replaces all of that contents file with the new content from the command line. The second symbol is '>>' which redirects stdout to a file and adds the contents that were specified to the end of the file. It is different from the previous one because it does not overwrite, it adds to the end. The third symbol is '<' which redirects stdin to read from a file. The challenges we might have while implementing this is handling the correct edge cases for each of the redirections, parsing the different symbols correctly, and changing the STDIN/STDOUT correct based on the command.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection sends input or output from or to a file. Piping connects the output of one command to the input of another command. Redirection is mainly for communication directly with files, and piping is mainly for connecting commands together.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  It is important to keep STDERR and STDOUT seperate because you need to make sure that the errors and results that you have, and/or are given, are not mixed into eachother. If your program outputs results and an error is one of those results, it shouldn't be combined with the regular program results. When you want to identify issues, it makes it easier by seeing what is specifically wrong if you seperate them.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our custom shell should handle errors from failed commands by seperating and clearly directing STDOUT and STDERR. The shell should capture errors in STDERR whenever the command fails which still making sure that STDOUT continues. This is a valid way because if we want to see the error output vs the normal output, keeping it seperate would be a good strategy. For cases where a command outputs to both, we should provide a way to merge them together. If you want to see all of the outputs together, we could implement this by having a command like 'command > output.txt 2>&1' where command is the executable. In this case, > redirects STDOUT to output.txt, and 2>&1 instructs the shell to also redirect STDERR to the same location as STDOUT.