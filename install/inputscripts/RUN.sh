#!/bin/bash

while true
do

PIR=`gpio read 29`
    if [ $PIR == 1 ]
    then
        echo loop
        FNAME=$( ls /home/pi/midi | sort -R | tail -1 )
        aplaymidi -p 14 /home/pi/midi/$FNAME ; sleep 1; killall aplay
        echo stop
    fi
sleep 1
done
