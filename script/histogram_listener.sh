#!/bin/bash 

 
#check arguments
case $# in
    3);;
    *) echo "wrong number of argument! 1st: path to folder containing maps. 2nd: path to file to save histogram. 3rd: path to folder containg rosbags to be played."  1>&2
        exit -1
        ;;
esac

source ~/bc_ros/devel/setup.bash

#rosparam set use_sim_time true
roslaunch stroll_bearnav stroll-core.launch folder:=$1 &            	 
P1=$!


sleep 3s


rosrun dynamic_reconfigure dynparam set /feature_extraction detector 2
rosrun dynamic_reconfigure dynparam set /feature_extraction descriptor 2

rostopic pub -1 /map_preprocessor/goal stroll_bearnav/loadMapActionGoal '{ header: { seq: 1, stamp: now , frame_id: ""}, goal_id: { stamp: now, id: "/Action_client_loader-1-0.000"}, goal: {prefix: "day_hostibejk_0"}}'

cd $3

if [ ! -e $2 ]; then
    echo "File not found!"
    touch $2
fi

TXT_FILES=( `ls` )

index=0

for i in ${TXT_FILES[*]}
do
    end=${i##*.}
    if [ "$end" = "bag" ]; then
        index=$(($index+1))
        echo "playing rosbag $i"
        
        rosrun image_view image_saver image:=navigationMatches &
        P2=$!
        rostopic pub -1 /navigator/goal stroll_bearnav/navigatorActionGoal '{ header: { seq: 1, stamp: now, frame_id: ""}, goal_id: { stamp: now, id: "/Action_client_navigator-1-0.000"}, goal: {traversals: 0}}' 
        rosbag play $i --clock &
	    P4=$!
        rostopic echo navigationInfo/histogram -n 1 >> $2 
	    kill $P2
	    kill $P4
        
        mv left0000.jpg $index.jpg
        
        
    fi 	
done
kill $P2
kill $P1
exit 0