set terminal fig color
set size 0.9,0.7
set key right bottom
set xlabel 'Registration error [px]'
set ylabel 'Probability [-]'
set ytics 0.2
stats 'results/Map_adaptive_fixed_15.srt' prefix "A"
plot [0:29] [:]\
'results/Map_static.srt' using 1:(1-$0/A_records) with lines title "Static map strategy" lw 1,\
'results/Map_plastic.srt' using 1:(1-$0/A_records) with lines title "Latest map strategy" lw 1,\
'results/Map_adaptive_experiences.srt' using 1:((1-$0/A_records)*1.1) with lines title "Experience-based strategy (n=3)" lw 1,\
'results/Map_adaptive_aggresive.srt' using 1:(1-$0/A_records) with lines title "Aggressive strategy" lw 1,\
'results/Map_adaptive_fixed_30.srt' using 1:(1-$0/A_records) with lines title "Adaptive map strategy (n=30)" lw 1,\
