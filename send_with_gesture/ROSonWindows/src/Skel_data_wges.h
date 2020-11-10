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

        //typedef struct jointPosition {
        //    double x, y, z;
        //};
        //
        //typedef struct jointOrientation {
        //    double x, y, z, w;
        //};

#pragma pack(push,1) // 1바이트 단위로 정렬해라. 
typedef struct Data {
    //skeleton
    bool type[25];
    double p[25][7]; //벡터로 정의 한 뒤에 resize해주고 인덱스로 대입하는 방법도. 

    //gesture
    int flag_d = 0;
    double discrete_confi[10];
    char discrete_type[10][20] = { 0, };

    int flag_c = 0;
    double continuous_confi[10];
    char continuous_type[10][20] = { 0, };
    //
};
#pragma pack(pop)