#!/bin/bash


if [ "$(pidof aplay)" ]
then
	killall aplay
	killall omxplayer
else
        amixer cset numid=1 85%
	nohup aplay "/home/pi/midioutput/Walking the Dog3.wav" </dev/null >/dev/null 2>&1 &   
	nohup omxplayer -p -o hdmi /opt/vc/src/hello_pi/hello_video/test.h264 </dev/null >/dev/null 2>&1 &
fi
