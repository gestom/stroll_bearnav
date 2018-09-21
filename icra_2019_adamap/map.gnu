set terminal fig color
set size 0.9,0.7
set key right bottom
set xlabel 'Registration error threshold [px]'
set ylabel 'Prob. of correct registration [-]'
set ytics 0.2
stats 'results/Map_adaptive_fixed_15.srt' prefix "A"
plot [0:29] [:]\
'results/Map_static.srt' using 1:(1-$0/A_records) with lines title "Static map strategy" lw 1 lc 1,\
'results/Map_plastic.srt' using 1:(1-$0/A_records) with lines title "Latest map strategy" lw 1 lc 2,\
'results/Map_adaptive_experiences.srt' using 1:(1-10*$0/A_records) with lines title "Multiple map strategy" lw 1 lc 3,\
'results/Map_adaptive_fixed_30.srt' using 1:(1-$0/A_records) with lines title "Adaptive map strategy (n=30)" lw 1 lc 7\
