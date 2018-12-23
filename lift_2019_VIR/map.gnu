set terminal fig color
set size 0.8,0.6
set key right bottom
set xlabel 'Registration error [px]'
set ylabel 'Probability [-]'
set ytics 0.2
plot [0:109] [0:1]\
'results/Map_static_1_1.srt' using 1:(1-$0/12/8) with lines title "Det/Des 1/1" lw 2,\
'results/Map_static_1_2.srt' using 1:(1-$0/12/8) with lines title "Det/Des 1/2" lw 2,\
'results/Map_static_2_1.srt' using 1:(1-$0/12/8) with lines title "Det/Des 2/1" lw 2,\
'results/Map_static_2_2.srt' using 1:(1-$0/12/8) with lines title "Det/Des 2/2" lw 2,\
'results/Map_static_3_1.srt' using 1:(1-$0/12/8) with lines title "Det/Des 3/1" lw 2,\
'results/Map_static_3_2.srt' using 1:(1-$0/12/8) with lines title "Det/Des 3/2" lw 2,\
#note the artificial offset! this is to stress that the static map is actually drawn, but it does not influence the statistical tests
