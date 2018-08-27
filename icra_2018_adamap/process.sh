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
if [ 0 == 1 ];then
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
f="$1/experimenty_icra_2018"
#numbers=`seq 1 86`
#maps=`for i in $numbers; do echo "A0,"; done`
#maps=`echo "[$mapsA87]"`
#numbers=`seq 1 86`
#views=`for i in $numbers; do echo "A$i,"; done`
rosparam set names_map [A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0,A0]
rosparam set names_view [A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27,A28,A29,A30,A31,A32,A33,A34,A35,A36,A37,A38,A39,A40,A41,A42,A43,A44,A45,A46,A47,A48,A49,A50,A51,A52,A53,A54,A55,A56,A57,A58,A59,A60,A61,A62,A63,A64,A65,A66,A67,A68,A69,A70,A71,A72,A73,A74,A75,A76,A77,A78,A79,A80,A81,A82,A83,A84,A85,A86,A87]
rosrun dynamic_reconfigure dynparam set /feature_extraction "{'maxLine':0.5, 'adaptThreshold':True}"&
roslaunch stroll_bearnav evaluate.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_static.txt

#numbers=`seq 0 86`
#plastics=`for i in $numbers; do echo "A$i,"; done`
#plastics=`echo "[$plastics,87]"`
rosparam set names_map [A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24,A25,A26,A27,A28,A29,A30,A31,A32,A33,A34,A35,A36,A37,A38,A39,A40,A41,A42,A43,A44,A45,A46,A47,A48,A49,A50,A51,A52,A53,A54,A55,A56,A57,A58,A59,A60,A61,A62,A63,A64,A65,A66,A67,A68,A69,A70,A71,A72,A73,A74,A75,A76,A77,A78,A79,A80,A81,A82,A83,A84,A85,A86,A87]
rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': True,'remapRotGain': 1.0}"&
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_plastic.txt
fi

# rename all map files from A to P
#for i in `seq 0 87`; do for j in $files; do mv A$i$j P$i$j;done;done

#adaptive map tests on the day/night dataset
if [ 1 == 1 ];
then
rosparam set names_map  [P0,M0]
rosparam set names_view [P0]
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False,'plasticMap': True,'remapRotGain': 0.0}"&
f="$1/experimenty_icra_2018"
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp $f/P0_GT.txt $f/M0_GT.txt

#test the map update schemes
rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': False,'remapRotGain': 1.0}"&
rosparam set names_map  [M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0, M0]
rosparam set names_view [P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29, P30, P31, P32, P33, P34, P35, P36, P37, P38, P39, P40, P41, P42, P43, P44, P45, P46, P47, P48, P49, P50, P51, P52, P53, P54, P55, P56, P57, P58, P59, P60, P61, P62, P63, P64, P65, P66, P67, P68, P69, P70, P71, P72, P73, P74, P75, P76, P77, P78, P79, P80, P81, P82, P83, P84, P85, P86, P87]
roslaunch stroll_bearnav evaluate.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_static_2.txt

rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': True,'remapRotGain': 1.0}"&
rosparam set names_map  [M0,B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, B11, B12, B13, B14, B15, B16, B17, B18, B19, B20, B21, B22, B23, B24, B25, B26, B27, B28, B29, B30, B31, B32, B33, B34, B35, B36, B37, B38, B39, B40, B41, B42, B43, B44, B45, B46, B47, B48, B49, B50, B51, B52, B53, B54, B55, B56, B57, B58, B59, B60, B61, B62, B63, B64, B65, B66, B67, B68, B69, B70, B71, B72, B73, B74, B75, B76, B77, B78, B79, B80, B81, B82, B83, B84, B85, B86, B87]
rosparam set names_view [P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29, P30, P31, P32, P33, P34, P35, P36, P37, P38, P39, P40, P41, P42, P43, P44, P45, P46, P47, P48, P49, P50, P51, P52, P53, P54, P55, P56, P57, P58, P59, P60, P61, P62, P63, P64, P65, P66, P67, P68, P69, P70, P71, P72, P73, P74, P75, P76, P77, P78, P79, P80, P81, P82, P83, P84, P85, P86, P87]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_plastic_2.txt

rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': False,'remapRotGain': 1.0}"&
rosparam set names_map  [M0,C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16, C17, C18, C19, C20, C21, C22, C23, C24, C25, C26, C27, C28, C29, C30, C31, C32, C33, C34, C35, C36, C37, C38, C39, C40, C41, C42, C43, C44, C45, C46, C47, C48, C49, C50, C51, C52, C53, C54, C55, C56, C57, C58, C59, C60, C61, C62, C63, C64, C65, C66, C67, C68, C69, C70, C71, C72, C73, C74, C75, C76, C77, C78, C79, C80, C81, C82, C83, C84, C85, C86, C87]
rosparam set names_view [P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29, P30, P31, P32, P33, P34, P35, P36, P37, P38, P39, P40, P41, P42, P43, P44, P45, P46, P47, P48, P49, P50, P51, P52, P53, P54, P55, P56, P57, P58, P59, P60, P61, P62, P63, P64, P65, P66, P67, P68, P69, P70, P71, P72, P73, P74, P75, P76, P77, P78, P79, P80, P81, P82, P83, P84, P85, P86, P87]
roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_adaptive_2.txt
fi

#map prediction effects
if [ 0 == 1 ];
then
#starting to build a summary map
rosparam set names_map  [A0,B0]
rosparam set names_view [A0]
f="$1/mesas_2018_exposure/temporal"
rosrun dynamic_reconfigure dynparam set /navigator "{'plasticMap': True,'remapRotGain': 0.0}"&
#roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f
#cp $f/A0_GT.txt $f/B0_GT.txt

rosparam set names_map [$(for i in $(seq 0 58);do echo -ne B$i,;done|sed s/,$//)]
rosparam set names_view [$(for i in $(seq 1 58);do echo -ne A$i,;done|sed s/,$//)]
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': True, 'plasticMap': False,'remapRotGain': 1.0}"&
#roslaunch stroll_bearnav remapTest.launch folder_map:=$f folder_view:=$f

rosparam set names_map [$(for i in $(seq 58 87);do echo -ne B56,;done|sed s/,$//)]
rosparam set names_view [$(for i in $(seq 58 87);do echo -ne A$i,;done|sed s/,$//)]
rosrun dynamic_reconfigure dynparam set /navigator "{'summaryMap': False, 'plasticMap': False,'remapRotGain': 1.0}"&
roslaunch stroll_bearnav evaluate.launch folder_map:=$f folder_view:=$f
cp ~/.ros/Results.txt results/Map_adaptive_LT.txt

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

for i in Map_adaptive_LT Fremen_2_Monte_Carlo_500_result ;do grep reports results/$i.txt|awk '{a=$21-$23;print (sqrt(a*a)+384)%768-384}'| tee results/$i.err|sort -nr > results/$i.srt;done
echo MAP PLASTICITY TEST: Section 4.4
echo -ne "	Error of Adaptive VS Static: "
paste results/Map_adaptive_LT.err results/Fremen_2_Monte_Carlo_500_result.err          |./mesas_2018_exposure/t-test $confidence
#gnuplot mesas_2018_exposure/map3.gnu >results/map3.fig
