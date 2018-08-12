set terminal fig color
set size 0.8,0.6
set key right bottom
set xlabel 'Registration error [px]'
set ylabel 'Probability [-]'
set ytics 0.2
plot [0:19] [0:1]\
'results/Map_plastic.srt' using 1:(1-$0/9/94) with lines title "Plastic map: new map every traverse" lw 2,\
'results/Map_static.srt' using 1:(1-$0/9/94-0.01) with lines title "Static map: no adaptation to changes" lw 2,\
'results/Standard.srt' using 1:(1-$0/9/94) with lines title "Adaptive map:  gradual adaptation to changes" lw 2,\
#note the artificial offset! this is to stress that the static map is actually drawn, but it does not influence the statistical tests
