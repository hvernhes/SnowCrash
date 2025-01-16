level 04

-> un fichier .pl -> c'est un fichier source Perl (langage principalement utilise pour la manipulation texte, automatisation taches systemes et dev web)
->Il est souvent utilisé dans les scripts d'administration système, les applications CGI (Common Gateway Interface) et pour l'analyse de données.

on execute le fichier : ./level04.pl
->
"Content-type: text/html


"

on cat le fichier: cat level04.pl
->
#!/usr/bin/perl
# localhost:4747
use CGI qw{param};
print "Content-type: text/html\n\n";
sub x {
  $y = $_[0];
  print `echo $y 2>&1`;
}
x(param("x"));


On comprend plusieurs choses :

    Ce script Perl est un script CGI qui reçoit des paramètres via une requête HTTP et exécute une commande système basée sur ces paramètres. 

        Un module CGI en Perl (Common Gateway Interface) est une bibliothèque qui permet de faciliter l'interaction entre un serveur web et des scripts Perl pour générer des pages web dynamiques

    # localhost:4747 -> le script est destiné à être exécuté sur le serveur localhost, et il écoute sur le port 4747

    use CGI qw{param}; -> Signifie que le script utilise le module CGI et qu'il importe spécifiquement la fonction param de ce module.

    sub x {
        $y = $_[0];
        print `echo $y 2>&1`;
    }
    -> le script utilise la fonction x pour afficher le contenu de la variable y.
        $y = $_[0] -> assigne cette valeur à la variable $y.
        print echo $y 2>&1; ->exécute la commande echo avec la valeur de $y. Cela signifie que toute commande contenue dans $y sera exécutée par le shell.

    x(param("x")); -> Cette ligne appelle la fonction x et lui passe comme argument la valeur du paramètre x de la requête HTTP.
    La variable y est donc récupérée via la fonction param du module CGI. 
    Le paramètre x est passé directement (sans aucune validation) à la commande shell, ce qui permet une injection de commande -> faille.


Exploitation de la faille :
 Nous allons exploiter cette faille pour exécuter des commandes arbitraires.

Exécution de commandes via injection :

Nous utilisons curl pour envoyer des requêtes HTTP avec des commandes injectées. Voici quelques exemples :

curl http://localhost:4747/level04.pl?x="\`/usr/bin/id\`"
-> uid=3004(flag04) gid=2004(level04) groups=3004(flag04),1001(flag),2004(level04)

curl http://localhost:4747/level04.pl?x="\`/usr/bin/whoami\`"
-> flag04 

Nous constatons que ce script est execute en tant qu'user flag04. Nous pouvons donc directement injecter la commande getflag :

curl http://localhost:4747/level04.pl?x="\`/bin/getflag\`"
-> Check flag.Here is your token : ne2searoevaevoem4ov4ar8ap 

/!\ ATTENTION: apres quelques tests, il n'est même pas obligatoire de spécifier "level04.pl"dans la requête curl car il semblerait que le serveur web soit configuré pour exécuter automatiquement le fichier level04.pl par défaut.

explication de la commande :
1. curl : Commande de ligne de commande pour effectuer des requêtes HTTP
2. URL http://localhost:4747/level04.pl?x="\/bin/getflag`"` : L'URL et ses composants

    http://localhost:4747/ : C'est l'URL de la ressource à laquelle nous faisons une requête. Cette adresse fait référence à un serveur web local ou distant qui écoute sur l'adresse IP localhost et le port 4747.
        localhost : L'adresse IP du serveur où le script level04.pl est exécuté.
        :4747 : Le port sur lequel le serveur web est accessible. Dans notre cas, le serveur web écoute sur le port 4747 et non le port standard HTTP 80.
    /level04.pl : Il s'agit du chemin vers le script CGI level04.pl qui sera exécuté par le serveur web.

    ?x="\/bin/getflag`"** : C'est la partie de l'URL qui représente la chaîne de requête, c'est-à-dire le paramètre que nous envoyons au script **level04.pl**. Ce paramètre est appelé **x`, et il contient une commande à exécuter.

        x= : Cela signifie que nous envoyons un paramètre nommé x dans l'URL. Ce paramètre est récupéré dans le script CGI par la fonction param("x") en Perl.
        "\/bin/getflag`"** : C'est la valeur du paramètre **x` que nous envoyons. Elle contient une commande à exécuter dans un shell.

3. Les backticks (`) dans l'URL

Les backticks sont utilisés dans un shell Unix/Linux pour exécuter une commande. Ils permettent de capturer la sortie d'une commande et de l'inclure dans une autre commande.

    `/bin/getflag` : Ce sont des backticks qui entourent /bin/getflag, ce qui signifie que /bin/getflag sera exécuté dans un shell et que sa sortie sera retournée.

4. Comment tout cela fonctionne ensemble

Lorsque nous envoyons cette requête curl http://192.168.1.48:4747/level04.pl?x="\/bin/getflag`"`, voici ce qui se passe :

    Le serveur web reçoit la requête HTTP GET.
        Le serveur web écoute sur le port 4747 et reçoit une requête contenant le paramètre x="\/bin/getflag`"`.

    Le serveur traite le script CGI level04.pl.
        Le script Perl level04.pl récupère la valeur du paramètre x en utilisant la fonction param("x") et l'assigne à la variable $y.
        Le script Perl exécute ensuite echo $y, où $y contient la commande `/bin/getflag`.
        Cela entraîne l'exécution de la commande /bin/getflag sur le serveur, et sa sortie est renvoyée comme réponse HTTP.

    La sortie de getflag est renvoyée à l'utilisateur.
        getflag affiche le flag, et cette sortie est renvoyée au client qui a effectué la requête (nous, via curl).

    Résultat final :
        curl affiche la réponse renvoyée par le serveur, qui est la sortie de la commande getflag, c'est-à-dire le flag.