// KinectProject.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <Windows.h>
#include <NuiApi.h>
#include <stdlib.h>
#include <chrono>
#include "Detector.h"
#include <string>
#include <time.h>
#include <ctime>
#include <FaceTrackLib.h>
#include <MsHTML.h>


using namespace std;

LPTSTR mutexname = TEXT("SwipeTextAccess");
int gestureID = 1;
void writeToFile(ofstream& channel, string filename, string txt);
float calcDistance(Vector4 pt1, Vector4 pt2);
void updateImageFrame(NUI_IMAGE_FRAME& imageFrame, bool isDepthFrame);

// User Identification festure
bool userIdentification = false;

// use to collect data in csv
bool dataCollectionMode = true;
bool gestureRecording = true;
bool bodyRecording = false;

// tracking?
bool tracking = true;

int main()
{
	HANDLE colorStreamHandle;
	HANDLE depthStreamHandle;
	NUI_SKELETON_FRAME frame;
	Detector detector;
	ofstream myFile;
	ofstream csv;

	cout << "START" << endl;

	HRESULT hr;

	hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_COLOR);
	if (FAILED(hr))
	{
		cout << "Failed to intialize Kinect: " << endl;
		return false;
	}


	hr = NuiImageStreamOpen(
		NUI_IMAGE_TYPE_COLOR,
		NUI_IMAGE_RESOLUTION_640x480,
		0,
		2,
		NULL,
		&colorStreamHandle
	);
	if (FAILED(hr))
		cout << "Failed to open color stream." << endl;

	hr = NuiImageStreamOpen(
		NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
		NUI_IMAGE_RESOLUTION_640x480,
		0,
		2,
		NULL,
		&depthStreamHandle
	);
	if (FAILED(hr))
		cout << "Failed to open depth stream." << endl;
	

	HANDLE WINAPI mutex = CreateMutex(NULL, false, mutexname);

	if (mutex == NULL)
		cout << "MUTEX ERROR" << endl;

	ReleaseMutex(mutex);



	IFTFaceTracker* pFT = FTCreateFaceTracker();
	if (!pFT)
	{
		cout << "ERROR: facetracker not created." << endl;
	}

	// Video camera config with width, height, focal length in pixels
	// NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS focal length is computed for 640x480 resolution
	// If you use different resolutions, multiply this focal length by the scaling factor
	FT_CAMERA_CONFIG videoCameraConfig = { 640, 480, NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS };

	// Depth camera config with width, height, focal length in pixels
	// NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS focal length is computed for 320x240 resolution
	// If you use different resolutions, multiply this focal length by the scaling factor
	FT_CAMERA_CONFIG depthCameraConfig = { 640, 480, NUI_CAMERA_DEPTH_NOMINAL_FOCAL_LENGTH_IN_PIXELS };

	// Initialize the face tracker
	hr = pFT->Initialize(&videoCameraConfig, &depthCameraConfig, NULL, NULL);
	if (FAILED(hr))
	{
		cout << "ERROR: facetracker not initialized." << endl;
	}

	// Create a face tracking result interface
	IFTResult* pFTResult = NULL;
	hr = pFT->CreateFTResult(&pFTResult);
	if (FAILED(hr))
	{
		cout << "ERROR: facetracker result not created." << endl;
	}

	// prepare Image and SensorData for 640x480 RGB images
	IFTImage* pColorFrame = FTCreateImage();
	if (!pColorFrame)
	{
		cout << "Error: color frame not created." << endl;
	}

	// Prepare image interfaces that hold RGB and depth data
	IFTImage* pDepthFrame = FTCreateImage();
	if (!pColorFrame || !pDepthFrame)
	{
		cout << "Error: depth frame not created." << endl;
	}

	// Attach created interfaces to the RGB and depth buffers that are filled with
	// corresponding RGB and depth frame data from Kinect cameras
	pColorFrame->Attach(640, 480, (void*)NULL, FTIMAGEFORMAT_UINT8_R8G8B8, 640 * 3);
	pDepthFrame->Attach(640, 480, (void*)NULL, FTIMAGEFORMAT_UINT16_D13P3, 320 * 2);
	cout << pColorFrame->IsAttached() << endl;
	cout << pDepthFrame->IsAttached() << endl;
	// You can also use Allocate() method in which case IFTImage interfaces own their memory.
	// In this case use CopyTo() method to copy buffers

	FT_SENSOR_DATA sensorData;
	sensorData.pVideoFrame = pColorFrame;
	sensorData.pDepthFrame = pDepthFrame;
	sensorData.ZoomFactor = 1.0f;			// Not used must be 1.0
	sensorData.ViewOffset.x = 0;			// Not used must be (0,0)
	sensorData.ViewOffset.y = 0;

	bool isFaceTracked = false;


	if (dataCollectionMode)
	{
		csv.open("./data.csv");
		if (bodyRecording)
		{
			csv << "Hip.z,Hip-Shoulder,Shoulder-Shoulder,lBicep,rBicep,lForearm,rForearm\n";
		}
		else if (gestureRecording)
		{
			csv << "lHand.x,lHand.y,lHand.z,lElbow.x,lElbow.y,lElbow.z,lShoulder.x,lShoulder.y,lShoulder.z, rHand.x, rHand.y, rHand.z, rElbow.x, rElbow.y, rElbow.z, rShoulder.x, rShoulder.y, rShoulder.z\n";
			//int woah = 999; // Think Owen Wilson but not cause Kinect
		}
	}

	while (1) //For all of time
	{

		NuiSkeletonGetNextFrame(0, &frame); //Get a frame and stuff it into ourframe

		if (userIdentification)
		{

			const NUI_IMAGE_FRAME* colorFrame = NULL;
			hr = NuiImageStreamGetNextFrame(colorStreamHandle, 0, &colorFrame);
			if (SUCCEEDED(hr))
			{
				INuiFrameTexture* pTexture = colorFrame->pFrameTexture;
				NUI_LOCKED_RECT LockedRect;
				pTexture->LockRect(0, &LockedRect, NULL, 0);
				if (LockedRect.Pitch != 0)
				{
					memcpy(pColorFrame->GetBuffer(), PBYTE(LockedRect.pBits), min(pColorFrame->GetBufferSize(), UINT(pTexture->BufferLen())));
				}
				NuiImageStreamReleaseFrame(colorStreamHandle, colorFrame);
			}
			else if (FAILED(hr))
			{
				//cout << "Failed to capture color." << endl;
			}

			const NUI_IMAGE_FRAME* depthFrame = NULL;
			hr = NuiImageStreamGetNextFrame(depthStreamHandle, 0, &depthFrame);
			if (SUCCEEDED(hr))
			{

				INuiFrameTexture* pTexture = depthFrame->pFrameTexture;
				NUI_LOCKED_RECT LockedRect;
				pTexture->LockRect(0, &LockedRect, NULL, 0);
				if (LockedRect.Pitch != 0)
				{
					memcpy(pDepthFrame->GetBuffer(), PBYTE(LockedRect.pBits), min(pDepthFrame->GetBufferSize(), UINT(pTexture->BufferLen())));

				}
				NuiImageStreamReleaseFrame(depthStreamHandle, depthFrame);
			}
			else if (FAILED(hr))
			{
				//cout << "Failed to capture depth." << endl;
			}

		}

		

		// KINECT GESTURE DETECTION CODE BELOW, UNCOMMENT TO HAVE IT RUN
		int firstTracked = 0;

		for (int i = 0; i < 6; i++) //Six times, because the Kinect has space to track six people
		{
			detector.excavate(frame.SkeletonData[i]);
			if (frame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED && frame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER].z <= 1.5)
			{
				//cout << i;

				if (dataCollectionMode)
				{
					//cout << "LIVE" << endl;
					float distHipShoulder = calcDistance(detector.getHip(), detector.getcShoulder());
					float distShoulders = calcDistance(detector.getlShoulder(), detector.getrShoulder());

					// distance from elbow to shoulder, AKA the bicep
					float lBicep = calcDistance(detector.getlShoulder(), detector.getlElbow());
					float rBicep = calcDistance(detector.getrShoulder(), detector.getrElbow());

					// distance from hand to elbow, AKA the forearm
					float lForearm = calcDistance(detector.getlHand(), detector.getlElbow());
					float rForearm = calcDistance(detector.getrHand(), detector.getrElbow());
					
					if (bodyRecording)
					{
						//cout << to_string(distHipShoulder) + "," + to_string(distShoulders);
						csv <<
							to_string(detector.getHip().z) + "," +
							to_string(distHipShoulder) + "," +
							to_string(distShoulders) + "," +
							to_string(lBicep) + "," +
							to_string(rBicep) + "," +
							to_string(lForearm) + "," +
							to_string(rForearm) + "," +
							"\n";
					}

					if (gestureRecording)
					{
						//int blah = 0; // we'll add to the blahs later okay
						csv <<
							to_string(detector.getlHand().x) + "," +
							to_string(detector.getlHand().y) + "," +
							to_string(detector.getlHand().z) + "," +
							to_string(detector.getlElbow().x) + "," +
							to_string(detector.getlElbow().y) + "," +
							to_string(detector.getlElbow().z) + "," +
							to_string(detector.getlShoulder().x) + "," +
							to_string(detector.getlShoulder().y) + "," +
							to_string(detector.getlShoulder().z) + "," +
							to_string(detector.getrHand().x) + "," +
							to_string(detector.getrHand().y) + "," +
							to_string(detector.getrHand().z) + "," +
							to_string(detector.getrElbow().x) + "," +
							to_string(detector.getrElbow().y) + "," +
							to_string(detector.getrElbow().z) + "," +
							to_string(detector.getrShoulder().x) + "," +
							to_string(detector.getrShoulder().y) + "," +
							to_string(detector.getrShoulder().z) + "," +
							to_string(detector.getHip().x) + "," +
							to_string(detector.getHip().y) + "," +
							to_string(detector.getHip().z) + "," +
							"\n";
					}
				}

				

				if (detector.detectSwipeLeft())
				{
					//cout << "L" << endl;
					writeToFile(myFile, "./testVariables.txt", "L");
				}
				if (detector.detectSwipeRight())
				{
					//cout << "R" << endl;
					writeToFile(myFile, "./testVariables.txt", "R");
				}
				if (detector.detectSwipeUp())
				{
					//cout << "U" << endl;
					writeToFile(myFile, "./testVariables.txt", "U");
				}
				if (detector.detectSwipeDown())
				{
					//cout << "D" << endl;
					writeToFile(myFile, "./testVariables.txt", "D");
				}
				if (detector.detectSwipeBack())
				{
					//cout << "B" << endl;
					writeToFile(myFile, "./testVariables.txt", "B");
				}
				if (detector.detectPress())
				{
					//cout << "P" << endl;
					writeToFile(myFile, "./testVariables.txt", "P");
				}


			}

		}

		// BACK TO FACETRACKING

		if (userIdentification)
		{

			if (pFT && pFTResult)
			{
				if (!isFaceTracked)
				{
					hr = pFT->StartTracking(&sensorData, NULL, NULL, pFTResult);
					if (SUCCEEDED(hr) && SUCCEEDED(pFTResult->GetStatus()))
					{
						isFaceTracked = true;
						cout << "Tracking started." << endl;
					}
					//cout << "EW!" << endl;
				}
				else
				{
					hr = pFT->ContinueTracking(&sensorData, NULL, pFTResult);
					if (FAILED(hr) || FAILED(pFTResult->GetStatus()))
					{
						isFaceTracked = false;
						cout << "Tracking stopped." << endl;
					}
					//cout << ":I" << endl;
				}

			}

		}

	}

	csv.close();
	if (userIdentification)
	{
		pFTResult->Release();
		pColorFrame->Release();
		pDepthFrame->Release();
		pFT->Release();
	}
	NuiShutdown();
	return 0;
}

void writeToFile(ofstream& channel, string filename, string txt)
{
	HANDLE mutexTemp = OpenMutex(MUTEX_ALL_ACCESS, false, mutexname);
	if (mutexTemp == NULL)
		cout << "E" << endl;
	else
	{
		channel.open(filename);
		//cout << txt << ": " << gestureID << endl;
		if (txt == "L")
			cout << "LEFT: " << gestureID << endl;
		if (txt == "R")
			cout << "RIGHT: " << gestureID << endl;
		if (txt == "U")
			cout << "UP: " << gestureID << endl;
		if (txt == "D")
			cout << "DOWN: " << gestureID << endl;
		if (txt == "P")
			cout << "PRESS: " << gestureID << endl;
		if (txt == "B")
			cout << "BACK: " << gestureID << endl;
		channel << txt << " " << gestureID << endl;
		channel.close();
		ReleaseMutex(mutexTemp);
		gestureID++;
	}
}

float calcDistance(Vector4 pt1, Vector4 pt2)
{
	float dist = 0;
	float dx = pt1.x - pt2.x;
	float dy = pt1.y - pt2.y;
	float dz = pt1.z - pt2.z;
	dist = sqrt(dx*dx + dy * dy + dz * dz);
	return dist;
}

//void updateImageFrame(NUI_IMAGE_FRAME& imageFrame, bool isDepthFrame)
//{
//	INuiFrameTexture* nuiTexture = imageFrame.pFrameTexture;
//	NUI_LOCKED_RECT lockedRect;
//	nuiTexture->LockRect(0, &lockedRect, NULL, 0);
//	if (lockedRect.Pitch != NULL)
//	{
//		const BYTE* buffer = (const BYTE*)lockedRect.pBits;
//		for (int i = 0; i<480; ++i)
//		{
//			const BYTE* line = buffer + i * lockedRect.Pitch;
//			const USHORT* bufferWord = (const USHORT*)line;
//			for (int j = 0; j<640; ++j)
//			{
//				if (!isDepthFrame)
//				{
//					unsigned char* ptr = colorTexture->bits + 3 * (i * 640 + j);
//					*(ptr + 0) = line[4 * j + 2];
//					*(ptr + 1) = line[4 * j + 1];
//					*(ptr + 2) = line[4 * j + 0];
//				}
//				else
//				{
//					USHORT* ptr = (USHORT*)packedDepthTexture->bits + (i * 640 + j);
//					*ptr = bufferWord[j];
//				}
//			}
//		}
//
//		//TextureObject* tobj = (isDepthFrame ? packedDepthTexture : colorTexture);
//		//glBindTexture(GL_TEXTURE_2D, tobj->id);
//		//glTexImage2D(GL_TEXTURE_2D, 0, tobj->internalFormat, tobj->width, tobj->height,
//		//	0, tobj->imageFormat, GL_UNSIGNED_BYTE, tobj->bits);
//	}
//	nuiTexture->UnlockRect(0);
//}