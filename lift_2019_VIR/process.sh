#check arguments
case $# in
    1) echo "path prefix: '$1'" ;;
    *) echo "wrong number of argument! 1st: path to folder containing maps."  1>&2
        exit -1
        ;;
esac

mkdir "results"

confidence=0.05
################################################
adaptive map tests on the day/night dataset
for det in $(seq 7 7)
do
for des in $(seq 7 7)
do
if [ 1 == 1 ];
then
#generate a map with new features
rosparam set names_map  [P0,M0]
rosparam set names_view [P0]
rosparam set detector $det
rosparam set descriptor $des
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False,'plasticMap': True,'remapRotGain': 0.0}"&
f="$1/lift_2019/reduced"
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp $f/P0_GT.txt $f/M0_GT.txt

#test vs a static map
rosparam set names_map  [P0,M0]
rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': False,'remapRotGain': 1.0}"&
rosparam set names_map  [M0,M0,M0,M0,M0,M0,M0,M0,M0,M0,M0,M0]
rosparam set names_view [P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12]
roslaunch stroll_bearnav evaluate.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_static_$det\_$des.txt
fi

#statisticak testing
if [ 0 == 1 ];
then
path=`pwd`
f="`pwd`/lift_2019_VIR"
cd $f
make
cd $path

echo "in `pwd`"

for i in Map_static_${det}\_${des};do grep reports results/$i.txt|awk '{a=$21-$23;b=(sqrt(a*a)+384)%768-384;print sqrt(b*b)}'| tee results/$i.err|sort -nr > results/$i.srt;done
fi
done
done


for det in $(seq 1 1)
do
for des in $(seq 1 1)
do
if [ 0 == 1 ];
then
#generate a map with new features
rosparam set names_map  [A0,B0]
rosparam set names_view [A0]
rosparam set detector $det
rosparam set descriptor $des
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False,'plasticMap': True,'remapRotGain': 0.0}"&
f="$1/lift_2019/reduced"
roslaunch stroll_bearnav remapTest2.launch folder_map:=$f folder_view:=$f
cp $f/P0_GT.txt $f/M0_GT.txt

#test vs a static map
#rosparam set names_map  [A0,B0]
#rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': False,'remapRotGain': 1.0}"&
#rosparam set names_map  [B0] #[M0,M0,M0,M0,M0,M0,M0,M0,M0,M0,M0,M0]
#rosparam set names_view  [A1] # [P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12] # P1,P2,P3,P4,P5,
#roslaunch stroll_bearnav evaluate2.launch folder_map:=$f folder_view:=$f
#cp ~/.ros/Results.txt results/Map_static_$det\_$des.txt
fi

#statisticak testing
if [ 0 == 1 ];
then
path=`pwd`
f="`pwd`/lift_2019_VIR"
cd $f
make
cd $path

echo "in `pwd`"

for i in Map_static_${det}\_${des};do grep reports results/$i.txt|awk '{a=$21-$23;b=(sqrt(a*a)+384)%768-384;print sqrt(b*b)}'| tee results/$i.err|sort -nr > results/$i.srt;done
fi
done
done
paste results/Map_static_6_6.err results/Map_static_7_7.err     |./lift_2019_VIR/t-test $confidence
ecap.ho
gnuplot lift_2019_VIR/map.gnu >results/map.fig 
