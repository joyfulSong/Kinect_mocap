#include "ros/ros.h"
#include "std_msgs/String.h"
#include "skeleton_listener/Skeleton.h"


void chatterCallback(const skeleton_listener::Skeleton& s) 
{
  for (int i = 0; i < s.position.size(); ++i) 
  {
                geometry_msgs::Vector3 p, o;
                p = s.position[i];
                o = s.orientation[i];
                ROS_INFO("%d : x: %lf, y: %lf, z: %lf", s.type[i], p.x, p.y, p.z);
                ROS_INFO("%d orientation: x: %lf, y: %lf, z: %lf", s.type[i], o.x, o.y, o.z);
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
