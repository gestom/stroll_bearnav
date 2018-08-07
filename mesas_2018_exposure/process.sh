# exposure tests
rosparam set /tester/names_map  [A0,A1,A2,A3,A4,A5,A6,A7,A8]
rosparam set /tester/names_view [A1,A2,A3,A4,A5,A6,A7,A8,A9]
#roslaunch stroll_bearnav evaluate.launch folder_map:=/home/gestom/mesas_2018_exposure/2 folder_view:=/home/gestom/mesas_2018_exposure/2
#cp ~/.ros/Results.txt Full.txt 

#roslaunch stroll_bearnav evaluate.launch folder_map:=/home/gestom/mesas_2018_exposure/3 folder_view:=/home/gestom/mesas_2018_exposure/3
#cp ~/.ros/Results.txt Exposure_full.txt 

#roslaunch stroll_bearnav evaluate.launch folder_map:=/home/gestom/mesas_2018_exposure/4 folder_view:=/home/gestom/mesas_2018_exposure/4
#cp ~/.ros/Results.txt Exposure_fixed.txt 

# feature adaptation tests
rosparam set /tester/names_map  [B0,B1,B2,B3,B4,B5,B6,B7,B8]
rosparam set /tester/names_view [A1,A2,A3,A4,A5,A6,A7,A8,A9]

roslaunch stroll_bearnav evaluate.launch folder_map:=/home/gestom/mesas_2018_exposure/2 folder_view:=/home/gestom/mesas_2018_exposure/2
cp ~/.ros/Results.txt Features_full.txt 

rosparam set /tester/names_map  [C0,C1,C2,C3,C4,C5,C6,C7,C8]
rosparam set /tester/names_view [A1,A2,A3,A4,A5,A6,A7,A8,A9]
roslaunch stroll_bearnav evaluate.launch folder_map:=/home/gestom/mesas_2018_exposure/2 folder_view:=/home/gestom/mesas_2018_exposure/2
cp ~/.ros/Results.txt Features_fixed.txt 


for i in Full Exposure_full Exposure_fixed;do grep reports $i.txt|tail -n $((6*94))|awk '{a=$21-$23;print sqrt(a*a)}'| tee $i.err|sort -nr > $i.srt;done
paste Full.err Exposure_fixed.err          |./t-test
paste Full.err Exposure_full.err 	   |./t-test
paste Exposure_fixed.err Exposure_full.err |./t-test

#for i in Map_static Map_plastic cor0-9_2 cor0-9_3 cor0-9_4 Features_half Features_all Features_fixed_1346;do grep reports $i.txt|tail -n $((6*94))|awk '{a=$21-$23;print sqrt(a*a)}'| tee $i.err|sort -nr > $i.srt;done


#test the plastic map
#rosparam set /tester/names_map  [A0,P1,P2,P3,P4,P5,P6,P7,P8]
#rosparam set /tester/names_view [A1,A2,A3,A4,A5,A6,A7,A8,A9]
#roslaunch stroll_bearnav evaluate.launch 
#cp ~/.ros/Results.txt results/Map_plastic.txt


#test the static map
#rosparam set /tester/names_map  [A0,A0,A0,A0,A0,A0,A0,A0,A0]
#rosparam set /tester/names_view [A1,A2,A3,A4,A5,A6,A7,A8,A9]
#roslaunch stroll_bearnav evaluate.launch 
#cp ~/.ros/Results.txt results/Map_static.txt

#for i in Features_half Features_all Features_fixed_1346;
#do
##needs to recompile right now
##roslaunch stroll_bearnav evaluate.launch 
#grep reports $i.txt|tail -n $((6*94))|awk '{a=$21-$23;print sqrt(a*a)}' >$i.err;
#done
#paste Features_half.err Features_all.err |./t-test 
#paste Features_half.err Features_fixed_1346.err |./t-test 
#paste Features_all.err Features_fixed_1346.err |./t-test 
