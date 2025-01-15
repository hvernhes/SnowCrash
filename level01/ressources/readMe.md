level 01 : pass : x24ti5gi3x0o12eh4esiuxias
	chercher les fichiers contenant "flag01" : find / -type f -exec grep -l "flag01" {} \; 2>/dev/null
	-> /etc/group et /etc/passwd
	on cherche flag 01 dans /etc/passwd: cat /etc/passwd | grep "flag01"
	-> flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash
	Les fichiers de type etc/example sont souvent des fichiers contenant des hachages de mots de passe.
	On utilise souvent John the Ripper pour cracker ces mots de passe (dans les CTF)
	->/etc/passwd : hashed password : 42hDRfypTqqnw
	-> john hashed password -> abcdefg
flag01 : pass: abcdefg
	token : f2av5il02puano7naaf6adaaf
