## Level 02: Capturing and Analyzing Network Traffic

### Objective
Capture network traffic, analyze it with `Wireshark`, and retrieve the password for `flag02`.

### Steps

1. **Download the `.pcap` file**  
   A `.pcap` file contains captured network packets, useful for network analysis. Transfer the file from the remote server to your local machine using `scp`:  
   ```bash
   scp -P 4242 level02@IPADDRESS:/home/user/level02/level02.pcap ./PATHTOPASTE
   ```
- `scp`: Securely copies the file from the remote machine to the local machine.
- Replace `IPADDRESS` with the actual IP address of the server and `PATHTOPASTE` with the local directory to store the file.

2. **Install Wireshark**  
	`Wireshark` is a graphical network protocol analyzer. Install it and set the necessary permissions to read the `.pcap` file.
	```bash
	chmod a+r level02.pcap
	```

3. **Open the `.pcap` file in Wireshark**  
	Once the file is open in `Wireshark`, you will see various TCP packets.  
	By analyzing the payload in hexadecimal, you can spot potential login credentials like `login` and `password`.

4. **Analyze the TCP Stream**  
	Right-click on any TCP packet and select Follow TCP Stream. You will see the communication in both ASCII and hexadecimal format.  
	Look for login and password attempts:
	```
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
	```

5. **Extract the Password**  
	In the payload, you find the login attempt:
	```txt
	login: l.le.ev.ve.el.lX.X
	Password: ft_wandr...NDRel.L0L
	```
6. **Try the Password**  
- In the TCP stream, we see the following login attempt:  
   ```
   Password: ft_wandr...NDRel.L0L
   ```  
- Initially, removing the dots (`.`) results in `ft_wandrNDRelL0L`.   
- We try this password with `su flag02`, but it doesn't work.

7. **Analyze the Hexadecimal Stream**  
	Next, we analyze the TCP stream in hexadecimal format and observe that the password
	```txt
	Password: ft_wandr...NDRel.L0L
	``` 
	Corresponds to the following hex values:
	```hex
	000d0a50617373776f72643a20 66 74 5f 77 61 6e 64 72 7f 7f 7f 4e 44 52 65 6c 7f 4c 30 4c 0d
	```
	The presence of `7F` (the hexadecimal value) in the dots positions, suggests that the dots (`.`) are actually the `DEL` (Delete) key, not actual dots. If they were true dots, their hexadecimal value would be `2E`.

8. **Correct the Password**  
	Since `7F` corresponds to the `DEL` key, we must `"press DELETE"` every time we encounter a dot in the password.  
	After removing the dots and applying this correction, the correct password becomes:
	```bash
	ft_waNDReL0L
	```



