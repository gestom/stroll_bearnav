#!/bin/bash 

 
#check arguments
case $# in
    3);;
    *) echo "wrong number of argument! 1st: path to folder containing maps. 2nd: path to file to same statistics. 3rd: path to folder containg rosbags to be played."  1>&2
        exit -1
        ;;
esac

source ~/bc_ros/devel/setup.bash

roslaunch stroll_bearnav stroll-core.launch folder:=$1 &            	 
P1=$!

sleep 3s

rosrun stroll_bearnav map_match_info_listener $2 &
P2=$!

roslaunch stroll_bearnav stroll-gui.launch &
P3=$!

sleep 10s
rosparam set use_sim_time true
kill $P3

time=$(date +%s)
time2=$(date +%s.%3N)


rostopic pub -1 /feature_extraction/parameter_updates dynamic_reconfigure/Config '{ bools: [ {name: "adaptThreshold", value: True}, {name: "optimazed", value: True} ], ints: [ {name: "targetKeypoints", value: 500}, {name: "thresholdParam", value: 400}, {name: "detector", value: 2}, {name: "descriptor", value: 2} ], strs: [ ], doubles: [ {name: "maxLine", value: 0.5}, {name: "featureOvershootRatio", value: 30.0} ], groups: [ {name: "Default", state: True, id: 0, parent: 0} ] }'

rostopic pub -1 /map_preprocessor/goal stroll_bearnav/loadMapActionGoal '{ header: { seq: 1, stamp: now, frame_id: ""}, goal_id: { stamp: now, id: "/Action_client_loader-1-'$time2'"}, goal: {prefix: "day_hostibejk_0"}}'

cd $3

TXT_FILES=( `ls` )

for i in ${TXT_FILES[*]}
do
	echo "playing rosbag $i"
    rostopic pub -1 /navigator/goal stroll_bearnav/navigatorActionGoal '{ header: { seq: 1, stamp: now, frame_id: ""}, goal_id: { stamp: now, id: "/Action_client_navigator-1-'$time2'"}, goal: {traversals: 0}}' 
    rosbag play $i --clock &
	P4=$!
	sleep 15s
	kill $P4
	
done

kill $P1
kill $P2
