## Level 00: Finding the First Flag

### Goal
Locate the file owned by the user `flag00` and retrieve the password to progress to the next level.

### Steps

1. **Search for Files Owned by `flag00`:**  
   Use the `find` command to locate files belonging to the user `flag00`, while redirecting error messages to `/dev/null` to avoid cluttering the output:  
   ```bash
   find / -user flag00 2>/dev/null
   ```
   This command reveals a file named `john`.

2. **Examine the File:**  
   Display the contents of the file john:
   ```bash
   cat john
   ```
   Output: 
   ```
   cdiiddwpgswtgt
   ```

3. **Decrypt the Content:**  
	The content appears encrypted. A first hypothesis is that it's encrypted using the Caesar cipher. 
	I used Use an online tool to decrypt it: https://www.dcode.fr/chiffre-cesar  
	Decryption result: 
   ```
   nottoohardhere
   ```

