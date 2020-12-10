#define _CRT_SECURE_NO_WARNINGS//block the compile error of fopen security warning

#define progma_once
#include <iostream>
#include <sstream>
#include <fstream> //for file r/w  / or use <stdio.h>
#include <string>

#define USE_GESTURE
#include "NtKinect.h"
#include "Skel_data.h"

void putText(cv::Mat& img, string s, cv::Point p) {
	cv::putText(img, s, p, cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2.0, CV_AA);
}

void doJob() {
	NtKinect kinect;
	vector<int> jointList{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,23}; // 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25
	Data data;

	HANDLE hEvent;
	DWORD WaitEvent;

	HANDLE hMutex;
	DWORD WaitResult;

	while (1) {
		hEvent = CreateEvent( 
			NULL, //unable the inherit
			TRUE, //TRUE - manual reset mode
			FALSE, //non-signaled state
			TEXT("hayEvent") // unnamed event
		);

		hMutex = CreateMutex(
			NULL,
			FALSE,
			TEXT("hayMutex")
		);

		bool flag_j = 0; 

		WaitResult = WaitForSingleObject(hMutex, INFINITE); //DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds), maximum waiting time until it gets signal in millisecond. 
		
		while (WaitResult == WAIT_OBJECT_0 && !(flag_j == 1 )) { 
			// WAIT_OBJECT_0: signaled to be used in other thread.
			
			flag_j = 0; 
			memset(&data, 0, sizeof(data)); //reset the structure instance to prevent the memory overlap. 
			kinect.setRGB();
			kinect.setSkeleton();

			int cnt = 0;
			for (auto person : kinect.skeleton) {
				
				for(int joint = 0; joint < person.size(); ++joint){
					// consider all joints of ONE person. 
					if (person[joint].first.TrackingState == TrackingState_NotTracked) continue;
					ColorSpacePoint cp;
					kinect.coordinateMapper->MapCameraPointToColorSpace(person[joint].first.Position, &cp);
					cv::rectangle(kinect.rgbImage, cv::Rect((int)cp.X - 5, (int)cp.Y - 5, 10, 10), cv::Scalar(0, 0, 255), 2);

					for (int idx = 0; idx < jointList.size(); ++idx) // check this joint is one of the list member
					{
						if (person[joint].first.JointType == jointList[idx])
						{
							data.type[jointList[idx]] = 1;

							data.p[jointList[idx]][0] = person[joint].first.Position.X;
							data.p[jointList[idx]][1] = person[joint].first.Position.Y;
							data.p[jointList[idx]][2] = person[joint].first.Position.Z;
							data.p[jointList[idx]][3] = person[joint].second.Orientation.x;
							data.p[jointList[idx]][4] = person[joint].second.Orientation.y;
							data.p[jointList[idx]][5] = person[joint].second.Orientation.z;
							data.p[jointList[idx]][6] = person[joint].second.Orientation.w;

							cnt++;
						}
			
					}
				}
				if (cnt == jointList.size()) flag_j = 1; // if detect all the needed joints
				
			}
			cv::imshow("rgb", kinect.rgbImage);
			auto key = cv::waitKey(1);
			if (key == 'q') break;
			
		}
		//check
		cout << "position value: x y z" << endl;
		for (int i = 0; i < jointList.size(); ++i) {
			int idx = jointList[i];
			if (data.type[idx] == 1) {
				cout << jointList[i] << ": " << data.p[idx][0] << " " << data.p[idx][1] << " " << data.p[idx][2] 
					<< data.p[idx][3] << data.p[idx][4] << data.p[idx][5] << data.p[idx][6] << '\n';
			}
		}

		FILE* fp = fopen("kinect_output.txt", "wb"); //open file as w/binary mode

		fwrite(&data, sizeof(data), 1, fp); // address of structure, write once.
		if (fp != NULL) {
			fclose(fp); //end of using the file. If it is failed to fopen, the returned value is NULL.
		}
		
		SetEvent(hEvent); // signal the event
		ReleaseMutex(hMutex); 
		WaitResult = WAIT_FAILED;
	}

	cv::destroyAllWindows();
}



int main(int argc, char** argv) {
	try {
		doJob();
	}
	catch (exception & ex) {
		cout << ex.what() << endl;
		string s;
		cin >> s;
	}
	return 0;
}

