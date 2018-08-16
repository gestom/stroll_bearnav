set terminal fig color
set size 0.8,0.6
set key right bottom
set xlabel 'Registration error [px]'
set ylabel 'Probability [-]'
set ytics 0.2
plot [0:19] [:]\
'results/Map_plastic_2.srt' using 1:(1-$0/960) with lines title "Plastic map: new map every traverse" lw 2,\
'results/Map_static_2.srt' using 1:(1-$0/960) with lines title "Static map: no adaptation to changes" lw 2,\
'results/Map_adaptive_2.srt' using 1:(1-$0/960) with lines title "Adaptive map:  gradual adaptation to changes" lw 2,\
