binaire present dans le level03

execution du binaire : ./level03 -> Exploit me

cat level03 -> binaire avec certaines commandes lisibles

Pour retrouver "Exploit me" : strings level03 | grep "Exploit me"
La commande strings extrait les chaînes de caractères imprimables d'un fichier binaire. Tu peux ensuite utiliser grep pour rechercher la phrase désirée.
-> /usr/bin/env echo Exploit me 

Le fichier exécute la commande echo sans utiliser de chemin absolu. Cela signifie que nous pouvons exploiter ce comportement en modifiant notre PATH pour que notre propre commande echo soit utilisée à la place.

Rappel du fonctionnement :

    La commande /usr/bin/env recherche l'exécutable echo dans les répertoires listés dans la variable d'environnement PATH.
    Si un attaquant peut contrôler ou modifier la variable PATH, il peut faire en sorte que env trouve un exécutable malveillant nommé echo avant le véritable echo standard.

La commande export PATH=/tmp:$PATH modifie temporairement la variable d'environnement PATH en ajoutant /tmp comme priorité dans les chemins où les commandes sont recherchées par le shell.

Cela signifie que si un exécutable portant le même nom qu'une commande standard existe dans /tmp, il sera prioritaire sur les autres exécutables trouvés dans les répertoires suivants de PATH.

Après l'exécution de export PATH=/tmp:$PATH, lorsque tu tapes echo, le shell exécutera /tmp/echo au lieu de /bin/env/echo.



Étapes pour exploiter cette faille :

a) Créer un exécutable malveillant :
echo "/bin/getflag" > /tmp/echo
et le rendre executable:
chmod +x /tmp/echo

b)Modifier le PATH pour inclure le répertoire contenant le script :
export PATH=/tmp:$PATH

c) Exécuter la commande vulnérable :
./level03
-> Check flag.Here is your token : qi0maab88jeaj46qoumi7maus

Voici pourquoi ./level03 fonctionne avec cette méthode, mais getflag directement dans le shell ne fonctionne pas :
Le programme level03 est exécuté avec des permissions spécifiques :

    Le binaire level03 est probablement configuré avec le bit SUID activé. Cela signifie qu'il s'exécute avec les privilèges de son propriétaire (souvent un utilisateur spécifique ou root) plutôt que ceux de l'utilisateur qui l'exécute.
    Quand tu exécutes ./level03, le programme a accès à des ressources ou des droits supplémentaires nécessaires pour exécuter getflag et obtenir le flag.

Preuve que le bit SUID est activé :
Tu peux vérifier cela avec la commande :
ls -l ./level03
-> -rwsr-sr-x 1 flag03 level03 8627 Mar  5  2016 ./level03

Le s dans rws indique que le programme est exécuté avec les droits de son propriétaire (ici flag03).
Cela permet au binaire level03 d’accéder aux privilèges nécessaires pour exécuter getflag avec succès.
cad quand on execute level03 apres modifs du path echo en getflag, cest comme si le user flag03 faisait la commande getflag -> on obtient bien le token sans avoir besoin du mot de passe du user flag03.