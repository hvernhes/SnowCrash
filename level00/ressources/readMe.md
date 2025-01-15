
level 00 : pass: level00
	chercher le fichiers appartenant a flag00 : find / -user flag00 2>/dev/null
	on utilise la commande find et on redirige les erreurs vers /null
	avec la commande on trouve les ifchiers appartenant au user flag00 -> un fichier nommé john
	cat john -> cdiiddwpgswtgt
	semble crypté -> 1ere idee chiffrement cesar:https://www.dcode.fr/chiffre-cesar
	cdiiddwpgswtgt -> dechiffrement cesar(decalage de 15) -> nottoohardhere

flag00 : pass: nottoohardhere 
	token: x24ti5gi3x0o12eh4esiuxias

