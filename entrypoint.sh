#!/bin/sh

mkdir -p ~/.vnc
echo "mypassword" | vncpasswd -f > ~/.vnc/passwd
chmod 600 ~/.vnc/passwd


vncserver :1 -geometry 1920x1080 -depth 32 -localhost


websockify --web /usr/share/novnc/ 6080 localhost:5901 &

openbox &

exec sh -c "DISPLAY=:1 ./build/NES $1"

tail -f /dev/null
