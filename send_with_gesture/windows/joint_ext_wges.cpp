#define _CRT_SECURE_NO_WARNINGS//block the compile error of fopen security warning

#define progma_once
#include <iostream>
#include <sstream>
#include <fstream> //for file r/w  / or use <stdio.h>
#include <string>

#define USE_GESTURE
#include "NtKinect.h"
#include "Skel_data_wges.h"

void putText(cv::Mat& img, string s, cv::Point p) {
	cv::putText(img, s, p, cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2.0, CV_AA);
}

void doJob() {
	NtKinect kinect;
	vector<int> jointList{ 2,3,4,5,7,8,9,11 };
	kinect.setGestureFile(L"SitAndStand.gbd");
	Data data;

	HANDLE hEvent;
	DWORD WaitEvent;

	HANDLE hMutex;
	DWORD WaitResult;

	while (1) {
		hEvent = CreateEvent( // #define progma_once 붙여서 헤더 여러번이라 중복되는거 때문에 인식 못하는거 방지.
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

		bool flag_j = 0, flag_d = 0, flag_c = 0;

		WaitResult = WaitForSingleObject(hMutex, INFINITE); //DWORD WaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds), maximum waiting time in millisecond. (신호상태가 될때까지 기다리나, 최대 시간도 부여함.)

		while (WaitResult == WAIT_OBJECT_0 && !(flag_j == 1 && flag_d == 1 && flag_c == 1)) {
			// WAIT_OBJECT_0: signaled to be used in other thread.
			// 셋다 1이 되어야지 false --> continuous, gesture는 flag가 되려면 임계값 넘어야 함.
			flag_j = 0, flag_d = 0, flag_c = 0;
			memset(&data, 0, sizeof(data)); //reset the structure instance to prevent the memory overlap. 
			kinect.setRGB();
			kinect.setSkeleton();

			int cnt = 0;
			for (auto person : kinect.skeleton) {
				// 하나의 id에 대해서만 넣기.
				// confidence value 높은것, 낮은것 선택할 기준 정해서 '벡터에' 넣기
				for (int joint = 0; joint < person.size(); ++joint) {
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
				
				kinect.setGesture();
				
				for (int i = 0; i < kinect.discreteGesture.size(); i++) {
					flag_d = 1;
					data.flag_d = kinect.discreteGesture.size();

					auto g = kinect.discreteGesture[i];
					putText(kinect.rgbImage, kinect.gesture2string(g.first) + " " + to_string(g.second), cv::Point(50, 30 + 30 * i));
					
					data.discrete_confi[i] = g.second; 
					strcpy(data.discrete_type[i], kinect.gesture2string(g.first).c_str());
				}

				for (int i = 0; i < kinect.continuousGesture.size(); i++) {
					flag_c = 1;
					data.flag_c = kinect.continuousGesture.size();
					
					auto g = kinect.continuousGesture[i];
					putText(kinect.rgbImage, kinect.gesture2string(g.first) + " " + to_string(g.second), cv::Point(500, 30 + 30 * i));
					
					data.continuous_confi[i] = g.second; 
					strcpy(data.continuous_type[i], kinect.gesture2string(g.first).c_str());
				}

			}
			cv::imshow("rgb", kinect.rgbImage);
			auto key = cv::waitKey(1);
			if (key == 'q') break;
		
			//std::cout << flag_j << ", " << flag_d << ", " << flag_c << std::endl;
		}
		//check
		cout << "pose value: x y z" << endl;
		for (int i = 0; i < jointList.size(); ++i) {
			int idx = jointList[i];
			if (data.type[idx] == 1) {
				cout << jointList[i] << ": " << data.p[idx][0] << " " << data.p[idx][1] << " " << data.p[idx][2]
					<< data.p[idx][3] << data.p[idx][4] << data.p[idx][5] << data.p[idx][6] << '\n';
			}
		}

		if (flag_d != 0) {
			std::cout << "discrete " << endl;
			for (int i = 0; i < data.flag_d; ++i) {
				cout << "discrete gesture type is " << data.discrete_type[i]
					<< "and confidence is " << data.discrete_confi[i] << endl;
			}
		}

		if (flag_c != 0) {
			std::cout << "continuous " << endl;
			for (int i = 0; i < data.flag_c; ++i) {
				cout << "continuous gesture type is " << data.continuous_type[i]
					<< "and confidence is " << data.continuous_confi[i] << endl;
			}
		}

		FILE* fp = fopen("kinect_output.txt", "wb"); //open file as w/binary mode

		// 데이터 출력부 작성
		fwrite(&data, sizeof(data), 1, fp); // address of structure, write once.
		if (fp != NULL) {
			fclose(fp); //end of using the file. If it is failed to fopen, the returned value is NULL.
		}
		
		////check
		//Data d;
		//FILE* fp1 = fopen("kinect_output.txt", "rb");
		//fread(&d, sizeof(d), 1, fp1);
		////check
		//cout << "position value: x y z" << endl;
		//for (int i = 0; i < 25; ++i) {
		//	if (d.type[i] == 1) {
		//		cout << i << ": " << d.p[i][0] << " " << d.p[i][1] << " " << d.p[i][2] << '\n';
		//	}
		//}
		//if (flag_d != 0) {
		//	std::cout << "discrete " << endl;
		//	for (int i = 0; i < d.discrete_type.size(); ++i) {
		//		cout << "discrete gesture type is " << d.discrete_type[i]
		//			<< "and confidence is " << d.discrete_confi[i] << endl;
		//	}
		//}

		//if (flag_c != 0) {
		//	std::cout << "continuous " << endl;
		//	for (int i = 0; i < d.continuous_type.size(); ++i) {
		//		cout << "continuous gesture type is " << d.continuous_type[i]
		//			<< "and confidence is " << d.continuous_confi[i] << endl;
		//	}
		//}
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

//string is a dynamically allocated array of chars that is managed by the object.