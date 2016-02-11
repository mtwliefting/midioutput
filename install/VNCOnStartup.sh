#!/bin/bash

echo "copy script to init.d"
cp init.d/startvnc /etc/init.d/startvnc
chown root:root /etc/init.d/startvnc
update-rc.d startvnc defaults
update-rc.d startvnc enable

