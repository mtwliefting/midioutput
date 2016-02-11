#!/bin/bash


if [ "$(pidof aplay)" ]
then
	killall aplay
else
        amixer cset numid=1 85%
	nohup aplay "/home/pi/midioutput/Walking the Dog3.wav" </dev/null >/dev/null 2>&1 &    
fi
