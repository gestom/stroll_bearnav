# STRoLL BearNav 
## Simple and Robust Visual Teach-and-replay Navigation System

### Authors: Filip Majer, Lucie Halodová, Tomáš Vintr and Tomáš Krajník

** For supporting materials of RA-L 17-0862, see also [WiKi](https://github.com/gestom/stroll_bearnav/wiki/RA-L-17-0862-supporting-materials) **

Bearnav is a simple teach-and-repeat visual navigation system robust to appearance changes induced by varying illumination and naturally-occurring environment changes. It's core method is computationally efficient, it does not require camera calibration and it can learn and autonomously traverse arbitrarily-shaped paths. During the teaching phase, where the robot is driven by a human operator, the robot stores its velocities and image features visible from its on-board camera.  During autonomous navigation, the method does not perform explicit robot localisation in the 2d/3d space but it simply replays the velocities that it learned during a teaching phase, while correcting its heading relatively to the path based on its camera data. The experiments performed indicate that the proposed navigation system corrects position errors of the robot as it moves along the path. Therefore, the robot can repeatedly drive along the desired path, which was previously taught by the human operator.
Early versions of the system proved their ability to reliably traverse polygonal trajectories indoors and outdoors during adverse illumination conditions [[1,2](#references)], in environments undergoing drastic appearance changes [[2,3](#references)] and on flying robots[[4](#references)].
The version presented here is described in [[5](#references)] and it allows to learn arbitrary, smooth paths, is fully integrated in the ROS operating system and is available on-line in this repository.

### System overview

The navigation system works in two steps: teach and repeat. During the learning phase, a robot is guided by an operator along a path, which is the robot supposed to autonomously navigate in the repeat phase. When learning, the robot extracts salient features from its on-board camera image and stores its current traveled distance and velocity. During autonomous navigation, the robot sets its velocity according to the traveled distance and compares the currently detected and previously mapped features to correct its heading. 

### Mapping phase

During this phase, the robot is driven through the environment by a human operator. The robot continuously measures the distance it traveled and whenever the operator changes the forward or angular velocity, the robot saves the current distance and the updated velocity values, a.k.a *path profile*. Additionally, the robot continuously extracts image features from its on-board camera image and every 0.2~m, it saves the currently detected image features in a local map, which is indexed by the current distance the robot traveled. Video 1 shows an early version of the system while learning. Note the image features detected in the on-board camera image of the robot.

[![Bearnav learning](https://img.youtube.com/vi/pXrnS68fY_k/0.jpg)](https://www.youtube.com/watch?v=pXrnS68fY_k)

### Navigation phase

During the navigation phase, the robot traverses the learned path by itself using the speeds in the *path profile* and *visual features* in the local maps. The path profile is simply 'replayed', i.e. the robot sets a relevant steering and forward velocity according to the distance traveled. The local maps are used to correct the robot heading. Videos 2 shows the navigation principle, videos 3 and 4 show an early version of the system where robots traverse a polyline-shaped path and finally, videos 5,6 and 7 show the robot traversing a smooth path.

[![P3AT moving along a straight line](https://img.youtube.com/vi/cfN587IjhKw/0.jpg)](https://www.youtube.com/watch?v=cfN587IjhKw)

[![MMP5 robot autonomous drive](https://img.youtube.com/vi/FEkp-f0PnAs/0.jpg)](https://www.youtube.com/watch?v=FEkp-f0PnAs)

[![UAV autonomous flight](https://img.youtube.com/vi/QR5G3qubKsk/0.jpg)](https://www.youtube.com/watch?v=QR5G3qubKsk)

[![Smooth path](https://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/pics/indoor_0.jpg)](https://youtu.be/1ATh0FF48Ao)

[![TAROS](https://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/pics/vop.jpg)](https://youtu.be/44A4YcfsqVI)

When moving, the robot retrieves image features from a relevant map (green crosses), extracts image features from its on-board camera (blue crosses), matches them  (green/blue lines) and calculates a histogram of their horizontal distances (green). The histogram maximum is then used to steer the robot close to the learned path.

### Navigation accuracy and reliability

Note that the system does not perform explicit localisation. Rather, it relies on the convergence principle [[1,2](#references)], which states that robot heading correction is sufficient to keep it near the intended path.
The idea behind the convergence principle is simple: since the robot steers to image features, which are not in infinity, it's trajectory slowly converges to the learned one.
The following video shows how the robot, which was placed away from its learned trajectory (in blue) slowly corrects its position. You can see that the potential robot trajectories would intersect at location with objects, which generated the image features used by the robot navigation method.

[![Single-segment convergence](https://img.youtube.com/vi/rqO5maZddLY/0.jpg)](https://www.youtube.com/watch?v=rqO5maZddLY)

The second video demonstrates the robot working as it traverses a closed path. You can observe that with each loop, the robot is getting closer to the learned path.

[![Closed path convergence](https://img.youtube.com/vi/M2krTZCbdaY/0.jpg)](https://www.youtube.com/watch?v=M2krTZCbdaY)

Finally, the third video shows a robot slowly converging to the originally taught smooth path with an error injected at the start of each loop.

[![Smooth path](https://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/pics/indoor_1.jpg)](https://youtu.be/aEMa03LRVDw)

The principal advantage of the system is that it does not require a lot of features to be matched and thus it's robust to adverse illumination conditions -- it works even at night.

[![Night outdoor experiment](https://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/pics/outdoor_0.jpg)](https://www.youtube.com/watch?v=XR5RzPOBX_8)

### System implementation

The navigation system is implemented in Robotic Operating System (ROS), version Kinetic. It requires that the robot hardware publishes */odom*, */joy* and */image* topics and listens to the */cmd* topic. The system composes of several nodes:

1. Feature extractor node extracts image features from the robot camera and passes them to the *mapping* and *navigator* nodes.
1. Distance monitor node receives data from robot odometry, measures traveled distance and notifies the mapper node to create an additional map every time a given distance was traveled.
1. Mapper node receives features from the *feature extractor* node and saves them into local map whenever it receives the aforementioned message from the *distance monitor* node. It also saves the path *profile*.
1. Map preprocessor node loads all local maps and path profile, sends them to the *navigator* node based on the traveled distance received from the *distance monitor*. 
1. Navigator node receives the velocity profile and local feature maps. Then it matches the features from the maps to the currently visible features from the *feature extractor* node, performs the histogram voting, calculates the robot velocity and steers the robot along the path. 

![Navigation system structure.](https://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/pics/system.png) Navigation system structure and modules' communication. 

A detailed system description is provided in [[5](#references)].

### References
1. T.Krajnik, L.Preucil: <b>[A simple visual navigation system with convergence property.](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/papers/convergence.pdf)</b>In European Robotics Symposium, 2008. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/files/convergence.bib)]
1. T.Krajnik, J.Faigl et al.: <b>[Simple yet stable bearing-only navigation.](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/papers/surfnav.pdf)</b> Journal of Field Robotics, 2010. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/files/surfnav.bib)]
1. T.Krajnik, S.Pedre, L.Preucil: <b>[Monocular navigation for long-term autonomy.](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/papers/longterm.pdf)</b>In 16th International Conference on Advanced Robotics (ICAR), 2013. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/files/longterm.bib)]
1. T.Krajnik, M.Nitsche et al: <b>[A simple visual navigation system for an UAV.](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/papers/uav.pdf)</b>In 9th International Multi-Conference on Systems, Signals and Devices (SSD), 2012. [[bibtex](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/files/uav.bib)]
1. F.Majer, L.Halodova, T.Krajnik: <b>[A precise teach and repeat visual navigation system based on the convergence theorem.](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/papers/pair.pdf)</b> In Student Conference on Planning in Artificial Intelligence and Robotics (PAIR), 2017 (in review). [[bibtex](http://raw.githubusercontent.com/wiki/gestom/stroll_bearnav/files/pair.bib)]
