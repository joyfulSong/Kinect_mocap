cmake_minimum_required(VERSION 3.0.2)
project(skeleton_markers)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  tf
  std_msgs
  visualization_msgs
  geometry_msgs
  std_msgs
  message_generation
)

add_message_files(FILES Skeleton.msg)

generate_messages(
  DEPENDENCIES
  std_msgs
  geometry_msgs
)

catkin_package(
  LIBRARIES skeleton_markers
  CATKIN_DEPENDS 
     roscpp 
     rospy
     tf
     std_msgs 
     message_runtime 
     geometry_msgs
     visualization_msgs
)

link_directories(
  ${catkin_LIBRARY_DIRS}
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

#add_executable(skeleton_markers src/listened_gesture.cpp)
#target_link_libraries(skeleton_markers ${catkin_LIBRARIES})

catkin_install_python(PROGRAMS nodes/markers_from_skeleton_msg.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
