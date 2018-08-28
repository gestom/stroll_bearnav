set terminal fig color
set size 0.8,0.6
set key right bottom
set xlabel 'Registration error [px]'
set ylabel 'Probability [-]'
set ytics 0.2
plot [0:75] [:]\
'results/Map_adaptive_LT.srt' using 1:(1-$0/1260) with lines title "Adaptive map:  gradual adaptation to changes" lw 1,\
'results/Fremen_2_Monte_Carlo_500_result.srt' using 1:(1-$0/1260) with lines title "Predictive map: predicting to the change" lw 1,\
'results/Fremen_0_Monte_Carlo_500_result.srt' using 1:(1-$0/1260) with lines title "Predictive map: predicting to the change" lw 1,\
