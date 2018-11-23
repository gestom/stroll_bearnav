set terminal fig color
set size 0.9,0.7
set key right bottom
set xlabel 'Registration error threshold [px]'
set ylabel 'Prob. of correct registration [-]'
set ytics 0.2
stats 'results/Map_static_brief.srt' prefix "A"
plot [0:25] [:]\
'results/Map_static_brief.srt' using 1:(1-$0/A_records) with lines title "Static + brief",\
'results/Map_adaptive_brief.srt' using 1:(1-$0/A_records) with lines title "Adaptive + brief",\
'results/Map_static_tracia.srt' using 1:(1-$0/A_records) with lines title "Static + tracia",\
'results/Map_adaptive_tracia.srt' using 1:(1-$0/A_records) with lines title "Adaptive + tracia",\
'results/Map_static_grief.srt' using 1:(1-$0/A_records) with lines title "Static + grief",\
'results/Map_adaptive_grief.srt' using 1:(1-$0/A_records) with lines title "Adaptive + grief",\

#'results/Map_adaptive.srt' using 1:(1-$0/A_records) with lines title "Static map strategy",\
#'results/Map_adaptive_tracie.srt' using 1:(1-$0/A_records) with lines title "Static map strategy wt",\
#'results/Map_adaptive_grief.srt' using 1:(1-$0/A_records) with lines title "Static map strategy gr",\
#'results/Map_plastic.srt' using 1:(1-$0/A_records) with lines title "Latest map strategy" lw 1 lc 2,\
#'results/Map_adaptive_experiences.srt' using 1:(1-$0/A_records) with lines title "Multiple map strategy" lw 1 lc 3,\
#'results/Map_adaptive_aggresive.srt' using 1:(1-$0/A_records) with lines title "Aggresinve strategy" lw 1 lc 4,\
##'results/Map_adaptive_strict.srt' using 1:(1-$0/A_records) with lines title "Strict strategy" lw 1 lc 5,\
#'results/Map_adaptive_summary_30.srt' using 1:(1-$0/A_records) with lines title "Summary map (n=30)" lw 1 lc 6,\
#'results/Map_adaptive_fixed_30.srt' using 1:(1-$0/A_records) with lines title "Adaptive map strategy (n=30)" lw 1 lc 7\
