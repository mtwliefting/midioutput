#!/bin/bash
REFA=0
REFB=0
while true
do
	sleep 0.5
	INPUTA=`gpio read 28`
	INPUTB=`gpio read 29`	
	if [ $INPUTA != $REFA ]
	then 
		REFA=$INPUTA	
		if [ $INPUTA == 1 ]
			then
		        /home/pi/midioutput/install/inputscripts/1aan.sh	
			else 
			/home/pi/midioutput/install/inputscripts/1uit.sh

		fi
	fi
        if [ $INPUTB != $REFB ]
        then
                REFB=$INPUTB
                if [ $INPUTB == 1 ]
                        then
                        /home/pi/midioutput/install/inputscripts/2aan.sh 
                        else 
			/home/pi/midioutput/install/inputscripts/2uit.sh
                fi



        fi




done
