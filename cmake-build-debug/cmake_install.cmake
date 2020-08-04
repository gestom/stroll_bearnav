# Install script for directory: /home/kevin/jaguar_ws/src/stroll_bearnav

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  
      if (NOT EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}")
        file(MAKE_DIRECTORY "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}")
      endif()
      if (NOT EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/.catkin")
        file(WRITE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/.catkin" "")
      endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/_setup_util.py")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local" TYPE PROGRAM FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/_setup_util.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/env.sh")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local" TYPE PROGRAM FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/env.sh")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/setup.bash;/usr/local/local_setup.bash")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local" TYPE FILE FILES
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/setup.bash"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/local_setup.bash"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/setup.sh;/usr/local/local_setup.sh")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local" TYPE FILE FILES
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/setup.sh"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/local_setup.sh"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/setup.zsh;/usr/local/local_setup.zsh")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local" TYPE FILE FILES
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/setup.zsh"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/local_setup.zsh"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/.rosinstall")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local" TYPE FILE FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/.rosinstall")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stroll_bearnav/msg" TYPE FILE FILES
    "/home/kevin/jaguar_ws/src/stroll_bearnav/msg/Feature.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/msg/FeatureArray.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/msg/PathProfile.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/msg/NavigationInfo.msg"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stroll_bearnav/srv" TYPE FILE FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/srv/SetDistance.srv")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stroll_bearnav/action" TYPE FILE FILES
    "/home/kevin/jaguar_ws/src/stroll_bearnav/action/mapper.action"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/action/navigator.action"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/action/loadMap.action"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stroll_bearnav/msg" TYPE FILE FILES
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/mapperAction.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/mapperActionGoal.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/mapperActionResult.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/mapperActionFeedback.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/mapperGoal.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/mapperResult.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/mapperFeedback.msg"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stroll_bearnav/msg" TYPE FILE FILES
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/navigatorAction.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/navigatorActionGoal.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/navigatorActionResult.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/navigatorActionFeedback.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/navigatorGoal.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/navigatorResult.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/navigatorFeedback.msg"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stroll_bearnav/msg" TYPE FILE FILES
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/loadMapAction.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/loadMapActionGoal.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/loadMapActionResult.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/loadMapActionFeedback.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/loadMapGoal.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/loadMapResult.msg"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/stroll_bearnav/msg/loadMapFeedback.msg"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stroll_bearnav/cmake" TYPE FILE FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/stroll_bearnav-msg-paths.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/include/stroll_bearnav")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/roseus/ros" TYPE DIRECTORY FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/roseus/ros/stroll_bearnav")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/common-lisp/ros" TYPE DIRECTORY FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/common-lisp/ros/stroll_bearnav")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gennodejs/ros" TYPE DIRECTORY FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/share/gennodejs/ros/stroll_bearnav")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND "/usr/bin/python2" -m compileall "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/lib/python2.7/dist-packages/stroll_bearnav")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages" TYPE DIRECTORY FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/lib/python2.7/dist-packages/stroll_bearnav")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/stroll_bearnav" TYPE FILE FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/include/stroll_bearnav/distanceConfig.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/stroll_bearnav" TYPE FILE FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/include/stroll_bearnav/featureExtractionConfig.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/stroll_bearnav" TYPE FILE FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/include/stroll_bearnav/navigatorConfig.h")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/stroll_bearnav" TYPE FILE FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/lib/python2.7/dist-packages/stroll_bearnav/__init__.py")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND "/usr/bin/python2" -m compileall "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/lib/python2.7/dist-packages/stroll_bearnav/cfg")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/stroll_bearnav" TYPE DIRECTORY FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/devel/lib/python2.7/dist-packages/stroll_bearnav/cfg")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/stroll_bearnav.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stroll_bearnav/cmake" TYPE FILE FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/stroll_bearnav-msg-extras.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stroll_bearnav/cmake" TYPE FILE FILES
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/stroll_bearnavConfig.cmake"
    "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/catkin_generated/installspace/stroll_bearnavConfig-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stroll_bearnav" TYPE FILE FILES "/home/kevin/jaguar_ws/src/stroll_bearnav/package.xml")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/gtest/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/kevin/jaguar_ws/src/stroll_bearnav/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
