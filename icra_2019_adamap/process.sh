#check arguments
case $# in
    1) echo "path prefix: '$1'" ;;
    *) echo "wrong number of argument! 1st: path to folder containing maps."  1>&2
        exit -1
        ;;
esac

mkdir "results"

confidence=0.05
if [ 1 == 1 ]
then
############ Map adaptation test (Section 4.4)
rosparam set names_map  [P0,M0]
rosparam set names_view [P0]
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False,'plasticMap': True,'histogramRating': False,'remapRotGain': 0.0}"&
f="$1/icra_2019_adamap/short"
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp $f/P0_GT.txt $f/M0_GT.txt
fi

if [ 1 == 1 ]
then
test the map update schemes
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False, 'plasticMap': False,'histogramRating': False,'remapRotGain': 1.0}"&
rosparam set names_map  [M0,M0,M0,M0,M0,M0,M0,M0,M0,M0,M0,M0]
rosparam set names_view [P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12]
roslaunch stroll_bearnav evaluate.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_static.txt

rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False, 'plasticMap': True,'histogramRating': False,'remapRotGain': 1.0}"&
rosparam set names_map  [M0,B1,B2,B3,B4,B5,B6,B7,B8,B9,B10,B11,B12]
rosparam set names_view [P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_plastic.txt

rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False, 'plasticMap': False,'histogramRating': False,'remapRotGain': 1.0}"&
rosparam set names_map  [M0,C1,C2,C3,C4,C5,C6,C7,C8,C9,C10,C11,C12]
rosparam set names_view [P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_adaptive.txt
fi

if [ 1 == 1 ]
then
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False, 'plasticMap': False,'histogramRating': True,'remapRotGain': 1.0}"&
rosparam set names_map  [M0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11,D12]
rosparam set names_view [P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_weighted.txt
fi

path=`pwd`
f="`pwd`/icra_2019_adamap"
cd $f
make
cd $path

echo "in `pwd`"

for i in Map_weighted Map_adaptive Map_plastic Map_static;do grep reports results/$i.txt|awk '{a=$21-$23;print (sqrt(a*a)+384)%768-384}'| tee results/$i.err|sort -nr > results/$i.srt;done
echo MAP PLASTICITY TEST: Section 4.4
echo -ne "	Error of Adaptive VS Static: "
paste results/Map_adaptive.err results/Map_static.err          |./icra_2019_adamap/t-test $confidence
echo -ne "	Error of Adaptive VS Plastic: "
paste results/Map_adaptive.err results/Map_plastic.err          |./icra_2019_adamap/t-test $confidence
echo -ne "	Error of Plastic VS Static: "
paste results/Map_plastic.err results/Map_static.err |./icra_2019_adamap/t-test $confidence
echo
gnuplot icra_2019_adamap/map.gnu >results/map.fig
