#!/bin/bash

echo "install wiringPi"
getWiringPi.sh

echo "install Asoundlib"
getAsoundlib.sh 

echo "installeer midiout als deamon"
midioutOnStartup.sh

echo "download en installeer vncserver als deamon"
getTightVNCserver.sh 
VNCOnStartup.sh

echo "!! vncserver opstarten om wachtwoord te maken"

echo "installeer inputs als deamon"
inputOnStartup.sh

echo "stel dhcpclient in op vast adres 192.168.1.10"
setStaticAdress.sh

echo "installeer seq24"
getSeq24.sh

echo "init git met username e.d."
initgit.sh


