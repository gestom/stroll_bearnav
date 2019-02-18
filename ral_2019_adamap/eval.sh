for i in Map_static Map_plastic Map_adaptive;
do
#needs to recompile right now
#roslaunch stroll_bearnav evaluate.launch 
grep reports results/$i.txt|awk '($23<5000){a=$21-$23;print (sqrt(a*a)+384)%768-384}'|tee $i.err|sort -nr > $i.srt;
done
paste Map_adaptive.err Map_static.err |./t-test $1 
paste Map_adaptive.err Map_plastic.err |./t-test $1
paste Map_plastic.err Map_static.err |./t-test $1
