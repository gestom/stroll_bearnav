#!/bin/bash

for i in stroll_bearnav/navigator stroll_bearnav/loadMap stroll_bearnav/mapper rqt_reconfigure /image
do 
while [ $(wmctrl -l |grep $i -c) == 0 ]; 
	do sleep 1; 
	echo Waiting for $i;
done
wmctrl -a $i -b add,above
done
mplayer $(rospack find stroll_bearnav)/data/nominal.wav
