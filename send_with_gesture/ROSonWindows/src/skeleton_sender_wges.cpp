#define _CRT_SECURE_NO_WARNINGS
#include <windows.h> 
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "skeleton_sender_wges/Skeleton.h"
#include "Skel_data_wges.h"

#include <iostream>
#include <sstream>
#include <string> 

using namespace std;

int main(int argc, char** argv)
{
    ros::init(argc, argv, "skeleton_sender_wges"); // initialize ros, node name
    ros::NodeHandle n;
    ros::Publisher chatter_pub = n.advertise<skeleton_sender_wges::Skeleton>("skeleton", 1000);
    ROS_INFO("ros is just initialized");

    Data data;
    HANDLE hEvent;
    DWORD WaitEvent;

    HANDLE hMutex;
    DWORD WaitResult;
    skeleton_sender_wges::Skeleton s;

    while (ros::ok())
    {
        hMutex = OpenMutex(
            SYNCHRONIZE, //to use a mutex
            FALSE,
            TEXT("hayMutex")
        );
        ROS_INFO("opened a mutex");

        hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("hayEvent"));
        ROS_INFO("opened a event");

        WaitEvent = WaitForSingleObject(hEvent, INFINITE);

        ROS_INFO("done with waiting a event");

        if (WaitEvent == WAIT_OBJECT_0) {
            WaitResult = WaitForSingleObject(hMutex, INFINITE);
            ROS_INFO("done with waiting a mutex");

            ResetEvent(hEvent); // make event as non-signaled.
            CloseHandle(hEvent);
            ROS_INFO("close event handle");
            if (WaitResult == WAIT_OBJECT_0) ROS_INFO("got the mutex");
            else if (WaitResult == WAIT_ABANDONED) ROS_INFO("WAIT_ABANDONED");
            else if (WaitResult == WAIT_FAILED) ROS_INFO("WAIT_FAILED");
            else ROS_INFO("timeout");
        }

        while (WaitResult == WAIT_OBJECT_0) {
            memset(&data, 0, sizeof(data));
            FILE* fp = fopen("C:\\Users\\shy47\\source\\repos\\kinect_to_ros\\kinect_to_ros\\kinect_output.txt", "rb"); // open the file as r/binary mode
            fread(&data, sizeof(data), 1, fp); //read the file and save in the structure instance

            s.type.clear();
            s.position.clear();
            s.orientation.clear();
            s.discrete_confi.clear();
            s.discrete_type.clear();
            s.continuous_confi.clear();
            s.continuous_type.clear();

            for (int i = 0; i < 25; ++i) {
                if (data.type[i] == 1) {
                    s.type.push_back(i);
                    std::cout << i << " ";

                    geometry_msgs::Vector3 p; //double x, y, z
                    geometry_msgs::Quaternion o; //1110
                    p.x = data.p[i][0]; p.y = data.p[i][1]; p.z = data.p[i][2];
                    o.x = data.p[i][3]; o.y = data.p[i][4]; o.z = data.p[i][5]; o.w = data.p[i][6];
                   /* std::cout << p.x << p.y << p.z << endl
                        << o.x << o.y << o.z << endl;*/
                    s.position.push_back(p);
                    s.orientation.push_back(o);
                }

            }

            if (data.flag_d != 0) {
                //std::cout << "!!!!" << data.discrete_confi.size() << endl;
                for (int i = 0; i < data.flag_d; ++i) {
                    cout << "discrete gesture type is " << data.discrete_type[i]
                        << "and confidence is " << data.discrete_confi[i] << endl;
                    s.discrete_type.push_back(data.discrete_type[i]);
                    s.discrete_confi.push_back(data.discrete_confi[i]);
                }

            }

            if (data.flag_c != 0) {
                //std::cout << "!!!!" << data.continuous_confi.size() << endl;
                for (int i = 0; i < data.flag_c; ++i) {
                    cout << "continuous gesture type is " << data.continuous_type[i]
                        << "and confidence is " << data.continuous_confi[i] << endl;
                    s.continuous_type.push_back(data.continuous_type[i]);
                    s.continuous_confi.push_back(data.continuous_confi[i]);
                }
            }

            //check 
            for (int i = 0; i < s.position.size(); ++i) 
             {
                geometry_msgs::Vector3 p;
                geometry_msgs::Quaternion o;
                p = s.position[i];
                o = s.orientation[i];
                ROS_INFO("%d position : x: %lf, y: %lf, z: %lf", s.type[i], p.x, p.y, p.z);
                ROS_INFO("%d orientation : x: %lf, y: %lf, z: %lf, w: %lf", s.type[i], o.x, o.y, o.z, o.w);
             }
            for (int i = 0; i < s.discrete_confi.size(); ++i) {
                std::cout << "111" << endl;
                ROS_INFO("Discrete gesture is: %s and %lf", s.discrete_type[i].c_str(), s.discrete_confi[i]);
            }
            for (int i = 0; i < s.continuous_confi.size(); ++i) {
                std::cout << "222" << endl;
                ROS_INFO("Continuous gesture is: %s and %lf", s.continuous_type[i].c_str(), s.continuous_confi[i]);
            }
            
            chatter_pub.publish(s);

            ros::spinOnce();
            //ros::Duration(0.5).sleep();
            fclose(fp);

            WaitResult = WAIT_FAILED;
            ReleaseMutex(hMutex);
            CloseHandle(hMutex);
        }


    }


    return 0;
}