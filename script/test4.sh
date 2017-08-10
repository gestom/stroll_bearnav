#!/bin/bash

# Print statistics of matching of images from directory

while getopts ":h" param; do
	case $param in
	# help
	h) echo "usage: "
	   echo "`basename $0` [-h] [<path>] [<hessian_value>] [<before_value>] [<after_value>] [<outliers_threshold>] [<show_images>]"
	   echo "ex: bash ./test.sh ~/.ros/strechy/1 100 5 5 40 0"
	   exit 0
                ;;
	?) echo "unknown parameter $OPTARG" >&2
                ;;
	*) echo "Error: unhandled parameter $param"
        esac
done

if [ $# -eq 0 ]
  then
    echo "use -h for help"
fi

for i in {0..15000..25};do echo $i;./../../../devel/lib/stroll_bearnav/feature_testing_node ~/.ros/strechy/1/strechy_1_0000053_061_0031_0015_l.png ~/.ros/strechy/1/strechy_1_0000053_061_0031_0015_r.png "$i" "$3" "$4" "$5" "$6" "$i">>hesian_61.txt;done

grep stats -A 1 hesian_61.txt |grep -v stat|grep -v '--' >stat_61.txt

exit 0

