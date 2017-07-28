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

#~/.ros/strechy/1
for i in $(ls "$1"/*_l.png|sed s/...png$//);do echo $i;./../../../devel/lib/stroll_bearnav/feature_testing_node $i\_l.png $i\_r.png "$2" "$3" "$4" "$5" "$6";done

exit 0

