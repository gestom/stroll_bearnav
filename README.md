## ROS driver for Cameleon E / UGV / Unmanned Ground Vehicle

1. create a workspace, clone the repo, catkin_make it and source your environment.
1. turn on the robot, connect to its Wifi with ESSID: CAMELEON040.
1. Robot IP is 172.43.50.193, set your own IP manually.
1. Go to the cameleon_ros_driver folder.
1. Copy robot-local files to the robot: scp -r server devel@172.43.50.193:~/
1. Compile and run the robot server: ssh devel@172.43.50.193 'cd server;make;./server'
1. Run the ros node: roslaunch cameleon_ros_driver cameleon_driver.launch
