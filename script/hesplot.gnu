set terminal fig color
set size 0.8,1.0
set logscale x 10
set title "Feature matches quality vs. hessian threshold"
set xlabel "Hessian threshold  [-]"
#set ylabel "Number of keypoints [-]"
plot [1:5000] [0:125] \
"stat_147.txt" using 8:($6) with lines lw 2 title "Number of correct matches [-]",\
'' using 8:($6/($6+$7)*100) with lines lw 2 title "Ratio of correct matches [%]",\
'' using 8:(($1+$2)/200)  with lines lw 2 title "Number of keypoints x100"

