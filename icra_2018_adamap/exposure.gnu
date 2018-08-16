set terminal fig color
set size 0.8,0.6
set key right bottom
set xlabel 'Registration error [px]'
set ylabel 'Probability [-]'
set ytics 0.2
plot [0:19] [:]\
'results/Exposure_fixed.srt' using 1:(1-$0/7/94) with lines title "Fixed: Fixed exposure, no adaptation" lw 2,\
'results/Exposure_full.srt' using 1:(1-$0/7/94) with lines title "Full: Adapting to the entire image" lw 2,\
'results/Standard.srt' using 1:(1-$0/7/94) with lines title "Standard: Adapting to top half of the image" lw 2,\
