level02: pass: f2av5il02puano7naaf6adaaf
	il y a un fichier pcap:
		quest-ce qu'un fichier pcap?
			c'est un fichier utilisé pour stocker des captures de paquets de données réseau. 
			utilisé pour l'analyse et le diagnostic des réseaux en permettant l'enregistrement de tout le trafic réseau
			Ces paquets incluent les en-têtes des protocoles (comme Ethernet, IP, TCP/UDP) ainsi que la charge utile des données.
	on copie le fichier sur llsa machine locale : scp -P 4242 level02@IPADDRESS:/home/user/level02/level02.pcap ./PATHTOPASTE

	on installe wireshark (outil d'analyse graphique) -> il faut changer les droits pour lire

	on ouvre le fichier dans wireshark -> que des protocoles TCP (100) en les parcourant rapidement on voit dans la payload (hex) qui est visualisee en ASCII a octe des mots cles comme login ou password
	->clique droit sur un des paquets -> suivre le flux TCP : 




..%..%..&..... ..#..'..$..&..... ..#..'..$.. .....#.....'........... .38400,38400....#.SodaCan:0....'..DISPLAY.SodaCan:0......xterm.........."........!........"..".....b........b....	B.
..............................1.......!.."......"......!..........."........"..".............	..
.....................
Linux 2.6.38-8-generic-pae (::ffff:10.1.1.2) (pts/10)

..wwwbugs login: l.le.ev.ve.el.lX.X
..
Password: ft_wandr...NDRel.L0L
.
..
Login incorrect
wwwbugs login: 




ce qui nous interesse : Password: ft_wandr...NDRel.L0L

en enlevant les points on aurait ft_wandrNDRelL0L
on essaye avec su flag02 -> ca ne fonctionne pas.
on reprend le flux TCP en hexadecimal : 
on se rend compte que Password: ft_wandr...NDRel.L0L correpsond à :

000d0a50617373776f72643a20 66 74 5f 77 61 6e 64 72 7f 7f 7f 4e 44 52 65 6c 7f 4c 30 4c 0d

les points ne sont donc pas des points car leur valeur est de 7F (val hex de "." : 2E). la valeur 7F correspond en realite a la touche DEL (man ASCII)
on comprend donc qu'il faut "appuyer 3 fois sur DELETE a chaque fois qu'il y a un point"
ce qui donne : ft_waNDReL0L


token: kooda2puivaav1idi4f57q8iq 
