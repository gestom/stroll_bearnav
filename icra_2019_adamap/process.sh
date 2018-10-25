#check arguments
confidence=0.05
case $# in
    1) echo "path prefix: '$1'" ;;
    *) echo "wrong number of argument! 1st: path to folder containing maps."  1>&2
        exit -1
        ;;
esac

mkdir "results"
date >time.txt
f="$1/icra_2019_adamap/long"

if [ 1 == 1 ]
then
rosparam set names_map  [A000,M000]
rosparam set names_view [A000]
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False,'plasticMap': True,'histogramRating': False,'remapRotGain': 0.0}"&
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp $f/A000_GT.txt $f/M000_GT.txt
fi

if [ 0 == 1 ] 
then
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': True, 'plasticMap': False,'histogramRating': False,'remapRotGain': 1.0,'maxFeatureRemap': 50,'minFeatureRemap': 50,'remapRatio': 0.9 }"&
rosparam set names_map  [$(echo -ne "M000,";for i in $(seq -w 1 87);do echo -ne T0$i,;done)]
rosparam set names_view [$(for i in $(seq -w 1 87);do echo -ne A0$i,;done)]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_adaptive_summary_50.txt
date >>time.txt
fi

path=`pwd`
f="`pwd`/icra_2019_adamap"
cd $f
make
cd $path

echo "in `pwd`"
echo >results/all.txt
for i in $(ls results/Map_adaptive*.txt|sed s/.txt//);do  grep reports $i.txt|awk '($23<5000){a=$21-$23;b=(sqrt(a*a)+384)%768-384;print sqrt(b*b)}'| tee $i.err|sort -nr > $i.srt;done

for i in $(ls results/Map_adaptive*.txt|sed s/.txt//);do 
echo >$i.tmp
for j in $(ls results/Map_adaptive*.txt|sed s/.txt//);do 
paste $j.err $i.err          |./icra_2019_adamap/t-test $confidence >>$i.tmp
done
echo "Error $i: " $(grep -c higher $i.tmp) $(grep -c smaller $i.tmp); >>results/all.txt
done
