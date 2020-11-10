#include "ros/ros.h"
#include "std_msgs/String.h"
#include "skeleton_listener_wges/Skeleton.h"



void chatterCallback(const skeleton_listener_wges::Skeleton& s)
{
  for (int i = 0; i < s.position.size(); ++i) {
                geometry_msgs::Vector3 p;
                geometry_msgs::Quaternion o;
                p = s.position[i];
                o = s.orientation[i];
                ROS_INFO("%d position: x: %lf, y: %lf, z: %lf", s.type[i], p.x, p.y, p.z);
                ROS_INFO("%d orientation: x: %lf, y: %lf, z: %lf, w: %lf", s.type[i], o.x, o.y, o.z, o.w);
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
  ros::init(argc, argv, "listener_wges"); //set this same as the cpp file

  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("skeleton", 1000, chatterCallback);
 
  ros::spin();
  //ros::Duration(0.1).sleep();

  return 0;
}
