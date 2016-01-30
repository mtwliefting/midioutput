
all:
	gcc midioutput.c -lasound -lwiringPi -g -Wall -o midioutput

inst: all
	sudo cp midioutput /usr/sbin
	sudo chmod 700 /usr/sbin/midioutput
	sudo chown root /usr/sbin/midiouput


