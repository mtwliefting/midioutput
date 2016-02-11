#!/bin/bash

echo copy script to init.d
cp init.d/midiout /etc/init.d/midiout
chown root:root /etc/init.d/midiout
update-rc.d midiout defaults
update-rc.d midiout enable

