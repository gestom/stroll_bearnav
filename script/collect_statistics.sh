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

time=$(date +%s)
time2=$(date +%s.%3N)



rosparam set use_sim_time true
rosrun dynamic_reconfigure dynparam set /feature_extraction detector 2
rosrun dynamic_reconfigure dynparam set /feature_extraction descriptor 2

rostopic pub -1 /map_preprocessor/goal stroll_bearnav/loadMapActionGoal '{ header: { seq: 1, stamp: now , frame_id: ""}, goal_id: { stamp: now, id: "/Action_client_loader-1-0.000"}, goal: {prefix: "day_hostibejk_0"}}'

cd $3

TXT_FILES=( `ls` )

for i in ${TXT_FILES[*]}
do
	echo "playing rosbag $i"
    rostopic pub -1 /navigator/goal stroll_bearnav/navigatorActionGoal '{ header: { seq: 1, stamp: now, frame_id: ""}, goal_id: { stamp: now, id: "/Action_client_navigator-1-0.000"}, goal: {traversals: 0}}' 
    rosbag play $i --clock &
	P4=$!
	sleep 60s
	kill $P4
	
done
sleep 120s
kill -2 $P2
kill $P1
