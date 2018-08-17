#DRUHY KROK
#!/bin/bash 

 
#check arguments 1531825980 1531853700 1531855080


case $# in
    3);;
    *) echo "wrong number of argument! 1st: number of submaps. 2nd: path to file to save statistics. 3rd: path to folder containg folders with maps to collect statistic from."  1>&2
        #exit -1
        ;;
esac

rosrun stroll_bearnav map_match_info_listener $1 &
P2=$!


if [ ! -e $1 ]; then
    echo "File not found and creating new for statistics"
    touch $1
fi

#cd $2/

view=[A0
map=[B56
for i in {1..57}
do
	view=${view},A$i
	map=${map},B56
	
done
view=${view}]
map=${map}]

view=[A0]
map=[B56]

echo $view;
echo $map;

rosparam set names_map  $map
rosparam set names_view  $view
        
roslaunch stroll_bearnav evaluate-predictions.launch stc_file:="bla" 

read -n 1

kill -2 $P2

wait $P2

exit
