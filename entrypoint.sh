#!/bin/sh
#Xvfb :1 -screen 0 1920x1080x24 &
#
#sleep 2
#
#x11vnc -display :1 -N -forever -nopw -rfbport 5900 -localhost &
#
#sleep 2
#
#websockify --web /usr/share/novnc/ 6080 localhost:5900 &
#
#sleep 2
#
#exec sh -c "DISPLAY=:1 ./build/NES $1" 

mkdir -p ~/.vnc
echo "mypassword" | vncpasswd -f > ~/.vnc/passwd
chmod 600 ~/.vnc/passwd


vncserver :1 -geometry 1920x1080 -depth 32 -localhost


websockify --web /usr/share/novnc/ 6080 localhost:5901 &

openbox &

exec sh -c "DISPLAY=:1 ./build/NES $1"

tail -f /dev/null