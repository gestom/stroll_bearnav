<<<<<<< HEAD
## ROS driver for Cameleon E / UGV / Unmanned Ground Vehicle

The ROS driver consists of two binaries, where one runs on the robot and the other locally.
To make it work, do the following.

1. Create a workspace, clone this repo, catkin_make it and source devel/setup.bash.
1. Turn on the robot and connect to its Wifi (ESSID: CAMELEON040).
1. The robot IP is 172.43.50.193, so set your own IP manually.
1. Go to the cameleon_ros_driver folder.
1. Copy robot-local files to the robot: scp -r server devel@172.43.50.193:~/
1. Compile and run the robot server: ssh devel@172.43.50.193 'cd server;make;./server'
1. Run the ros node: roslaunch cameleon_ros_driver cameleon_driver.launch
=======
# stroll_bearnav
>>>>>>> fd8e0a126df75cef962a14c361459a01ee61a22e
