cmake_minimum_required(VERSION 2.8.3)
project(skeleton_sender_wges) #package name will be saved in ${PROJECT_NAME}

set(TARGET_ARCH "Win32")

set(skeleton_sender_INCLUDE_DIRS 
  C:/opt/ros/melodic/x64/my_catkin_ws/src/skeleton_sender_wges/include
)

## Find catkin and any catkin packages for building
 # for a just default package--> find_package(catkin REQUIRED)
find_package(catkin REQUIRED COMPONENTS 
  roscpp
  rospy 
  std_msgs 
  geometry_msgs
  message_generation 
)

include_directories(include 
 ${skeleton_sender_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
)

## Declare a message file
add_message_files(FILES Skeleton.msg)

## Set message dependencies
generate_messages(DEPENDENCIES std_msgs geometry_msgs skeleton_sender)

## Declare a catkin package
catkin_package(
  CATKIN_DEPENDS roscpp rospy std_msgs message_runtime geometry_msgs
)

## create .exe with .cpp file
add_executable(skeleton_sender_wges src/skeleton_sender_wges.cpp)

target_link_libraries(skeleton_sender_wges
  ${catkin_LIBRARIES}
) 

