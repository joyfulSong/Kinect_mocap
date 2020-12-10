#pragma once

/* defined joint types
        JointType_SpineBase	= 0,
        JointType_SpineMid	= 1,
        JointType_Neck	= 2,
        JointType_Head	= 3,
        JointType_ShoulderLeft	= 4,
        JointType_ElbowLeft	= 5,
        JointType_WristLeft	= 6,
        JointType_HandLeft	= 7,
        JointType_ShoulderRight	= 8,
        JointType_ElbowRight	= 9,
        JointType_WristRight	= 10,
        JointType_HandRight	= 11,
        JointType_HipLeft	= 12,
        JointType_KneeLeft	= 13,
        JointType_AnkleLeft	= 14,
        JointType_FootLeft	= 15,
        JointType_HipRight	= 16,
        JointType_KneeRight	= 17,
        JointType_AnkleRight	= 18,
        JointType_FootRight	= 19,
        JointType_SpineShoulder	= 20,
        JointType_HandTipLeft	= 21,
        JointType_ThumbLeft	= 22,
        JointType_HandTipRight	= 23,
        JointType_ThumbRight	= 24,
        JointType_Count	= ( JointType_ThumbRight + 1 )
        --> focus on upper body:
        JointType_Neck	= 2,
        JointType_Head	= 3,
        JointType_ShoulderLeft	= 4,
        JointType_ElbowLeft	= 5,
        JointType_HandLeft	= 7,
        JointType_ShoulderRight	= 8,
        JointType_ElbowRight	= 9,
        JointType_HandRight	= 11,
        */

#pragma pack(push,1) 
typedef struct Data {
    //skeleton
    bool type[25];
    double p[25][7]; 
};
#pragma pack(pop)