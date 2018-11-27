#check arguments
case $# in
    1) echo "path prefix: '$1'" ;;
    *) echo "wrong number of argument! 1st: path to folder containing maps."  1>&2
        exit -1
        ;;
esac

mkdir "results"

f="$1/icra_2019_adamap/full"

############ Perform a remap
confidence=0.05
if [ 1 == 1 ]
then
rosparam set names_map  [A000,M000TR]
rosparam set names_view [A000]
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False,'plasticMap': True,'histogramRating': False,'remapRotGain': 0.0,'matchingRatio': 0.7}"&
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp $f/A000_GT.txt $f/M000_GT.txt
fi

#static and plastic maps
if [ 1 == 0 ]
then
#test the map update schemes
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False, 'plasticMap': False,'histogramRating': False,'remapRotGain': 1.0}"&
rosparam set names_map  [$(echo -ne "M000,";for i in $(seq -w 1 87);do echo -ne M000,;done)]
rosparam set names_view [$(for i in $(seq -w 1 87);do echo -ne A0$i,;done)]
roslaunch stroll_bearnav evaluate.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_static_grief.txt
fi

if [ 1 == 0 ]
then
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False, 'plasticMap': True,'histogramRating': False,'remapRotGain': 1.0}"&
rosparam set names_map  [$(echo -ne "M000,";for i in $(seq -w 1 5 87);do echo -ne B0$i,;done)]
rosparam set names_view [$(for i in $(seq -w 1 5 87);do echo -ne A0$i,;done)]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_plastic_tracia.txt
fi

#adaptive map
if [ 1 == 0 ]
then
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False, 'plasticMap': False,'histogramRating': False,'remapRotGain': 1.0,'maxFeatureRemap': 30,'minFeatureRemap': 30 }"&
rosparam set names_map  [$(echo -ne "M000,";for i in $(seq -w 1 87);do echo -ne C0$i,;done)]
rosparam set names_view [$(for i in $(seq -w 1 87);do echo -ne A0$i,;done)]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_adaptive_grief.txt
fi

#summary map
if [ 1 == 0 ]
then
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': True, 'plasticMap': False,'histogramRating': False,'remapRotGain': 1.0,'maxFeatureRemap': 30,'minFeatureRemap': 30 }"&
rosparam set names_map  [$(echo -ne "M000,";for i in $(seq -w 1 87);do echo -ne D0$i,;done)]
rosparam set names_view [$(for i in $(seq -w 1 87);do echo -ne A0$i,;done)]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_adaptive_summary_30.txt
fi

path=`pwd`
f="`pwd`/icra_2019_adamap"
cd $f
make
cd $path

echo "in `pwd`"
echo >results/all.txt
for i in $(ls results/Map_*.txt|sed s/.txt//);do  grep reports $i.txt|awk '($23<5000){a=$21-$23;b=(sqrt(a*a)+384)%768-384;print sqrt(b*b)}'| tee $i.err|sort -nr > $i.srt;done

for i in $(ls results/Map_*.txt|sed s/.txt//);do 
echo >$i.tmp
for j in $(ls results/Map_*.txt|sed s/.txt//);do 
paste $j.err $i.err          |./icra_2019_adamap/t-test $confidence >>$i.tmp
done
echo "Error $i: " $(grep -c higher $i.tmp) $(grep -c smaller $i.tmp) >>results/all.txt
done
gnuplot icra_2019_adamap/map.gnu >results/map.fig
