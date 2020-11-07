#include "ros/ros.h"
#include "std_msgs/String.h"
#include "skeleton_listener/Skeleton.h"


/**
 * This tutorial demonstrates simple receipt of messages over the ROS system.
 */
void chatterCallback(const skeleton_listener::Skeleton& s) 
{
  for (int i = 0; i < s.position.size(); ++i) {
                geometry_msgs::Vector3 p;
                p = s.position[i];
                ROS_INFO("%d : x: %lf, y: %lf, z: %lf", s.type[i], p.x, p.y, p.z);
            }
           
            for (int i = 0; i < s.discrete_confi.size(); ++i) {
                ROS_INFO("Discrete gesture is: %s and %lf", s.discrete_type[i].c_str(), s.discrete_confi[i]);
            }
            for (int i = 0; i < s.continuous_confi.size(); ++i) {
                ROS_INFO("Continuous gesture is: %s and %lf", s.continuous_type[i].c_str(), s.continuous_confi[i]);
            }

}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "listener");

  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("skeleton", 1000, chatterCallback);
 
  ros::spin();
  ros::Duration(1.0).sleep();

  return 0;
}
