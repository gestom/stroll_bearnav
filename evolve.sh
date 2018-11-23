for i in $(seq -w 0 0);do ./icra_2019_adamap/process.sh /home/gestom/ $i;killall -SIGUSR1 feature_evolver;done
