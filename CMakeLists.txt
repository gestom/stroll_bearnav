cmake_minimum_required(VERSION 2.8.3)
project(stroll_bearnav)
set ( CMAKE_CXX_FLAGS "-ggdb")
## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  nav_msgs
  geometry_msgs
  cv_bridge
  image_transport
  message_generation
  dynamic_reconfigure
  actionlib_msgs
  actionlib
  rosbag
)
find_package(OpenCV REQUIRED)
## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)
## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend tag for "message_generation"
##   * add a build_depend and a run_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependency has been pulled in
##     but can be declared for certainty nonetheless:
##     * add a run_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

## Generate messages in the 'msg' folder
 add_message_files(DIRECTORY msg
   FILES
   Feature.msg
   FeatureArray.msg	
   PathProfile.msg
   NavigationInfo.msg
)

## Generate services in the 'srv' folder
add_service_files(
  FILES
  SetDistance.srv
)

## Generate actions in the 'action' folder
 add_action_files(
   FILES
   mapper.action
  navigator.action
	loadMap.action
 )

## Generate added messages and services with any dependencies listed here
 generate_messages(
   DEPENDENCIES
   std_msgs
   actionlib_msgs
 )

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## To declare and build dynamic reconfigure parameters within this
## package, follow these steps:
## * In the file package.xml:
##   * add a build_depend and a run_depend tag for "dynamic_reconfigure"
## * In this file (CMakeLists.txt):
##   * add "dynamic_reconfigure" to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * uncomment the "generate_dynamic_reconfigure_options" section below
##     and list every .cfg file to be processed

## Generate dynamic reconfigure parameters in the 'cfg' folder
 generate_dynamic_reconfigure_options(
  	cfg/distance.cfg
	cfg/featureExtraction.cfg
	cfg/navigator.cfg
    cfg/supervisor.cfg
 )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES cameleon_ros_driver
  CATKIN_DEPENDS roscpp rospy std_msgs message_runtime actionlib_msgs
#  DEPENDS 
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
# include_directories(include)
include_directories(
  ${catkin_INCLUDE_DIRS}
)
include_directories(${OpenCV_INCLUDE_DIRS})
## Declare a C++ library
#add_library(cameleon_driver src/CRobot.cpp)
#add_library(cameleon_message src/CMessage.cpp)
#add_library(cameleon_message_client src/CMessageClient.cpp)
#add_library(cameleon_timer src/CTimer.cpp)

## Add cmake target dependencies of the library
## as an example, code may need to be generated before libraries
## either from message generation or dynamic reconfigure
# add_dependencies(cameleon_ros_driver ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Declare a C++ executable
#add_executable(cameleon_ros_driver src/cameleon_ros_driver.cpp)
#add_executable(cameleon_video src/cameleon_video.cpp)
#add_executable(pid src/pid.cpp)
#add_executable(ImageConverter src/ImageConverter.cpp)
#add_executable(joyImage src/joyImage.cpp)
add_executable(mapper_node src/mapper.cpp)
add_executable(feature_extraction_node src/featureExtraction.cpp)
add_executable(feature_testing_node src/featureTesting.cpp)
add_executable(odometry_monitor_node src/odometryMonitor.cpp)
add_executable(map_preprocessor_node src/mapPreprocessor.cpp)
add_executable(tester_image_extractor src/testerImageExtractor.cpp)
add_executable(navigator_node src/navigator.cpp)
add_executable(visualisation_node src/visualisation.cpp)
add_executable(supervisor_node src/supervisor.cpp)
add_executable(rosbag_node src/rosbag.cpp)
add_executable(feature_holder_node src/featureHolder.cpp)
add_executable(map_match_info_listener src/mapMatchInfoListener.cpp)
add_executable(tester src/tester.cpp)
add_executable(auto_remapper src/remapper.cpp)

## Add cmake target dependencies of the executable
## same as for the library above
#add_dependencies(cameleon_ros_driver cameleon_message_client cameleon_message ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
#add_dependencies(cameleon_video imgFeatures ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
add_dependencies(feature_extraction_node ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg ${catkin_EXPORTED_TARGETS})
add_dependencies(feature_testing_node ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg ${catkin_EXPORTED_TARGETS})
add_dependencies(mapper_node ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg ${catkin_EXPORTED_TARGETS})
add_dependencies(map_preprocessor_node ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg ${catkin_EXPORTED_TARGETS})
add_dependencies(tester_image_extractor ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg ${catkin_EXPORTED_TARGETS})
add_dependencies(odometry_monitor_node ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg  ${catkin_EXPORTED_TARGETS})
add_dependencies(navigator_node ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg ${catkin_EXPORTED_TARGETS})
add_dependencies(visualisation_node ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg ${catkin_EXPORTED_TARGETS})
add_dependencies(supervisor_node ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg ${catkin_EXPORTED_TARGETS})
add_dependencies(feature_holder_node ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg ${catkin_EXPORTED_TARGETS})
add_dependencies(map_match_info_listener ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg ${catkin_EXPORTED_TARGETS})
add_dependencies(tester ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg ${catkin_EXPORTED_TARGETS})
add_dependencies(auto_remapper ${PROJECT_NAME}_generate_messages_cpp ${PROJECT_NAME}_gencfg ${catkin_EXPORTED_TARGETS})

## Specify libraries to link a library or executable target against
#target_link_libraries(cameleon_ros_driver cameleon_message_client cameleon_message cameleon_timer${catkin_LIBRARIES})
#target_link_libraries(cameleon_video ${catkin_LIBRARIES})
#target_link_libraries(pid ${catkin_LIBRARIES})
#target_link_libraries(ImageConverter cameleon_timer ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
#target_link_libraries(joyImage cameleon_timer ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(mapper_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(feature_extraction_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(feature_testing_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(odometry_monitor_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(map_preprocessor_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(tester_image_extractor ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(navigator_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(visualisation_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(supervisor_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(rosbag_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(feature_holder_node ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(map_match_info_listener ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(tester ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})
target_link_libraries(auto_remapper ${catkin_LIBRARIES} ${OpenCV_LIBRARIES})

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables and/or libraries for installation
# install(TARGETS cameleon_ros_driver cameleon_ros_driver_node
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_cameleon_ros_driver.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
