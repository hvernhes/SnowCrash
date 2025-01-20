# Level 09: Reverse Engineering a Hash Function

## Goal
Decrypt the content of the `token` file by understanding and reversing the algorithm used by the `level09` binary.

## Steps

### I - Initial Exploration  

1. **Directory Contents:**  

    ```bash
    $ ls -la
    -rwsr-sr-x 1 flag09  level09 7640 Mar  5  2016 level09
    ----r--r-- 1 flag09  level09   26 Mar  5  2016 token
    ```
    Found a `binary` and a `token` file.

2. **Testing the Binary:**  

    ```bash
    $ ./level09
    You need to provied only one arg.

    $ ./level09 coucou
    cpwfsz

    $ ./level09 blabla
    bmcepf
    ```

### II - Understanding the Algorithm 

1. **Testing the algorithm**   

    ```bash
    $ ./level09 1
    1

    $ ./level09 11
    12

    $ ./level09 111
    123

    $ ./level09 1111
    1234

    $ ./level09 11111
    12345
    ```

2. **Algorithm Analysis:**  

    The program adds the index position value to the ASCII value of each character in the string.  
    For example:  
    - First character: ASCII value + 0 (index 0)  
    - Second character: ASCII value + 1  
    - Third character: ASCII value + 2  
    - And so on...  

### III - Solution

1. **Creating a Decryption Program**  
    ```bash
    vim /tmp/reverse_hash.c
    ```
    ```c
    #include <stdio.h>

    int main(int ac, char **av) {
        int i = 0;
        char c;
        
        while (av[1][i] != 0) {
            c = av[1][i];
            printf("%c", (c - i));
            i++;
        }
        printf("\n");
        return 0;
    }
    ```
    For each `character`, the program subtracts the current position `i` from the `ASCII value` of the `character`, and prints the result.  


2. **Decrypting the Token**  
    First, compile the C program:
    ```bash
    cd /tmp
    gcc reverse_hash.c -o decrypt
    ```

    Then use it to decrypt the token:
    ```bash
    ./decrypt `cat /home/user/level09/token`
    ```
    Output:
    ```bash
    f3iji1ju5yuevaus41q1afiuq
    ```

    Note: The backticks (\`) around `cat token` execute the `cat` command and pass its output as an argument to our decrypt program. You could also use `$(cat token)` which does the same thing.

3. **Retrieving the flag**  
    To retrieve the flag, the session has to switch to user `flag09`:  
    
    ```bash 
    su flag09
    ```
    Output:
    ```bash
    Password: 
    ```
    The password  `f3iji1ju5yuevaus41q1afiuq` is entered and the session switches to flag09.  
    To retrieve the flag the command `getflag` is used:   
    Output:
    ```bash
    Check flag.Here is your token : s5cAJpM8ev6XHw998pRWG728z
    ```