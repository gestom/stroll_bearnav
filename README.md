# STRoLL BearNav ++
## Robust and Long-Term Monocular Teach-and-Repeat Navigation using a Single-Experience Topological Map 

+ Day2Night navigation
+ Fisheye-Panorama navigation

### Authors: Li Sun, Marwan Taher, Filip Majer, Tomáš Krajník

This paper proposes a robust approach to monocular visual teach-and-repeat navigation (V-T&R) for long-term operation in the wild. The approach leverages deep learned descriptors and a fisheye camera to handle changes in illumination and rapid manoeuvres. In particular, a novel illumination adaptation method is proposed to learn illumination-invariant descriptors in a self-supervised way. Moreover, the approach exploits panoramic images from a fisheye camera to address the out-of-plane rotation variance issue of pin-hole camera-based navigation systems, in which the rotational error can be estimated explicitly without requiring accurate calibration. Consequently, our approach achieves day-to-night navigation using a single map and is able to repeat complex and fast manoeuvres, and even recover from kidnapped situations. To verify our approach, we performed three navigation experiments in various outdoor environments, where both navigation accuracy and robustness are investigated. The experimental results show that our approach is superior to the baseline method with regards to accuracy and robustness.  

## Prerequisities - before the seminar 

1. You should install Ubuntu 16 with ROS kinetic or Ubuntu 18 with ROS melodic.
2. Also, you should install other prerequisities: `sudo apt install git`.

### Ubuntu 16

Nothing special needs to be done here. You can continue with installation.

### Ubuntu 18

If you are using Ubuntu 18, you will need to compile opencv with opencv-contrib:

1. Create a folder to perform the compilation and switch to it: `mkdir ~/opencv;cd ~/opencv`
1. Download opencv: `git clone -b 3.4 --single-branch https://github.com/opencv/opencv.git`
1. Download opencv-contrib: `git clone -b 3.4 --single-branch https://github.com/opencv/opencv_contrib.git`
1. Go to opencv folder, create a build folder and switch to it: `mkdir opencv/build;cd opencv/build`
1. Tell opencv to compile with the contrib: `cmake -DOPENCV_ENABLE_NONFREE:BOOL=ON -DOPENCV_EXTRA_MODULES_PATH=~/opencv/opencv_contrib/modules ~/opencv/opencv`
1. Compile it: `make -j5`.
1. Install it: `sudo make install`

## Installation - before or during the seminar  

Prepare your environment in the home folder:

1. `cd `, `mkdir -p ~/robotika_ws/src`, `cd ~/robotika_ws/src`, `catkin_init_workspace`

Make your usb camera work:
1. Clone the usb_cam ROS driver: `git clone https://github.com/gestom/usb_cam.git`
1. Compile it: `cd ..`, `catkin_make`
1. Source your environment: `source devel/setup.bash`
1. Make your camera easy to access: `sudo chmod 777 /dev/video0`
1. Run the camera node: `roslaunch usb_cam usb_cam-test.launch`
1. Keep the camera running and open a new terminal to continue.

Make the `stroll_bearnav` package work:

1. `cd ~/robotika_ws/src`
1. Clone the stroll_bearnav package: `git clone --branch robotika_sk_19 https://github.com/gestom/stroll_bearnav.git`
1. Compile it: `cd ..`, `catkin_make`
1. Source your environment: `source devel/setup.bash`
1. Run it: `roslaunch stroll_bearnav stroll-core.launch`
1. Open a new terminal, source your environment and check the image features: `rosrun rqt_image_view rqt_image_view /image_with_features`
1. Open a new terminal, source your environment and check the system structure `rosrun rqt_graph rqt_graph`
1. Run the operator GUIs: `roslaunch stroll_bearnav stroll-gui.launch`
1. Now find the `mapper` client gui and create a map by entering its name, e.g. `A` behind the `fileName`click `Send goal`, wait for feedback and then click `Cancel goal`.
1. Now find the `loadMap` gui, enter the map name in the prefix and click `Send goal`
1. Start the navigation by clicking `Send goal` in the `navigator` gui.


## Run the docker container
docker run -it --network=host --gpus all --rm -v /home/kevin/tmp_ws:/home/tmp_ws sha256:5fc203c5dfa7a6329c985d73ca19b485ef5446e4e19ac5a4fce8aa28c6a5f018

### Instll Pytorch
pip3 install torch==1.5.0+cu101 torchvision==0.6.0+cu101 -f https://download.pytorch.org/whl/torch_stable.html

### Build the workspace
cd /home/tmp_ws && catkin_make && source devel/setup.bash

### Run the feature matching node
cd src/TRN_server/scripts
python KeyPointMatching_server.py

### Troubleshoot
[ERROR] docker: Error response from daemon: could not select device driver "" with capabilities: [[gpu]].
sudo apt-get install nvidia-container-toolkit



### References
1. 	Li Sun, Marwan Taher, Christopher Wild, Zhu Dingyun, Cheng Zhao, Filip Majer, Zhi Yan, Tomáš Krajník, Tony J Prescott, Tom Duckett: <b>[Robust and Long-Term Monocular Teach-and-Repeat Navigation using a Single-Experience Map.] side product of IROS 2021 submission.
1. T.Krajnik, J.Faigl et al.: <b>[Simple yet stable bearing-only navigation.](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/papers/surfnav.pdf)</b> Journal of Field Robotics, 2010. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/files/surfnav.bib)]
1. T.Krajnik, S.Pedre, L.Preucil: <b>[Monocular navigation for long-term autonomy.](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/papers/longterm.pdf)</b>In 16th International Conference on Advanced Robotics (ICAR), 2013. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/files/longterm.bib)]

