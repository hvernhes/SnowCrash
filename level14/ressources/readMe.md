# LEVEL 14: Bypassing Anti-Debug Protection

## Goal
Retrieve the flag by understanding and bypassing the anti-debugging protection mechanisms in the `getflag` binary, and manipulating the process to execute with elevated privileges.

## Initial Analysis
When entering the level,there are no file or binary to exploit.  
When we try to run `ltrace getflag`, we discover that the binary is protected by `ptrace`. This reveals an important security mechanism.

```bash
level01@SnowCrash:~$ ltrace /bin/getflag
__libc_start_main(0x8048946, 1, 0xbffff7a4, 0x8048ed0, 0x8048f40 <unfinished ...>
ptrace(0, 0, 1, 0, 0)                                            = -1
puts("You should not reverse this"You should not reverse this
)                              = 28
+++ exited (status 1) +++
```

### Understanding `ptrace` Protection
`ptrace` is a system call that provides a means by which one process (the "tracer") may observe and control the execution of another process (the "tracee").  
It's commonly used for debugging and is the foundation for tools like GDB. In this case, it's used as an anti-debugging technique:

- The binary attempts to `ptrace` itself
- Only one process can trace another process at a time
- If a debugger (like GDB) is already attached, the `ptrace` call will fail and return `-1`
- If no debugger is attached, `ptrace` succeeds and returns `0`
- By attempting to `ptrace` itself, the program can detect if it's being debugged

## Solution Steps

### I - Analyzing the Binary with GDB

1. **Initial Attempt:**
    ```bash
    gdb /bin/getflag
    (gdb) run
    Starting program: /bin/getflag
    You should not reverse this
    [Inferior 1 (process 17216) exited with code 01]
    ```
   The program detects debugging and outputs: "**you should not reverse this**"

2. **Examining the Assembly:**
   ```bash
   (gdb) disas main
   0x08048946 <+0>:     push %ebp
   0x08048947 <+1>:     mov    %esp,%ebp
   0x08048949 <+3>:     push   %ebx
   [...]
   0x08048989 <+67>:    call   0x8048540 <ptrace@plt>
   0x0804898e <+72>:    test   %eax,%eax
   0x08048990 <+74>:    jns    0x80489a8 <main+98>
   ```
   Key observations:
   - At `main+67`: The program calls ptrace
   - At `main+72`: The return value in `$eax` is tested
   - At `main+74`: A jump is performed if the value is non-negative (`jns` = "jump if not sign")

### II - Bypassing ptrace Protection

To bypass this protection, we need to manipulate the program's execution:

1. **Setting a Strategic Breakpoint:**
   ```bash
   (gdb) break *main+72
   Breakpoint 1 at 0x804898e
   (gdb) run
   Breakpoint 1, 0x0804898e in main ()
   ```
   We break right after the `ptrace` call to inspect its return value

2. **Manipulating the Return Value:**
   ```bash
   (gdb) print $eax
   $1 = -1
   (gdb) set $eax=0
   ```
   - We see `ptrace` returned `-1` (debugging detected)
   - We manually set it to `0` to trick the program into thinking it's not being debugged

### III - Handling User ID Verification

After bypassing `ptrace`, we encounter another protection: the program checks the **user's UID**.

1. **Locating the UID Check in Assembly:**
   ```bash
   (gdb) disassemble main
   [...]
   0x08048afd <+439>:   call   0x80484b0 <getuid@plt>
   0x08048b02 <+444>:   mov    %eax,0x18(%esp)
   0x08048b06 <+448>:   mov    0x18(%esp),%eax
   0x08048b0a <+452>:   cmp    $0xbbe,%eax
   [...]
   ```
   Analysis:
   - At `main+439`: The program calls `getuid()` to get the current **user's ID**
   - At `main+452`: It compares the `UID` with a specific value

2. **Finding the Required UID:**
   ```bash
   id -u flag14
   ```
   Output: `3014`
   
   This shows we need to impersonate **flag14's UID** to get the corresponding flag

### IV - Complete Exploit Process

1. **Full GDB Command Sequence:**
   ```bash
   gdb /bin/getflag
   (gdb) break *main+72       # Break after ptrace
   (gdb) break *main+452      # Break at UID comparison
   (gdb) run
   (gdb) set $eax=0          # Bypass ptrace check
   (gdb) continue
   (gdb) set $eax=3014       # Set UID to flag14's UID
   (gdb) continue
   ```

2. **Result:**
   ```
   Check flag.Here is your token : 7QiHafiNa3HVozsaXkawuYrTstxbpABHD8CPnHJ
   ```

## Understanding the UID Pattern

An interesting discovery is the pattern for **flag UIDs** in the system:
- The `UID` for any `flag user` follows the pattern: `30[level_number]`
- Examples:
  - flag07: UID = 3007
  - flag14: UID = 3014

This means we could theoretically retrieve any flag by adjusting the **UID** value in our exploit.
