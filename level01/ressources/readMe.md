## Level 01: Cracking the Password Hash

### Goal
Find the password hash for `flag01`, crack it, and retrieve the token to progress to the next level.

### Steps

1. **Search for Files Containing "flag01"**  
   Use the `find` command to locate files containing the string `flag01`:  
   ```bash
   find / -type f -exec grep -l "flag01" {} \; 2>/dev/null
   ```
   - `-exec`: indicates that for each file found, a specific command will be executed..  
   - `grep -l "flag01"`:  
   grep searches for the text "flag01" inside the file.  
   The -l option makes grep display only the name of the file if the string is found, not the matching content.  
   - `{}` dynamically represents the path of the file found by find at that moment.     

	This command reveals the following files:
   ```
   /etc/group
   /etc/passwd
   ```

2. **Examine `/etc/passwd`:**  
	Since /etc/passwd often contains user information, search for flag01 in the file:
	```bash
	cat /etc/passwd | grep "flag01"
	```
	Output:
	```bash
	flag01:42hDRfypTqqnw:3001:3001::/home/flag/flag01:/bin/bash
	``` 
	The second field, `42hDRfypTqqnw`, is a hashed password.
3. **Crack the Hashed Password:**  
	Password hashes like this can be cracked using tools like John the Ripper, commonly used in CTF challenges.  
	Steps to crack the hash with john command:
- Save the hashed password into a file, e.g., `hash.txt`:
	```bash
	echo "42hDRfypTqqnw" > hash.txt
	```
- Use John the Ripper to crack the hash:
	```bash
	john hash.txt
	```
- To see the result use this command:
	```bash
	john --show hash.txt
	```
	Output:
	```bash
	abcdefg
	``` 


