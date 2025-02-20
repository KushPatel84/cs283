1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  We used fork/execvp instead of just calling execvp directly because when a process calls fork(), the operating system creates a new child process that is an exact copy of the parent, inheriting its memory, file descriptors, and execution state. It allows the shell to handle multiple commands, specifically new commands while the forked child processes the first command. execvp() is used within the forked child process which allows the shell to keep running, then it is called to replace and then run the command. 

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**: If fork() system call fails then it means that the operating system could not create a new child process. In my implementation, I handle this scenario by checking if the f_result which is equal to the fork() in my code, is less than 0, if it is then return the error code ERR_EXEC_CMD, which is -6. This is needed because then we should not be able to call execvp() if the child process was not created if fork() didn't run properly.


3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: execvp() will search the `PATH` system environment variable locations for binaries. It searched for the command in each of the directories, it goes through each directory in PATH until it finds the correct executable and then execvp() executes the command if it was found, if it wasn't then we have to handle that error as well.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**: The purpose of calling wait() in the parent process after forking is because we have to make the shell wait for the forked child process to finish executing before the program can continue. If we want to get the child's exit status after it is finished executing, we can get it with `WEXITSTATUS` to extract the status code from the child process. If we didn't call wait then the parent process would continue executing before the child process finished executing resulting to issues such as not being able to get the child's exit status and maybe commands not executing in the correct order which results in incorrect output.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**: `WEXITSTATUS` is used to extract the status code from the child process. This exit code is the value that the child process returns after it is done executing. We can use it to determine if the child process executed correctly or not. We need it to display errors if the child process did not finish executing or something was wrong. Without it, we cannot see whether or not the child process executed correctly.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  I use a loop that skips any whitespace in the beginning, then I use a switch statement to check if the character is a quote, it checks if it is a double quote and then sets a flag for it. If it is quoted then I skip the opening quote and then set the token start pointer to the next character, then I loop until I get the matching quote that closes it. Then after the closing quote is found, I terminate the token with a null terminator. This is necessary because I need to preserve spaces inside the quoted string for the output to match what the normal terminal command does.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**: In my previous assignment I split the parsing into 2 different functions, one was used for splitting the executable name and arguments, the other was used for splitting an entire command line by pipes and then processing each command by itself. In this assignment there were no pipes so I combined all of my parsing logic into 1 singular function here. I had to make sure that spaces outside quotes are trimmed, double quotes were handled correctly, and the input is processed as a single command. Some unexpected challenges when I refactoring was when I was keeping track of where the tokens were stored. Previously, I had a temp copy for each command, now I had to make sure the tokens were stored until the function call was complete, they were not each individually stored in a temp copy anymore. I also had to add some more logic into the code, I had different ways of handling spaces before, so I had to change the way that worked. Making sure outside quotes spaces were removed and inside quotes spaces remained was challenging.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**: The purpose of a signal is to send messages to tell a specific process that something happened. Each signal has a current disposition, which determines how the process behaves when it is delivered the signal. It sends messages through dispositions to a process about what happened. There are default dispositions for each signal like Term (action is to terminate the process) and Stop (action is to stop the process). They differ from other forms of IPC because signals do not carry any other data except for the disposition and message for the process. Other IPC's such as pipes carry data between different processes.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**: SIGKILL is used to terminate a process immediately. This signal cannot be caught, blocked, or ignored. A typical use case is if a specific process is not responding and you need to stop it. Also it is used by container orchastators like Kubernetes, if they need to shut down a container immediately. SIGTERM is also used to terminate a program, but it is different from SIGKILL because SIGTERM lets the program shut down and perform cleanup tasks, or even refuse to shut down unlike SIGKILL. Typical use cases for SIGTERM are stopping a running service or application on a server and application updates. SIGINT is used to interrupt a process from keyboard. It can be caught or ignored. A typical use case for this is when you are running your code and it is in an infinite loop and you want to stop the execution of the code, you click control+C to interrupt the execution.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: When a process recieves SIGSTOP it stops the process it was sent to. It, like SIGKILL, cannot be caught, blocked, or ignored. This is because it is a signal designed like SIGKILL or control over the execution of the process. If this signal is meant to have absolute control over a process when it is sent to it, then it would not make sense for the signal to be caught or ignored, it must have absolute power and control over the process. When a process recieves SIGSTOP or SIGKILL, they cannot be ignored because they are meant to have control over the execution of the process.
