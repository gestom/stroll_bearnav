set terminal fig color
set size 0.8,0.6
set key right bottom
set xlabel 'Registration error [px]'
set ylabel 'Probability [-]'
set ytics 0.2
plot [0:25] [0:1]\
'results/Map_static_1_1.srt' using 1:(1-$0/8/12) with lines title "AGAST/BRIEF" lw 2,\
'results/Map_static_7_7.srt' using 1:(1-$0/8/12) with lines title "TF FULL LIFT/LIFT" lw 2,\
'results/Map_static_4_4.srt' using 1:(1-$0/8/12) with lines title "LIFT/LIFT" lw 2,\
'results/Map_static_5_5.srt' using 1:(1-$0/8/12) with lines title "Correlation" lw 2,\
'results/Map_static_3_1.srt' using 1:(1-$0/8/12) with lines title "UPSURF/BRIEF" lw 2,\
'results/Map_static_6_6.srt' using 1:(1-$0/8/12) with lines title "TF LIFT/LIFT" lw 2,\
#note the artificial offset! this is to stress that the static map is actually drawn, but it does not influence the statistical tests
