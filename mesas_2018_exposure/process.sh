#check arguments
case $# in
    1) echo "path prefix: '$1'" ;;
    *) echo "wrong number of argument! 1st: path to folder containing maps."  1>&2
        exit -1
        ;;
esac

mkdir "results"

confidence=0.2
######################## exposure tests (Section 4.2)
#prepare maps for exposure tests
if [ 1 == 1 ];then
rosparam set names_map  [A0,B0]
rosparam set names_view [A0]
rosrun dynamic_reconfigure dynparam set /feature_extraction "{'adaptThreshold': True, 'maxLine': 0.5}"&
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False,'plasticMap': True,'remapRotGain': 0.0}"&

f="$1/mesas_2018_exposure/standard_converg"
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp $f/A0_GT.txt $f/B0_GT.txt

f="$1/mesas_2018_exposure/exposure_fixed"
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp $f/A0_GT.txt $f/B0_GT.txt

f="$1/mesas_2018_exposure/exposure_full"
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp $f/A0_GT.txt $f/B0_GT.txt

#run the exposure tests
rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': False,'remapRotGain': 1.0}"&
rosrun dynamic_reconfigure dynparam set /feature_extraction "{'maxLine':0.5, 'adaptThreshold':True}"&
rosparam set names_map  [B0,B1,B2,B3,B4,B5,B6,B7,B8,B9]
rosparam set names_view [A1,A2,A3,A4,A5,A6,A7,A8,A9]

# adaptive threshold, half image 
f="$1/mesas_2018_exposure/standard_converg"
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f 
cp ~/.ros/Results.txt results/Standard.txt 

# fixed threshold 
f="$1/mesas_2018_exposure/exposure_fixed"
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Exposure_full.txt 

# adaptive threshold, full image 
f="$1/mesas_2018_exposure/exposure_full"
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Exposure_fixed.txt 
fi

############ Feature adaptation tests (Section 4.3)
if [ 0 == 1 ];then

# adaptive - full image
rosparam set names_map  [A0,D0]
rosparam set names_view [A0]
f="$1/mesas_2018_exposure/standard_converg"
rosrun dynamic_reconfigure dynparam set /feature_extraction "{'maxLine':1.0}"&
rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': True,'remapRotGain': 0.0}"&
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp $f/A0_GT.txt $f/D0_GT.txt

rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': False,'remapRotGain': 1.0}"&
rosparam set names_map  [D0,D1,D2,D3,D4,D5,D6,D7,D8,D9]
rosparam set names_view [A1,A2,A3,A4,A5,A6,A7,A8,A9]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Features_full.txt 

# adaptive - no threshold adaptation 
rosparam set names_map  [A0,E0]
rosparam set names_view [A0]
rosrun dynamic_reconfigure dynparam set /feature_extraction "{'adaptThreshold': False, 'thresholdParam': 1346}"&
rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': True,'remapRotGain': 0.0}"&
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp $f/A0_GT.txt $f/E0_GT.txt

rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': False,'remapRotGain': 1.0}"&
rosparam set names_map  [E0,E1,E2,E3,E4,E5,E6,E7,E8,E9]
rosparam set names_view [A1,A2,A3,A4,A5,A6,A7,A8,A9]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Features_fixed.txt 
fi

############ Map adaptation test (Section 4.4)
if [ 0 == 1 ];then
#adaptive map tests on the day dataset 
f="$1/mesas_2018_exposure/standard_converg"
rosparam set names_map  [B0,B0,B0,B0,B0,B0,B0,B0,B0]
rosparam set names_view [A1,A2,A3,A4,A5,A6,A7,A8,A9]
rosrun dynamic_reconfigure dynparam set /feature_extraction "{'maxLine':0.5, 'adaptThreshold':True}"&
roslaunch stroll_bearnav evaluate.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_static.txt 

rosparam set names_map  [B0,F1,F2,F3,F4,F5,F6,F7,F8,F9]
rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': True,'remapRotGain': 1.0}"&
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_plastic.txt 
fi

#adaptive map tests on the day/night dataset 
if [ 0 == 1 ];
then
rosparam set names_map  [P0,M0]
rosparam set names_view [P0]
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False,'plasticMap': True,'remapRotGain': 0.0}"&
f="$1/mesas_2018_exposure/maps"
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp $f/P0_GT.txt $f/M0_GT.txt

#test the map update schemes 
rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': False,'remapRotGain': 1.0}"&
rosparam set names_map  [M0,M0,M0,M0,M0,M0,M0,M0,M0,M0,M0,M0]
rosparam set names_view [P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12]
roslaunch stroll_bearnav evaluate.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_static_2.txt

rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': True,'remapRotGain': 1.0}"&
rosparam set names_map  [M0,B1,B2,B3,B4,B5,B6,B7,B8,B9,B10,B11,B12]
rosparam set names_view [P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_plastic_2.txt

rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': False,'remapRotGain': 1.0}"&
rosparam set names_map  [M0,C1,C2,C3,C4,C5,C6,C7,C8,C9,C10,C11,C12]
rosparam set names_view [P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_adaptive_2.txt
fi

if [ 0 == 1 ];then
#starting to build a summary map 
rosparam set names_map  [A0,B0]
rosparam set names_view [A0]
f="$1/mesas_2018_exposure/super"
rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': True,'remapRotGain': 0.0}"&
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp $f/A0_GT.txt $f/B0_GT.txt

rosparam set names_map [$(for i in $(seq 0 88);do echo -ne B$i,;done|sed s/,$//)]
rosparam set names_view [$(for i in $(seq 1 87);do echo -ne A$i,;done|sed s/,$//)]
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': True, 'plasticMap': False,'remapRotGain': 0.0}"&
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f

fi

path=`pwd`
f="`pwd`/mesas_2018_exposure"
cd $f
make
cd $path

echo "in `pwd`"

#for i in Standard Exposure_full Exposure_fixed Features_full Features_fixed Map_static Map_plastic;do grep reports results/$i.txt|tail -n $((7*94))|awk '{a=$21-$23;print sqrt(a*a)}'| tee results/$i.err|sort -nr > results/$i.srt;done
for i in Standard Exposure_full Exposure_fixed;do grep reports results/$i.txt|tail -n $((7*94))|awk '{a=$21-$23;print (sqrt(a*a)+384)%768-384}'| tee results/$i.err|sort -nr > results/$i.srt;done #tail is to eliminate the effect of the map start
echo 
echo EXPOSURE TESTS: Section 4.2
echo -ne "	Error of Standard VS Fixed exposure: "
paste results/Standard.err results/Exposure_fixed.err          |./mesas_2018_exposure/t-test $confidence
echo -ne "	Error of Standard VS Exposure adapted to full image: " 
paste results/Standard.err results/Exposure_full.err 	   |./mesas_2018_exposure/t-test $confidence
echo -ne "	Exposure adapted to full image VS Fixed exposure: " 
paste results/Exposure_full.err results/Exposure_fixed.err |./mesas_2018_exposure/t-test  $confidence
echo 
gnuplot mesas_2018_exposure/exposure.gnu >results/exposure.fig 

for i in Standard Features_full Features_fixed;do grep reports results/$i.txt|awk '{a=$21-$23;print (sqrt(a*a)+384)%768-384}'| tee results/$i.err|sort -nr > results/$i.srt;done
echo 
echo FEATURE DETECTOR ADAPTATION TESTS: Section 4.3
echo -ne "	Error of Standard VS Fixed hessian: "
paste results/Standard.err results/Features_fixed.err          |./mesas_2018_exposure/t-test $confidence
echo -ne "	Error of Standard VS Features extracted from full image: " 
paste results/Standard.err results/Features_full.err          |./mesas_2018_exposure/t-test  $confidence
echo -ne "	Error of Features from full image image VS Fixed hessian: " 
paste results/Features_full.err results/Features_fixed.err |./mesas_2018_exposure/t-test $confidence	
echo 
gnuplot mesas_2018_exposure/features.gnu >results/features.fig 

for i in Standard Map_plastic Map_static;do grep reports results/$i.txt|awk '{a=$21-$23;print (sqrt(a*a)+384)%768-384}'| tee results/$i.err|sort -nr > results/$i.srt;done
echo MAP PLASTICITY TEST: Section 4.4
echo -ne "	Error of Adaptive VS Static: "
paste results/Standard.err results/Map_static.err          |./mesas_2018_exposure/t-test $confidence
echo -ne "	Error of Adaptive VS Plastic: " 
paste results/Standard.err results/Map_plastic.err          |./mesas_2018_exposure/t-test $confidence
echo -ne "	Error of Plastic VS Static: " 
paste results/Map_plastic.err results/Map_static.err |./mesas_2018_exposure/t-test $confidence	
echo 
gnuplot mesas_2018_exposure/map.gnu >results/map.fig 


for i in Map_adaptive_2 Map_plastic_2 Map_static_2;do grep reports results/$i.txt|awk '{a=$21-$23;print (sqrt(a*a)+384)%768-384}'| tee results/$i.err|sort -nr > results/$i.srt;done
echo MAP PLASTICITY TEST: Section 4.4
echo -ne "	Error of Adaptive VS Static: "
paste results/Map_adaptive_2.err results/Map_static_2.err          |./mesas_2018_exposure/t-test $confidence
echo -ne "	Error of Adaptive VS Plastic: " 
paste results/Map_adaptive_2.err results/Map_plastic_2.err          |./mesas_2018_exposure/t-test $confidence
echo -ne "	Error of Plastic VS Static: " 
paste results/Map_plastic_2.err results/Map_static_2.err |./mesas_2018_exposure/t-test $confidence	
echo 
gnuplot mesas_2018_exposure/map2.gnu >results/map2.fig 



#exit

#test the static map
#rosparam set /tester/names_map  [A0,A0,A0,A0,A0,A0,A0,A0,A0]
#rosparam set /tester/names_view [A1,A2,A3,A4,A5,A6,A7,A8,A9]
#roslaunch stroll_bearnav evaluate.launch 
#cp ~/.ros/Results.txt results/Map_static.txt

#for i in Features_half Features_all Features_fixed;
#do
#needs to recompile right now
#roslaunch stroll_bearnav evaluate.launch 
#grep reports $i.txt|tail -n $((7*94))|awk '{a=$21-$23;print sqrt(a*a)}' >$i.err;
#done
#paste Features_half.err Features_all.err |./t-test 
#paste Features_half.err Features_fixed.err |./t-test 
#paste Features_all.err Features_fixed.err |./t-test 

echo AAA
for i in Map_static Map_plastic Map_adaptive;
do
#needs to recompile right now
#roslaunch stroll_bearnav evaluate.launch 
grep reports results/$i.txt|awk '{a=$21-$23;print (sqrt(a*a)+384)%768-384}'|tee $i.err|sort -nr > $i.srt;
done
paste Map_adaptive.err Map_static.err |./mesas_2018_exposure/t-test 
paste Map_adaptive.err Map_plastic.err |./mesas_2018_exposure/t-test 
paste Map_plastic.err Map_static.err |./mesas_2018_exposure/t-test 
echo AAA
for i in random fremen_0 fremen_1_best fremen_1_mc;
do
#needs to recompile right now
#roslaunch stroll_bearnav evaluate.launch 
grep reports $i.txt|awk '{a=$21-$23;print (sqrt(a*a)+384)%768-384}' >$i.err;
done

for i in random without fremen_0 fremen_1 fremen_1_mc;do grep reports $i.txt|awk '{a=$21-$23;print (sqrt(a*a)+384)%768-384}'|tee $i.err|sort -nr > $i.srt;done
echo AA
paste fremen_0.err fremen_1.err |./mesas_2018_exposure/t-test 
paste fremen_0.err fremen_1_mc.err |./mesas_2018_exposure/t-test 
