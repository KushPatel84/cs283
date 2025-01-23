# universal makefile

This folder contains a "univeral" makefile that does the following:

- uses gcc to compile any file that matches *.c and outputs the base filename without extension
  - for example, if you have app1.c and app2.c, it would output two binaries: app1, app2
- creates a .gitignore in the same folder as makefile, that includes all the binaries output by the makefile
- (optional) run `make launch` to create `.vscode/launch.json` that has a target for each binary - **this is a basic launch config that uses gdb to debug your program**

# how to use it

Copy `makefile` and `launch.sh` to the folder that contains your *.c files. If you get an error that launch.sh is not executable, run `chmod +x launch.sh`.