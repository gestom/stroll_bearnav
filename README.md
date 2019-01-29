# STRoLL BearNav 
## Simple and Robust Visual Teach-and-replay Navigation System

### Authors: Filip Majer, Lucie Halodová, Tomáš Vintr and Tomáš Krajník

Bearnav is a simple teach-and-repeat visual navigation system robust to appearance changes induced by varying illumination and naturally-occurring environment changes. It's core method is computationally efficient, it does not require camera calibration and it can learn and autonomously traverse arbitrarily-shaped paths. During the teaching phase, where the robot is driven by a human operator, the robot stores its velocities and image features visible from its on-board camera.  During autonomous navigation, the method does not perform explicit robot localisation in the 2d/3d space but it simply replays the velocities that it learned during a teaching phase, while correcting its heading relatively to the path based on its camera data. The experiments performed indicate that the proposed navigation system corrects position errors of the robot as it moves along the path. Therefore, the robot can repeatedly drive along the desired path, which was previously taught by the human operator.
Early versions of the system proved their ability to reliably traverse polygonal trajectories indoors and outdoors during adverse illumination conditions [[1,2](#references)], in environments undergoing drastic appearance changes [[2,3](#references)] and on flying robots[[4](#references)].
The version presented here is described in [[5](#references)] and it allows to learn arbitrary, smooth paths, is fully integrated in the ROS operating system and is available on-line in this repository.

## Prerequisities - before the seminar 

You should install Ubuntu 16 with ROS kinetic (recommended) or Ubuntu 18 with ROS melodic.
Also, you should have git installed: `sudo apt install git`.

### Ubuntu 16

Nothing special needs to be done here. You can continue with installation.

### Ubuntu 18

If you are using Ubuntu 18, you will need to compile opencv with opencv-contrib:

1. Create a folder to perform the compilation and switch to it: `mkdir ~/opencv;cd ~/opencv`
1. Download opencv: `git clone -b 3.4 --single-branch https://github.com/opencv/opencv.git`
1. Download opencv-contrib: `git clone -b 3.4 --single-branch https://github.com/opencv/opencv_contrib.git`
1. Go to opencv folder, create a build folder and switch to it: `mkdir opencv/build;cd opencv/build`
1. Tell opencv to compile with the contrib: `cmake -DOPENCV_EXTRA_MODULES_PATH=~/opencv/opencv_contrib/modules ~/opencv/opencv`
1. Compile it: `make -j5`.
1. Install it: `sudo make install`

## Installation - before or during the seminar  

Prepare your environment in the home folder:

1. `cd `, `mkdir -p robotika_ws/src`, `cd robotika_ws/src`, `catkin_init_workspace`

Make your usb camera work:
1. Clone the usb_cam ROS driver: `git clone https://github.com/ros-drivers/usb_cam.git`
1. Compile it: `cd ..`, `catkin_make`
1. Source your environment: `source devel/setup.bash`
1. Make your camera easy to access: `sudo chmod 777 /dev/video0`
1. Run the camera node: `roslaunch usb_cam usb_cam-test.launch`
1. In Ubuntu 18, you might experience a crash of the: image_view, which is supposed to display the live camera feed. Instead, use rqt_image_view `rosrun rqt_image_view rqt_image_view image:=/usb_cam/image_raw`.

Make the `stroll_bearnav` package work:

1. `cd ~/robotika_ws/src`
1. Clone the stroll_bearnav package: `git clone --branch robotika_sk_19 https://github.com/gestom/stroll_bearnav.git`
1. Compile it: `cd ..`, `catkin_make`
1. Source your environment: `source devel/setup.bash`
1. Run it: `roslaunch stroll_bearnav stroll-core.launch`
1. Check the image features: `rosrun image_view image_view image:=/image_with_features`
1. Check the system structure `rosrun rqt_graph rqt_graph`
1. Run the operator GUIs: `roslaunch stroll_bearnav stroll-gui.launch`
1. Now find the `mapper` client gui and create a map by entering its name, e.g. `A` behind the `fileName`click `Send goal`, wait for feedback and then click `Cancel goal`.
1. Now find the `loadMap` gui, enter the map name in the prefix and click `Send goal`
1. Start the navigation by clicking `Send goal` in the `navigator` gui.

Test how the image features' matches from the map to the current view reflect the pan of the camera.

A detailed system description is provided in [[5](#references)].

### References
1. T.Krajnik, L.Preucil: <b>[A simple visual navigation system with convergence property.](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/papers/convergence.pdf)</b>In European Robotics Symposium, 2008. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/files/convergence.bib)]
1. T.Krajnik, J.Faigl et al.: <b>[Simple yet stable bearing-only navigation.](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/papers/surfnav.pdf)</b> Journal of Field Robotics, 2010. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/files/surfnav.bib)]
1. T.Krajnik, S.Pedre, L.Preucil: <b>[Monocular navigation for long-term autonomy.](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/papers/longterm.pdf)</b>In 16th International Conference on Advanced Robotics (ICAR), 2013. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/files/longterm.bib)]
1. T.Krajnik, M.Nitsche et al: <b>[A simple visual navigation system for an UAV.](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/papers/uav.pdf)</b>In 9th International Multi-Conference on Systems, Signals and Devices (SSD), 2012. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/files/uav.bib)]
1. F.Majer, L.Halodova, T.Krajnik: <b>[A precise teach and repeat visual navigation system based on the convergence theorem.](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/papers/pair.pdf)</b> In Student Conference on Planning in Artificial Intelligence and Robotics (PAIR), 2017 (in review). [[bibtex](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/files/pair.bib)]
