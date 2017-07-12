#!/bin/bash
if (( $# > 0 )) ; then
	mplayer "rtsp://172.43.50.194/axis-media/media.amp?camera=$1"
else
	mplayer "rtsp://172.43.50.194/axis-media/media.amp?camera=quad"
fi
