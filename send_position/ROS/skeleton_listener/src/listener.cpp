#include "ros/ros.h"
#include "std_msgs/String.h"
#include "skeleton_listener/Skeleton.h"


void chatterCallback(const skeleton_listener::Skeleton& s) 
{
  for (int i = 0; i < s.position.size(); ++i) 
  {
                geometry_msgs::Vector3 p;
                p = s.position[i];
                ROS_INFO("%d : x: %lf, y: %lf, z: %lf", s.type[i], p.x, p.y, p.z);
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
