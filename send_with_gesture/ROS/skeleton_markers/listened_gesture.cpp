#include <ros/ros.h>
#include <string>

#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>

#include <geometry_msgs/Point.h>
#include "std_msgs/String.h"
#include "using_markers/Skeleton.h"

// /* message definition
// std_msgs/Header header
// string ns
// int32 id
// int32 type
// int32 action
// geometry_msgs/Pose pose
// geometry_msgs/Vector3 scale
// std_msgs/ColorRGBA color
// duration lifetime
// bool frame_locked
// geometry_msgs/Point[] points : float64 x,y,z형 array. push_back으로 append하면 x,y,z 묶음으로 한 셋트씩 넣어짐.
// std_msgs/ColorRGBA[] colors
// string text
// string mesh_resource
// bool mesh_use_embedded_materials
// */

using std::string;

#pragma pack(push,1)
struct DataJoint {
  double p[5][3];
  double DGestureInfo_f;
  double CGestureInfo_f;
  char DGestureInfo_s[20] = {0,};
  char CGestureInfo_s[20] = {0,};
};
#pragma pack(pop)

struct DataJoint d;

// for subscriber
void chatterCallback(const using_markers::Gesture& g) 
{
  for(int i = 0; i < 3; i++){
    d.p[0][i] = g.SpineBase[i];
    d.p[1][i] = g.ShoulderLeft[i];
    d.p[2][i] = g.ShoulderRight[i];
    d.p[3][i] = g.HandLeft[i];
    d.p[4][i] = g.HandRight[i];
  }
  
  d.DGestureInfo_f = g.DGestureInfo_f; d.CGestureInfo_f = g.CGestureInfo_f;
  strcpy(d.DGestureInfo_s, g.DGestureInfo_s.c_str()); //string을 char [] 로 변환하고, 대입한다. 
  strcpy(d.CGestureInfo_s, g.CGestureInfo_s.c_str()); 

  //print out gesture information on cmd 
  ROS_INFO("RECIEVED DATA IS: ");
  ROS_INFO("Head x: %f, y: %f, z: %f",  d.p[0][0],  d.p[0][1],  d.p[0][2]);
  ROS_INFO("ShoulderLeft x: %f, y: %f, z: %f",  d.p[1][0],  d.p[1][1],  d.p[1][2]);
  ROS_INFO("ShoulderRight x: %f, y: %f, z: %f", d.p[2][0], d.p[2][1], d.p[2][2]);
  ROS_INFO("HandLeft x: %f, y: %f, z: %f",      d.p[3][0], d.p[3][1], d.p[3][2]);
  ROS_INFO("HandRight x: %f, y: %f, z: %f",     d.p[4][0], d.p[2][1], d.p[4][2]);
  
  ROS_INFO("Discrete gesture is: %s  and %f",     d.DGestureInfo_s, d.DGestureInfo_f);
  ROS_INFO("Continuous gesture is: %s  and %f",     d.CGestureInfo_s, d.CGestureInfo_f);
// // %Tag(INIT)%
}

    visualization_msgs::Marker makePoints(visualization_msgs::Marker points)
    {
    points.header.frame_id = "/my_frame"; 
    points.header.stamp = ros::Time::now();
    points.lifetime = ros::Duration();

    // Set the namespace and id for this marker.  This serves to create a unique ID
    // !!Any marker sent with the same namespace and id will overwrite the old one
    points.ns = "points"; 
    points.id = 0;

    points.action = visualization_msgs::Marker::ADD;
    points.type = visualization_msgs::Marker::SPHERE_LIST;

    points.pose.orientation.w = 1.0; // 일단 해보고 어떤 의미가 있는지 보자. 

    points.scale.x = 0.05; 
    points.scale.y = 0.05;
    points.scale.z = 0.05;

    points.color.g = 1.0f;
    points.color.a = 1.0;

    //x,y,z값을 한번에 push_back해주기 위해서 float32[]로 받은 gesture의 joint값 [0]~[2]을 저장해준다. 
    geometry_msgs::Point p_spineBase, p_ShoulderLeft, p_ShoulderRight, p_HandLeft, p_HandRight;

    p_spineBase.x = -d.p[0][0];   p_spineBase.y = d.p[0][2];   p_spineBase.z = d.p[0][1];
    p_ShoulderLeft.x = -d.p[1][0];  p_ShoulderLeft.y = d.p[1][2];  p_ShoulderLeft.z = d.p[1][1];
    p_ShoulderRight.x = -d.p[2][0];  p_ShoulderRight.y = d.p[2][2];  p_ShoulderRight.z = d.p[2][1];
    p_HandLeft.x = -d.p[3][0];  p_HandLeft.y = d.p[3][2];  p_HandLeft.z = d.p[3][1];
    p_HandRight.x = -d.p[4][0];  p_HandRight.y = d.p[4][2];  p_HandRight.z = d.p[4][1];

    points.points.push_back(p_spineBase);
    points.points.push_back(p_ShoulderLeft);
    points.points.push_back(p_ShoulderRight);
    points.points.push_back(p_HandLeft);
    points.points.push_back(p_HandRight);

    return points;
    }

    visualization_msgs::MarkerArray connection(visualization_msgs::MarkerArray line)
    {
      line.markers.resize(3); // to save four strings

      //settings for each entry of array
      for(int i = 0; i<3; i++){
        line.markers[i].header.frame_id = "/my_frame"; 
        line.markers[i].header.stamp = ros::Time::now();
        line.markers[i].ns = "gesture_info"; 
        line.markers[i].id = i; 
        line.markers[i].lifetime = ros::Duration();

        line.markers[i].action = visualization_msgs::Marker::ADD;
        line.markers[i].type = visualization_msgs::Marker::CYLINDER;

        line.markers[i].scale.x = 0.1; 
        line.markers[i].scale.y = 0.1;
        line.markers[i].scale.z = 0.1;

        line.markers[i].color.g = 1.0f;
        line.markers[i].color.a = 1.0;
        line.markers[i].color.b = 0.4;
      }

      line.markers[0].pose.position.x = -0.5; line.markers[0].pose.position.y = 0.1; line.markers[0].pose.position.z = 0.2-0.1*i; line.markers[0].pose.orientation.w = 1.0; 
      line.markers[1].pose.position.x = -0.5; line.markers[1].pose.position.y = 0.1; line.markers[1].pose.position.z = 0.2-0.1*i; line.markers[1].pose.orientation.w = 1.0; 
      line.markers[2].pose.position.x = -0.5; line.markers[2].pose.position.y = 0.1; line.markers[2].pose.position.z = 0.2-0.1*i; line.markers[2].pose.orientation.w = 1.0; 

      return line;
    }

    visualization_msgs::MarkerArray makeTexts(visualization_msgs::MarkerArray info)
    {
      info.markers.resize(4); // to save four strings

      //settings for each entry of array
      for(int i = 0; i<4; i++){
        info.markers[i].header.frame_id = "/my_frame"; 
        info.markers[i].header.stamp = ros::Time::now();
        info.markers[i].ns = "gesture_info"; 
        info.markers[i].id = i; 
        info.markers[i].lifetime = ros::Duration();

        info.markers[i].action = visualization_msgs::Marker::ADD;
        info.markers[i].type = visualization_msgs::Marker::TEXT_VIEW_FACING;

        info.markers[i].pose.position.x = -0.5;
        info.markers[i].pose.position.y = 0.1;
        info.markers[i].pose.position.z = 0.2-0.1*i;
        info.markers[i].pose.orientation.w = 1.0; 

        info.markers[i].scale.x = 0.1; 
        info.markers[i].scale.y = 0.1;
        info.markers[i].scale.z = 0.1;

        info.markers[i].color.g = 1.0f;
        info.markers[i].color.a = 1.0;
        info.markers[i].color.b = 0.4;
      }
     
      info.markers[0].text = d.DGestureInfo_s; 
      info.markers[1].text = std::to_string(d.DGestureInfo_f);
      info.markers[2].text = d.CGestureInfo_s;
      info.markers[3].text = std::to_string(d.CGestureInfo_f);

      return info;
    }



int main( int argc, char** argv )
{
  ros::init(argc, argv, "skeleton_markers");
  
   ros::NodeHandle n;
   //ros::Rate r(1); // 1data/1sec DURATION있는데 필요할까?
   ros::Subscriber sub = n.subscribe("chatter", 1000, chatterCallback); //receive gesture msg
   ros::Publisher marker_pub1 = n.advertise<visualization_msgs::Marker>("visualization_marker", 1); //publish visualization msg
   ros::Publisher marker_pub2 = n.advertise<visualization_msgs::MarkerArray>("visualization_marker_info", 1); //publish visualization msg


  while (ros::ok())
  {
    memset(&d, 0, sizeof(d));
    //H: call callback function before calculating the publisher object: save the received msg(gesture) in g_skel.
    ros::spinOnce(); // 
    ros::Duration(1.0).sleep(); 

    visualization_msgs::Marker p;
    p = makePoints(p); //in-output이 같은데, 그러면 매개변수 하나로 해도 될거 같은데... 

    visualization_msgs::Marker l;
    
    // to visualize gesture info with text.
    visualization_msgs::MarkerArray info_arr;
    info_arr = makeTexts(info_arr);

// %Tag(PUBLISH)%
    while (marker_pub1.getNumSubscribers() < 1) //wait for the marker to have a subscriber
    {
      if (!ros::ok())
      {
        return 0;
      }
      ROS_WARN_ONCE("Please create a subscriber to the marker");
      sleep(1);
    }
    marker_pub1.publish(p);
    marker_pub2.publish(info_arr); 
  }
// %EndTag(PUBLISH)%

}
