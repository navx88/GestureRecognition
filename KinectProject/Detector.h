#ifndef DATE_H
#define DATE_H

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <NuiApi.h>
#include <stdlib.h>

using namespace std;

class Detector
{
private:
	// for waves
	bool wavePosL;
	bool wavePosR;
	bool waveRight;
	bool waveLeft;
	bool wave;
	// for horizontal swipes
	bool swipePosL;
	bool swipeMidL;
	bool swipeL;
	bool swipePosR;
	bool swipeMidR;
	bool swipeR;

	// for up swipes
	float chanX;
	bool swipePosLU;
	bool swipePosRU;
	bool swipeMidU;
	bool swipeU;

	// for down swipes
	bool swipePosLD;
	bool swipePosRD;
	bool swipeMidD;
	bool swipeD;

	// for back swipes
	bool swipePosLB;
	bool swipePosRB;
	bool swipeMidB;
	bool swipeB;

	// for presses
	bool pressPosB;
	bool pressPosM;
	bool press;

	// tolerances, lower tolerance results in greater accuracy
	float tolX;
	float tolY;
	float tolZ;

	// skeleton data to be tracked
	Vector4 rHand;
	Vector4 rElbow;
	Vector4 rWrist;
	Vector4 rShoulder;
	Vector4 lHand;
	Vector4 lElbow;
	Vector4 lWrist;
	Vector4 lShoulder;
	Vector4 hip;
	Vector4 spine;
	Vector4 cShoulder;
public:
	
	// Constructor
	Detector();

	void excavate(NUI_SKELETON_DATA &bones);

	// Detection Functions
	bool detectWave();			// detect a wave gesture from either hand
	bool detectSwipeLeft();		// single swipe with left hand, moves things from left to right
	bool detectSwipeRight();	// single swipe with right hand, moves things from right to left
	bool detectSwipeUp();		// single swipe upwards with either hand, moves things from bottom to top
	bool detectSwipeDown();		// single swipe downwards with either hand, moves things from top to bottom
	bool detectSwipeBack();		// single swipe backwards with eitehr hand, does...something...
	bool detectPress();			// detect a hand moving forwards
	bool detectGrab();			// how we do this tho? :(

	// Grab Private Values
	Vector4 getlHand()		{ return lHand; }
	Vector4 getlElbow()		{ return lElbow; }
	Vector4 getlWrist()		{ return lWrist; }
	Vector4 getlShoulder()	{ return lShoulder; }
	Vector4 getrHand()		{ return rHand; }
	Vector4 getrElbow()		{ return rElbow; }
	Vector4 getrWrist()		{ return rWrist; }
	Vector4 getrShoulder()	{ return rShoulder; }
	Vector4 getHip()		{ return hip; }
	Vector4 getcShoulder()	{ return cShoulder; }
};

#endif