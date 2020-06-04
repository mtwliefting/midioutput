#!/bin/bash


if [ "$(pidof omxplayer.bin)" ]
then
	killall aplay
	echo raspberry | sudo -S pkill -9 omxplayer

else
        amixer cset numid=1 85%
	nohup aplay "/home/pi/midioutput/Walking the Dog3.wav" </dev/null >/dev/null 2>&1 &   
	nohup omxplayer -o local /opt/vc/src/hello_pi/hello_video/test.h264 </dev/null >/dev/null 2>&1 &
fi
