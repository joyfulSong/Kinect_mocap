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
	vector<int> jointList{ 0,2,3,4,5,7,8,9,11 }; //set for which joint should be detected. This is for upper body.
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

		bool flag_j = 0; // flag_d = 0, flag_c = 0;

		WaitResult = WaitForSingleObject(hMutex, INFINITE); //DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds), maximum waiting time in millisecond. (wait for mutex until it is signaled infinitely)
		
		while (WaitResult == WAIT_OBJECT_0 && !(flag_j == 1 )) { 
			// WAIT_OBJECT_0: signaled to be used in other thread.
			flag_j = 0;
			memset(&data, 0, sizeof(data)); //reset the structure instance to prevent the memory overlap. 
			kinect.setRGB();
			kinect.setSkeleton();

			cv::imshow("rgb", kinect.rgbImage);
			auto key = cv::waitKey(1);
			if (key == 'q') break;

			int cnt = 0;
			for (auto person : kinect.skeleton) {
				// do this only for one id.
				
				for (auto joint : person) {
					// consider all joints of ONE person. 
					if (joint.TrackingState == TrackingState_NotTracked) continue;
					ColorSpacePoint cp;
					kinect.coordinateMapper->MapCameraPointToColorSpace(joint.Position, &cp);
					cv::rectangle(kinect.rgbImage, cv::Rect((int)cp.X - 5, (int)cp.Y - 5, 10, 10), cv::Scalar(0, 0, 255), 2); //it shows the detected joint on the window.

					for (int idx = 0; idx < jointList.size(); ++idx) // check this joint if is one of the list member
					{
						if (joint.JointType == jointList[idx])
						{
							data.type[jointList[idx]] = 1;

							data.p[jointList[idx]][0] = joint.Position.X;
							data.p[jointList[idx]][1] = joint.Position.Y;
							data.p[jointList[idx]][2] = joint.Position.Z;
							//orientaion value of one joint is set in joint.Orientation.w, joint.Orientation.x, joint.Orientation.y, joint.Orientation.z

							cnt++;
						}
			
					}
				}
				if (cnt == jointList.size()) flag_j = 1; // if detect all the needed joints	
			}
			
		}
		//check
		//cout << "position value: x y z" << endl;
		//for (int i = 0; i < jointList.size(); ++i) {
		//	int idx = jointList[i];
		//	if (data.type[idx] == 1) {
		//		cout << jointList[i] << ": " << data.p[idx][0] << " " << data.p[idx][1] << " " << data.p[idx][2] << '\n';
		//	}
		//}

		FILE* fp = fopen("kinect_output.txt", "wb"); //open file as w/binary mode

		// write the data on the file.
		fwrite(&data, sizeof(data), 1, fp); // address of structure, write once.
		if (fp != NULL) {
			fclose(fp); //end of using the file. If it is failed to fopen, the returned value is NULL.
		}
		
		
		SetEvent(hEvent); // signal the event
		ReleaseMutex(hMutex); // 여기서 release하자마자 저쪽에서 인식함.
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
