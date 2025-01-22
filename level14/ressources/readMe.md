ltrace getflag -> que getflag ets protégé par ptrace: 

Cette utilisation particulière de ptrace est une technique anti-débogage classique :

- Si le processus est déjà sous débogueur (comme GDB), l'appel à ptrace échouera
- Un processus ne peut être tracé que par un seul autre processus à la fois
- Si un débogueur est déjà attaché, ptrace retournera -1 (échec)
- Si aucun débogueur n'est attaché, ptrace retournera 0 (succès)

on utilise gdb sur /bin/getflag :  
	run : you should not reverse this -> protection par ptrace  
	disas main :  
```bash
0x08048946 <+0>:     push %ebp
0x08048947 <+1>:     mov    %esp,%ebp
0x08048949 <+3>:     push   %ebx
0x0804894a <+4>:     and    $0xfffffff0,%esp
0x0804894d <+7>:     sub    $0x120,%esp
0x08048953 <+13>:    mov    %gs:0x14,%eax
0x08048959 <+19>:    mov    %eax,0x11c(%esp)
0x08048960 <+26>:    xor    %eax,%eax
0x08048962 <+28>:    movl   $0x0,0x10(%esp)
0x0804896a <+36>:    movl   $0x0,0xc(%esp)
0x08048972 <+44>:    movl   $0x1,0x8(%esp)
0x0804897a <+52>:    movl   $0x0,0x4(%esp)
0x08048982 <+60>:    movl   $0x0,(%esp)
0x08048989 <+67>:    call   0x8048540 <ptrace@plt>
0x0804898e <+72>:    test   %eax,%eax
0x08048990 <+74>:    jns    0x80489a8 <main+98>
```  
on remarque que ptrace est appelée a main+67 et sa valeur est testée a main+72 
Pour contourner cette protection, on break a main+72, run dans gdb, et set $eax à 0 au lieu de -1:

```bash
(gdb) break *main+72
Breakpoint 1 at 0x804898e
(gdb) run
Starting program: /bin/getflag
Breakpoint 1, 0x0804898e in main ()
(gdb) print $eax
$1 = -1
(gdb) set $eax=0                           
```
Si on continue à ce moment là, un autre probleme: la commande getflag doit etre executée par user flag14. Notre uid = uid de level14, qui n'a pas les bonne permissions  -> il faut donc avec gdb trouver ou le uid est recupéré:
```bash
(gdb) disassemble main
(...)
  0x08048afd <+439>:   call   0x80484b0 <getuid@plt>
  0x08048b02 <+444>:   mov    %eax,0x18(%esp)
  0x08048b06 <+448>:   mov    0x18(%esp),%eax
  0x08048b0a <+452>:   cmp    $0xbbe,%eax
(...)       
```
getuid() est appelée à main+439, et le uid est comparé à la ligne main+452. 
Il faut maintenant chercher le uid de flag 14 pour ensuite pouvoir l'affecter à $eax jsute avant la comparaison (main+452) :  
```bash
id -u flag14
```
output:
```bash
3014
```

on relance gdb, et refait les premieres etapes pour bypass ptrace  
ensuite on s'occupe du uid:
```bash
(gdb) break *main+452
Breakpoint 2 at 0x8048b0a
(gdb) continue
Continuing.
Breakpoint 2, 0x08048b0a in main ()
(gdb) set $eax=3014
(gdb) continue
Continuing.
Check flag.Here is your token : 7QiHafiNa3HVozsaXkawuYrTstxbpABHD8CPnHJ
[Inferior 1 (process 3700) exited normally]                     
```
le token est : `7QiHafiNa3HVozsaXkawuYrTstxbpABHD8CPnHJ`  
note: il est intéressant de noter, que l'on peut utiliser cette technique pour tous les flags, sachant que leur uid = 30[numéro du level]. par exemple uid de flag07 = 3007.