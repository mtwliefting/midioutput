#!/bin/bash

gpio write 16 1

now=$(date +%d%b-%H%M)

FNAME="dans-$now.mid"

arecordmidi -p 14 /home/pi/midi/$FNAME & 

