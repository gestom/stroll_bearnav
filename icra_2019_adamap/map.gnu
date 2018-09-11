set terminal fig color
set size 0.8,0.6
set key right bottom
set xlabel 'Registration error [px]'
set ylabel 'Probability [-]'
set ytics 0.2
stats 'results/Map_plastic.srt' prefix "A"
plot [0:29] [:]\
'results/Map_plastic.srt' using 1:(1-$0/A_records) with lines title "Plastic map: new map every traverse" lw 1,\
'results/Map_static.srt' using 1:(1-$0/A_records) with lines title "Static map: no adaptation to changes" lw 1,\
'results/Map_adaptive.srt' using 1:(1-$0/A_records) with lines title "Adaptive map:  gradual adaptation to changes" lw 1,\
'results/Map_adaptive_experiences.srt' using 1:(1-$0/A_records) with lines title "Adaptive map:  gradual adaptation to changes" lw 1,\
