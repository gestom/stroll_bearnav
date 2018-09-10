set terminal fig color
set size 0.8,0.6
set key right bottom
set xlabel 'Registration error [px]'
set ylabel 'Probability [-]'
set ytics 0.2
stats 'results/Map_adaptive_fixed_15.srt' prefix "A"
plot [0:29] [:]\
'results/Map_static.srt' using 1:(1-$0/A_records) with lines title "Static map" lw 1,\
'results/Map_plastic.srt' using 1:(1-$0/A_records) with lines title "Plastic" lw 1,\
'results/Map_adaptive_fixed_15.srt' using 1:(1-$0/A_records) with lines title "Adaptive map - fixed 30" lw 1,\
'results/Map_adaptive_limited_0.5.srt' using 1:(1-$0/A_records) with lines title "Adaptive map - ratio 0.5" lw 1,\
