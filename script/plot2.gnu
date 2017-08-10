set terminal fig color
set size 0.8,1.0
set title "Feature correspondence vs. image brightness"
set xlabel "Average brightness of image pixels [-]"
set ylabel "Number of keypoints [-]"
#set y2label "Ratio of correct correspondences [-]"
plot [153:204] [0:12]\
"stat3.txt" using 8:($7) with lines lw 2 title "Number of incorrect correspondences",\
'' using 8:($6)  with lines lw 2 title "Number of correct correspondences",\
'' using 8:(($1+$2)/2)  with lines lw 2 title "Number of keypoints"

