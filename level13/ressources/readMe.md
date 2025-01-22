# LEVEL 13: Exploiting UID Verification

## Goal
Retrieve the token by manipulating the UID check in the `level13` binary using GDB.

## Steps

## I - Initial Context
1. **Directory Contents:**  
    In our home directory, we have one suid/guid executable with extended permissions:
    ```bash
    level13@SnowCrash:~$ ls -l
    total 8
    -rwsr-sr-x 1 flag13 level13 7303 Aug 30  2015 level13
    ```

2. **Testing the binary:**  
    When we run the executable, it performs a UID check:
    ```bash
    level13@SnowCrash:~$ ./level13
    UID 2013 started us but we we expect 4242
    ```
    The program indicates it expects to be run by a user with UID 4242, but we are running it as UID 2013.

2. **Checking our User ID:**  
    We can verify our current user ID:
    ```bash
    level13@SnowCrash:~$ id -u
    2013
    ```
    This confirms we're running as user ID 2013, which matches our level13 user account.

## II - Analyzing the Binary with GDB

1. **Examining the assembly code:**  
    We can use GDB to inspect how the program performs its UID verification:
    ```bash
    level13@SnowCrash:~$ gdb -q ./level13
    Reading symbols from /home/user/level13/level13...(no debugging symbols found)...done.
    (gdb) disas main
    Dump of assembler code for function main:
       0x0804858c <+0>:	push   %ebp
       0x0804858d <+1>:	mov    %esp,%ebp
       0x0804858f <+3>:	and    $0xfffffff0,%esp
       0x08048592 <+6>:	sub    $0x10,%esp
       0x08048595 <+9>:	call   0x8048380 <getuid@plt>
       0x0804859a <+14>:	cmp    $0x1092,%eax
       (...)
    ```
    We can see that:
    - The program calls `getuid()` to get the current user's UID
    - It then compares this value (stored in the `eax` register) with `0x1092` (4242 in decimal)

## III - Exploiting the UID Check

1. **Setting up the breakpoint:**  
    We'll place a breakpoint right after the `getuid()` call to modify the UID:
    ```bash
    (gdb) break *main+14
    Breakpoint 1 at 0x804859a
    (gdb) r
    Starting program: /home/user/level13/level13
    Breakpoint 1, 0x0804859a in main ()
    ```

2. **Verifying current UID:**  
    We can confirm our current UID matches what getuid retrieved:
    ```bash
    (gdb) print $eax
    $1 = 2013
    ```

3. **Modifying the UID:**  
    We can change the value in the `eax` register to bypass the check:
    ```bash
    (gdb) set $eax=4242
    (gdb) print $eax
    $3 = 4242
    ```

4. **Retrieving the token:**  
    Continue program execution to get the token:
    ```bash
    (gdb) cont
    Continuing.
    your token is 2A31L79asukciNyi8uppkEuSx
    [Inferior 1 (process 22851) exited with code 050]
    ```

The token is: `2A31L79asukciNyi8uppkEuSx`

## IV - Understanding the Vulnerability
The program relies on a simple UID check to validate the user's identity. By using GDB to manipulate the register that holds the UID value, we can bypass this security check. This highlights the importance of not relying solely on user ID checks that can be manipulated through debugging tools.