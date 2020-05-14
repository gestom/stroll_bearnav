set terminal fig color  #tell the system to store in .fig file
set size 0.9,0.7 	#size of the graph
set key right bottom	#position of legents
set xlabel 'Registration error threshold [px]'
set ylabel 'Prob. of correct registration [-]'
set ytics 0.2
stats 'sorted_1.txt' prefix "A" #figure out length of the data
plot [0:200] [:]\
'sorted_1.txt' using 1:(1-$0/A_records) with lines title "Method 1" lw 1 lc 1,\
'sorted_2.txt' using 1:(1-$0/A_records) with lines title "Method 2 with injected error" lw 1 lc 1,\
