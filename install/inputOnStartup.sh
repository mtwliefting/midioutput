#!/bin/bash

echo copy script to init.d
cp init.d/inputs /etc/init.d/inputs
chown root:root /etc/init.d/inputs
update-rc.d inputs defaults
update-rc.d inputs enable

