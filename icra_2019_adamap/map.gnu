set terminal fig color
set size 0.8,0.6
set key right bottom
set xlabel 'Registration error [px]'
set ylabel 'Probability [-]'
set ytics 0.2
stats 'results/Map_adaptive_fixed_0.srt' prefix "A"
plot [0:29] [:]\
'results/Map_adaptive_fixed_0.srt' using 1:(1-$0/A_records) with lines title "Adaptive map:  gradual adaptation to changes" lw 1,\
'results/Map_adaptive_fixed_15.srt' using 1:(1-$0/A_records) with lines title "Adaptive map 15" lw 1,\
'results/Map_adaptive_fixed_0,5.srt' using 1:(1-$0/A_records) with lines title "Adaptive map 0.5" lw 1,\
