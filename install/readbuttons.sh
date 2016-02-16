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
			echo "schakelaar 1 aan" >> /home/pi/test.txt
			else 
			echo "schakelaar 1 uit" >> /home/pi/test.txt

		fi
	fi
        if [ $INPUTB != $REFB ]
        then
                REFB=$INPUTB
                if [ $INPUTB == 1 ]
                        then
                        echo "schakelaar 2 aan" >> /home/pi/test.txt 
                        else 
			echo "schakelaar 2 uit" >> /home/pi/test.txt
                fi



        fi




done
