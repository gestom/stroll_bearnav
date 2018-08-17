#!/bin/bash 
 
view=[A58
map=[B56
for i in {59..87}
do
	view=${view},A$i
	map=${map},B56
	
done

view=${view}]
map=${map}]

view=[A58]
map=[B56]

rosparam set names_map  $map
rosparam set names_view  $view

imp=0
mp=0
mps=(1)		#parametry modelu
#sps=(250 500 1000)	#parametry strategii 
sps=(500)

for mt in  Fremen #Fremen Fremen #Sum W_Sum Mov_Avg
do
	mp=${mps[$imp]}		
	for st in Best #Monte_Carlo  #Quantile Monte_Carlo Best
	do
		for sp in ${sps[*]}
		do
       		echo "$mt $mp $st $sp"
			roslaunch stroll_bearnav evaluate-predictions.launch stc_file:=$(pwd)$1 stc_model_type:=$mt stc_model_param:=$mp stc_strategy_type:=$st stc_strategy_param:=$sp 
	
			mv ~/.ros/Results.txt $(pwd)/"$mt"_"$mp"_"$st"_"$sp"_result.txt
	

		done
			
	done	
	((imp++))
done
