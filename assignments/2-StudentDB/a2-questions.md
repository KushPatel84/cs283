## Assignment 2 Questions

#### Directions
Please answer the following questions and submit in your repo for the second assignment.  Please keep the answers as short and concise as possible.

1. In this assignment I asked you provide an implementation for the `get_student(...)` function because I think it improves the overall design of the database application.   After you implemented your solution do you agree that externalizing `get_student(...)` into it's own function is a good design strategy?  Briefly describe why or why not.

    > **Answer**:  I think that it is a good idea for having a separate function for get_student(...). We use the get student function in the delete student to find and read the location. If we ever want to add any other functionality that needs to retrieve a student location then we can use the get_student function instead of having to retype the code again wherever it is needed. The function also does different error checks so if we need to add some other restrictions on finding a student we would only have to make changes in there.

2. Another interesting aspect of the `get_student(...)` function is how its function prototype requires the caller to provide the storage for the `student_t` structure:

    ```c
    int get_student(int fd, int id, student_t *s);
    ```

    Notice that the last parameter is a pointer to storage **provided by the caller** to be used by this function to populate information about the desired student that is queried from the database file. This is a common convention (called pass-by-reference) in the `C` programming language. 

    In other programming languages an approach like the one shown below would be more idiomatic for creating a function like `get_student()` (specifically the storage is provided by the `get_student(...)` function itself):

    ```c
    //Lookup student from the database
    // IF FOUND: return pointer to student data
    // IF NOT FOUND: return NULL
    student_t *get_student(int fd, int id){
        student_t student;
        bool student_found = false;
        
        //code that looks for the student and if
        //found populates the student structure
        //The found_student variable will be set
        //to true if the student is in the database
        //or false otherwise.

        if (student_found)
            return &student;
        else
            return NULL;
    }
    ```
    Can you think of any reason why the above implementation would be a **very bad idea** using the C programming language?  Specifically, address why the above code introduces a subtle bug that could be hard to identify at runtime? 

    > **ANSWER:** The implementation would be a very bad idea because of a subtle bug that could occur. Because student_t student is a local variable and local variables are stored on the memory stack, when the function is returning the memory address of student there may be a bug that occurs if that memory space is overwritten by another function call. This can cause the program to crash which is why the above implementation is not a good idea.

3. Another way the `get_student(...)` function could be implemented is as follows:

    ```c
    //Lookup student from the database
    // IF FOUND: return pointer to student data
    // IF NOT FOUND or memory allocation error: return NULL
    student_t *get_student(int fd, int id){
        student_t *pstudent;
        bool student_found = false;

        pstudent = malloc(sizeof(student_t));
        if (pstudent == NULL)
            return NULL;
        
        //code that looks for the student and if
        //found populates the student structure
        //The found_student variable will be set
        //to true if the student is in the database
        //or false otherwise.

        if (student_found){
            return pstudent;
        }
        else {
            free(pstudent);
            return NULL;
        }
    }
    ```
    In this implementation the storage for the student record is allocated on the heap using `malloc()` and passed back to the caller when the function returns. What do you think about this alternative implementation of `get_student(...)`?  Address in your answer why it work work, but also think about any potential problems it could cause.  
    
    > **ANSWER:** This approach solves the problem of the memory space being overwritten on the stack memory. It may work because now it uses malloc to allocate memory for that pointer but you have to remember to free the memory that it uses after the program is complete or there will be a memory leak. 


4. Lets take a look at how storage is managed for our simple database. Recall that all student records are stored on disk using the layout of the `student_t` structure (which has a size of 64 bytes).  Lets start with a fresh database by deleting the `student.db` file using the command `rm ./student.db`.  Now that we have an empty database lets add a few students and see what is happening under the covers.  Consider the following sequence of commands:

    ```bash
    > ./sdbsc -a 1 john doe 345
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 128 Jan 17 10:01 ./student.db
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 3 jane doe 390
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 256 Jan 17 10:02 ./student.db
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 63 jim doe 285 
    > du -h ./student.db
        4.0K    ./student.db
    > ./sdbsc -a 64 janet doe 310
    > du -h ./student.db
        8.0K    ./student.db
    > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 4160 Jan 17 10:03 ./student.db
    ```

    For this question I am asking you to perform some online research to investigate why there is a difference between the size of the file reported by the `ls` command and the actual storage used on the disk reported by the `du` command.  Understanding why this happens by design is important since all good systems programmers need to understand things like how linux creates sparse files, and how linux physically stores data on disk using fixed block sizes.  Some good google searches to get you started: _"lseek syscall holes and sparse files"_, and _"linux file system blocks"_.  After you do some research please answer the following:

    - Please explain why the file size reported by the `ls` command was 128 bytes after adding student with ID=1, 256 after adding student with ID=3, and 4160 after adding the student with ID=64? 

        > **ANSWER:** For ID=1, lseek jumps to the offset that I defined in my program which is id*student record size which is 64. So the offset is 64. Then it is going to write another 64 bytes so it is 64+64. lseek also creates holes by extending the file size. It fills these holes with zeros which creates gaps. After adding a student with ID=1, the ls returned 128 because my code writes 64 bytes onto each id_offset, but the id * the student record size is 64 so the total size is 1*64+64=128. lseek here creates a hole up until the 64th byte, then the write starts from 64 and onwards. For ID=3, it would be 3*64+64=256. Then 4160 with ID=64 because of 64*64+64=4160. Even though the zeros are not being used for storage, it adds to the total file size. ls does not seperate holes, it just returns the entire file size.

    -   Why did the total storage used on the disk remain unchanged when we added the student with ID=1, ID=3, and ID=63, but increased from 4K to 8K when we added the student with ID=64? 

        > **ANSWER:** In linux file systems the smallest unit of data storage is a block and they are defaulted to 4KB. They can be 512 bytes, 1KB, 2KB, 4KB, or 8KB. When the student’s ID=1,3, and 63, all of them fall within the 4KB block so du command shows 4K. ID=63 will be at 63*64+64=4096 bytes which means that this is the exact location where 4KB ends. When you go to ID=64, you are at 64*64+64 = 4160 bytes which means that a new block of size 4KB is needed because it no longer fits under the 4KB size block. So the 4KB block is allocated onto another 4KB block which results in the du command showing 8K for the ID=64 student.

    - Now lets add one more student with a large student ID number  and see what happens:

        ```bash
        > ./sdbsc -a 99999 big dude 205 
        > ls -l ./student.db
        -rw-r----- 1 bsm23 bsm23 6400000 Jan 17 10:28 ./student.db
        > du -h ./student.db
        12K     ./student.db
        ```
        We see from above adding a student with a very large student ID (ID=99999) increased the file size to 6400000 as shown by `ls` but the raw storage only increased to 12K as reported by `du`.  Can provide some insight into why this happened?

        > **ANSWER:**  When the student with ID=99999 is added, the id offset is going to be in 64*99999 and then write in the next 64 bytes so the total is going to be 64*99999+64=6400000. Ls has no regards for holes or gaps that are filled with zeros up until that student which is why it shows the entire file size as 6400000. However, du does mind the holes from the beginning of the file up until where the actual data was written. The file system doesn’t write blocks for space that is not holding actual data so for students with ID=1,3, and 63 the block of data that is needed is 4K and then for ID=64, the block that is needed is another 4K and then for only that portion with ID=99999, not counting the hole up until that data, another 4K for that data. So the total comes out to 4K+4K+4K which is the resulting 12K for the raw storage from the du command.