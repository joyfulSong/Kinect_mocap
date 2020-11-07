#!/usr/bin/env python
# -*- coding: utf-8 -*-  

import sys
import copy
import rospy
import math
import moveit_commander
import moveit_msgs.msg
from skeleton_markers.msg import Skeleton
from visualization_msgs.msg import Marker
from visualization_msgs.msg import MarkerArray
from geometry_msgs.msg import Point

class SkeletonMarkers(object):
    def __init__(self): #인스턴스 생성시 자동으로 실행되는함수
        super(SkeletonMarkers, self).__init__()
        
        rospy.init_node('skeleton_markers')
        
        rospy.on_shutdown(self.shutdown)
        
        rospy.loginfo("Initializing Skeleton Markers Node...")
        
        #-- FOR MARKERS----------------------------------------------------
        self.fixed_frame = rospy.get_param('~fixed_frame', 'kinect_ir_frame')
        self.rate = rospy.get_param('~rate', 20)
        self.scale = rospy.get_param('~scale', 0.07)
        self.lifetime = rospy.get_param('~lifetime', 0) # 0 is forever
        self.ns = rospy.get_param('~ns', 'skeleton_markers')
        self.id = rospy.get_param('~id', 0)
        self.color_j = rospy.get_param('~color_j', {'r': 0.0, 'g': 1.0, 'b': 0.0, 'a': 1.0})
        self.color_l = rospy.get_param('~color_l', {'r': 0.0, 'g': 0.0, 'b': 1.0, 'a': 1.0})

        #rate = rospy.Rate(self.rate) #lifetime이 있으면 이건 필요할지 모르겠음.
        
        # Subscribe to the skeleton topic. 큐 사이즈.. 어케 정해줘야하지
        rospy.Subscriber('skeleton', Skeleton, self.skeleton_handler, queue_size=1000) #skeleton타입의 'skeleton'토픽을 subscribe.사용하는 callback함수.
        
        # Define a marker publisher. 
        self.marker_pub_joints = rospy.Publisher('skeleton_marker_joints', Marker) #(cpp에서 썼던것처럼, publish는 visualization msg로)
        self.marker_pub_links = rospy.Publisher('skeleton_marker_links', MarkerArray) 
        self.marker_pub_infos = rospy.Publisher('skeleton_marker_infos', MarkerArray) 

        # Initialize the marker points list. 
        # 1. Describe the joints as sphere
        self.joints = Marker()
        self.joints.ns = self.ns
        self.joints.id = self.id
        self.joints.type = Marker.SPHERE_LIST # lists of spheres with all the same properties except their positions
        self.joints.action = Marker.ADD
        self.joints.lifetime = rospy.Duration(self.lifetime)
        self.joints.scale.x = self.scale
        self.joints.scale.y = self.scale
        self.joints.color.r = self.color_j['r']
        self.joints.color.g = self.color_j['g']
        self.joints.color.b = self.color_j['b']
        self.joints.color.a = self.color_j['a']

        # 2. Describe the links as cylinder--line_list
        self.linksList = MarkerArray()
        self.links = Marker() # 1 마커들을 만들고 markerarray에 넣는것이 될지, 2 마커어레이의 마커에 접근해 넣어야할지. -- 1먼저
        self.links.ns = self.ns
        self.links.id = self.id 
        self.links.type = Marker.LINE_LIST
        self.links.action = Marker.ADD
        self.links.lifetime = rospy.Duration(self.lifetime)
        self.links.scale.x = 0.05
        self.links.scale.y = self.scale
        self.links.color.r = self.color_j['r'] #yaml 보고 고쳐
        self.links.color.g = self.color_j['g']
        self.links.color.b = self.color_j['b']
        self.links.color.a = self.color_j['a']

        # 3. Describe gesture information  
        self.infoList = MarkerArray()
        # self.info = Marker()
        # self.info.ns = self.ns
        # self.info.id = self.id
        # self.info.type = Marker.TEXT_VIEW_FACING
        # self.info.action = Marker.ADD
        # self.info.lifetime = rospy.Duration(self.lifetime)
        # self.info.scale.x = 0.1
        # self.info.scale.y = 0.1
        # self.info.scale.z = 0.1
        # #self.info.color.r = self.color_l['r']
        # self.info.color.g = 1.0
        # self.info.color.b = 0.4
        # self.info.color.a = 1.0 

        #-- END MARKERS----------------------------------------------------

        # -- FOR OBSTACLES----------------------------------------------------
        # initialize `moveit_commander` 
        # moveit_commander.roscpp_initialize(sys.argv)
       
        # instatiate a 'robot commander' object: provides robot's kinematic model and the robot's current joint states
        # robot = moveit_commander.RobotCommander()
        
        # Instantiate a `PlanningSceneInterface`_ object.  This provides a remote interface
        # for getting, setting, and updating the robot's internal understanding of the
        # surrounding world
        # scene = moveit_commander.PlanningSceneInterface()

        # Instantiate a `MoveGroupCommander`_ object.  This object is an interface to a planning group (group of joints)
        # 지금은 판다암이지만 다른 로봇 사용 가능
        # group_name = "panda_arm"
        # move_group = moveit_commander.MoveGroupCommander(group_name)

        # # Create a `DisplayTrajectory`_ ROS publisher which is used to display
        # # trajectories in Rviz:
        # display_trajectory_publisher = rospy.Publisher('/move_group/display_planned_path',
        #                                                moveit_msgs.msg.DisplayTrajectory,
        #                                                queue_size=20)
        # planning_frame = move_group.get_planning_frame()
        # eef_link = move_group.get_end_effector_link()
        # group_names = robot.get_group_names()

        # self.box_name = ''
        # self.robot = robot
        # self.scene = scene
        # self.move_group = move_group
        # self.display_trajectory_publisher = display_trajectory_publisher
        # self.planning_frame = planning_frame
        # self.eef_link = eef_link
        # self.group_names = group_names
        
        # print "============ Printing robot state"
        # print robot.get_current_state()
        # print ""

        # -- END OBSTACLES----------------------------------------------------

        while not rospy.is_shutdown():
            self.marker_pub_joints.publish(self.joints)
            self.marker_pub_links.publish(self.linksList)  
            self.marker_pub_infos.publish(self.infoList)                      
            #rospy.sleep(1) #rate.sleep()
    
    # #fuction for ensuring collision updates are receieved
    # def wait_for_state_update(self, box_is_known=False, box_is_attached=False, timeout=4):
    #     obs_name = self.obs_name
    #     scene = self.scene

    #     start = rospy.get_time()
    #     seconds = rospy.get_time()
    #     while (seconds - start < timeout) and not rospy.is_shutdown():
    #     # Test if the box is in attached objects
    #         attached_objects = scene.get_attached_objects([box_name])
    #         is_attached = len(attached_objects.keys()) > 0

    #         # Test if the box is in the scene.
    #         # Note that attaching the box will remove it from known_objects
    #         is_known = box_name in scene.get_known_object_names()

    #         # Test if we are in the expected state
    #         if (box_is_attached == is_attached) and (box_is_known == is_known):
    #             return True

    #         # Sleep so that we give other threads time on the processor
    #         rospy.sleep(0.1)
    #         seconds = rospy.get_time()

    #         # If we exited the while loop without returning then we timed out
    #         return False
    #         ## END_SUB_TUTORIAL

    # def add_obstacle(self, timeout=4):
    #     # Copy class variables to local variables to make the web tutorials more clear.
    #     # In practice, you should use the class variables directly unless you have a good
    #     # reason not to.
    #     obs_name = "one joint"
    #     scene = self.scene

    #     obs_pose = self.joints.points[0]
    #     obs_pose.header.frame_id = self.fixed_frame
        
    #     obs_pose.header.frame_id = "joint 1"
    #     obs_pose.pose.orientation.w = 1.0

    #     #this function add a box to the planning scene.(cylinder, mesh,plane,sphere)
    #     scene.add_box(obs_name, obs_pose, size=(0.1, 0.1, 0.1)) 

    #     return self.wait_for_state_update(box_is_known=True, timeout=timeout)

    # def remove_box(self, timeout=4):
    #     obs_name = self.obs_name
    #     scene = self.scene

    #     scene.remove_world_object(obs_name)

    #     return self.wait_for_state_update(box_is_attached=False, box_is_known=False, timeout=timeout)


    #define markers of joints, links and texts 
    def make_joints(self, msg):
        # msg check: 튜플은 불변형 리스트는 가변형/ 보내는 쪽에서 정리해서 쓸것만 보냈었음.
        # int32[] type #tuple
        # geometry_msgs/Vector3[] position #list
        # float64[] discrete_confi #tuple
        # string[] discrete_type #list
        # float64[] continuous_confi #tuple
        # string[] continuous_type #list

        for i in msg.type:
            print('joint ', i, ':')
        for a in range(len(msg.position)):
            print('x', msg.position[a].x, 'y', msg.position[a].y, 'z', msg.position[a].z)
        for j in range(len(msg.discrete_confi)):
            print(msg.discrete_type[j], ': ', msg.discrete_confi[j])
        for k in range(len(msg.continuous_confi)):
            print(msg.continuous_type[k], ': ', msg.continuous_confi[k])

        #joints
        for i in range(len(self.joints.points)):
            self.joints.points.pop()
        self.joints.header.frame_id = self.fixed_frame
        self.joints.header.stamp = rospy.Time.now()
        self.joints.points = list() #list형으로 변환

        for idx in range(len(msg.position)):   
            self.joints.id = idx        
            p = Point() #geometry_msgs point.
            p.x = msg.position[idx].x
            p.y = msg.position[idx].z
            p.z = msg.position[idx].y
            # 좌표축에 대해 조정: x는 y축 대칭으로, y와 z는 바꿈
            self.joints.points.append(p) #마지막에 추가. 리스트에 저장되어있는 조인트 순서대로 조인트 포지션이 입력될것임.

        # for value in self.joints.points:
        #     print(value)

    def make_links(self, msg):
        # links       0 1 2 3 4 5 6 7 8 <= append 한 순서에 따른 인덱스 
        # upper body: 0 2 3 4 5 7 8 9 11
        # 0 2 2 2 4 5 8 9
        # 2 3 4 8 5 7 9 11
        
        # 자동화하기엔 규칙성이 없어서 일단은 리스트로 만들어두고 진행
        joint1 = [0, 1, 1, 1, 3, 4, 6, 7]
        joint2 = [1, 2, 3, 6, 4, 5, 7, 8]
        
        for i in range(len(self.linksList.markers)):
            self.linksList.markers.pop()
        for i in range(len(self.links.points)):
            self.links.points.pop()
        
        self.links.header.frame_id = self.fixed_frame
        self.links.header.stamp = rospy.Time.now()

        for idx in range(len(joint1)):
            p1 = Point() 
            p2 = Point()
            self.links.id = idx

            x1 = self.joints.points[joint1[idx]].x
            x2 = self.joints.points[joint2[idx]].x
        #     # if x1 > x2:
        #     #     self.links.pose.position.x = (x1 - x2)/2
        #     # else: 
        #     #     self.links.pose.position.x = (x2 - x1)/2
            y1 = self.joints.points[joint1[idx]].y
            y2 = self.joints.points[joint2[idx]].y
        #     # if y1 > y2:
        #     #     self.links.pose.position.y = (y1 - y2)/2
        #     # else:
        #     #     self.links.pose.position.y = (y2 - y1)/2
            z1 = self.joints.points[joint1[idx]].z
            z2 = self.joints.points[joint2[idx]].z
        #     # if z1 > z2:
        #     #     self.links.pose.position.z = (z1 - z2)/2
        #     # else:
        #     #     self.links.pose.position.z = (z2 - z1)/2
        
            p1.x = x1
            p1.y = y1
            p1.z = z1
        
            p2.x = x2
            p2.y = y2
            p2.z = z2   
        
            self.links.points.append(p1)
            self.links.points.append(p2)
            self.linksList.markers.append(self.links)
            
            # self.links.pose.orientation.x = 1.54
            # self.links.pose.orientation.y = 1.54
            # self.links.pose.orientation.z = 1.54
            # self.links.scale.z = math.sqrt(math.pow((x1-x2),2)+math.pow((y1-y2),2)+math.pow((z1-z2),2)) # abs는 아예 부호를 없애버리므로 벡터의 의미가 없어짐
    
    def make_info(self, msg):
        # text: 의도는 아래로 주르르륵
        for i in range(len(self.infoList.markers)):
            self.infoList.markers.pop()
        # for i in range(len(self.info.points)):
        #     self.info.points.pop()

        #cnt2 = 0
        # for idx, val in enumerate(msg.discrete_type):
        info1 = Marker()
        info1.header.frame_id = self.fixed_frame 
        info1.header.stamp = rospy.Time.now()
        info1.type = Marker.TEXT_VIEW_FACING
        info1.action = Marker.ADD
        info1.lifetime = rospy.Duration(self.lifetime)
        info1.scale.z = 0.1
        info1.color.g = 1.0
        info1.color.b = 0.4
        info1.color.a = 1.0 
        
        info1.text = msg.discrete_type[0] #val 
        info1.id = 0 #idx
        
        info1.pose.position.x = 0.5 #+ 0.5*idx
        info1.pose.position.y = 0.0
        info1.pose.position.z = 0.5 
        self.infoList.markers.append(info1)
            #cnt2 = idx
            
        #for idx, val in enumerate(msg.discrete_confi):
        info2 = Marker()
        info2.header.frame_id = self.fixed_frame 
        info2.header.stamp = rospy.Time.now()
        info2.type = Marker.TEXT_VIEW_FACING
        info2.action = Marker.ADD
        info2.lifetime = rospy.Duration(self.lifetime)
        info2.scale.z = 0.1
        info2.color.g = 1.0
        info2.color.b = 0.4
        info2.color.a = 1.0 

        info2.text = str(msg.discrete_confi[0])#str(val) 
        info2.id = 1 #idx + (cnt2 + 1)
        
        info2.pose.position.x = 0.5 + 1 #*idx+(cnt2+1)
        info2.pose.position.y = 0.0
        info2.pose.position.z = 0.5
        self.infoList.markers.append(info2)
            #cnt2 = idx + (cnt2 + 1)
        
        #for idx, val in enumerate(msg.continuous_type):
        info3 = Marker()
        info3.header.frame_id = self.fixed_frame 
        info3.header.stamp = rospy.Time.now()
        info3.type = Marker.TEXT_VIEW_FACING
        info3.action = Marker.ADD
        info3.lifetime = rospy.Duration(self.lifetime)
        info3.scale.z = 0.1
        info3.color.g = 1.0
        info3.color.b = 0.4
        info3.color.a = 1.0 
        
        info3.text = msg.continuous_type[0] #val 
        info3.id = 2 #idx + (cnt2 + 1)
        
        info3.pose.position.x = 0.5 #+ 0.5*idx
        info3.pose.position.y = 0.0
        info3.pose.position.z = 0.3
        self.infoList.markers.append(info3)
        #cnt2 = idx + (cnt2 + 1)
            
        #for idx, val in enumerate(msg.continuous_confi):
        info4 = Marker()
        info4.header.frame_id = self.fixed_frame 
        info4.header.stamp = rospy.Time.now()
        info4.type = Marker.TEXT_VIEW_FACING
        info4.action = Marker.ADD
        info4.lifetime = rospy.Duration(self.lifetime)
        info4.scale.z = 0.1
        info4.color.g = 1.0
        info4.color.b = 0.4
        info4.color.a = 1.0 

        info4.text = str(msg.continuous_confi[0])#str(val) 
        info4.id = 3 #idx + (cnt2 + 1)
        
        info4.pose.position.x = 0.5 + 1#*idx+(cnt2+1)
        info4.pose.position.y = 0.0
        info4.pose.position.z = 0.3
        self.infoList.markers.append(info4)
            #cnt2 = idx + (cnt2 + 1)

    def skeleton_handler(self, msg):
         make_joints(self, msg)
         make_links(self, msg)
         make_info(self, msg)
        #  add_obstacle(self, timeout=4)
        #  remove_box(self, timeout=4)

    def shutdown(self):
        rospy.loginfo('Shutting down Skeleton Marker Node.')
        
if __name__ == '__main__':
    try:
        while not rospy.is_shutdown():
            SkeletonMarkers()
    except rospy.ROSInterruptException:
        pass

