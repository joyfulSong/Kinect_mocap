cmake_minimum_required(VERSION 2.8.3)
project(skeleton_listener_wges)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  geometry_msgs
  std_msgs
  message_generation
)

include_directories(include
  ${skeleton_listener_wges_INCLUDE_DIRS} 
  ${catkin_INCLUDE_DIRS}
)

add_message_files(FILES Skeleton.msg)

generate_messages(
  DEPENDENCIES
  std_msgs
  geometry_msgs
  skeleton_listener
)

## Declare a catkin package
catkin_package(
  LIBRARIES skeleton_listener_wges
  CATKIN_DEPENDS roscpp rospy std_msgs message_runtime geometry_msgs
)

add_executable(listener_wges src/listener.cpp)
add_dependencies(listener_wges ${${PROJECT_NAME}_EXPORTED_TARGETS}
${catkin_EXPORTED_TARGETS})
target_link_libraries(listener_wges ${catkin_LIBRARIES})

