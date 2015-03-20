#!/bin/bash
clear
address=$(cat machine.config | grep serialAddress | sed 's/\s\s*/ /g' | cut -d' ' -f3)
echo ATTEMPTING CONNECTION WITH PORT $address

cd arduino/stdFirmataPlusServo
echo BUILDING FIRMATA
ino build 

echo UPLOADING FIRMATA
ino upload -p $address