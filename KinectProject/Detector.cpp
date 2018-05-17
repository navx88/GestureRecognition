#include "Detector.h"

// Detector Constructor
Detector::Detector()
{
	tolX = 0.1;
	tolY = 0.2;
	tolZ = 0.1;

	wavePosL = false;
	wavePosR = false;
	waveLeft = false;
	waveRight = false;
	wave = false;

	swipePosL = false;
	swipeMidL = false;
	swipeL = false;
	swipePosR = false;
	swipeMidR = false;
	swipeR = false;

	chanX = 0;
	swipePosLU = false;
	swipePosRU = false;
	swipeMidU = false;
	swipeU = false;

	swipePosLD = false;
	swipePosRD = false;
	swipeMidD = false;
	swipeD = false;

	swipePosLB = false;
	swipePosRB = false;
	swipeMidB = false;
	swipeB = false;

}

// excavate: grabs information from the skeleton data for processing, should be run at the start of every frame/cycle
void Detector::excavate(NUI_SKELETON_DATA &bones)
{
	lHand = bones.SkeletonPositions[NUI_SKELETON_POSITION_HAND_LEFT];
	lElbow = bones.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_LEFT];
	lWrist = bones.SkeletonPositions[NUI_SKELETON_POSITION_WRIST_LEFT];
	lShoulder = bones.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_LEFT];
	rHand = bones.SkeletonPositions[NUI_SKELETON_POSITION_HAND_RIGHT];
	rElbow = bones.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT];
	rWrist = bones.SkeletonPositions[NUI_SKELETON_POSITION_WRIST_RIGHT];
	rShoulder = bones.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT];
	hip = bones.SkeletonPositions[NUI_SKELETON_POSITION_HIP_CENTER];
	spine = bones.SkeletonPositions[NUI_SKELETON_POSITION_SPINE];
	cShoulder = bones.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER];

	if ( ( (lHand.y < hip.y) && (rHand.y < hip.y) ) || ( abs(lShoulder.z-rShoulder.z) > 0.1 ) )
	{
		wavePosL = false;
		wavePosR = false;
		waveLeft = false;
		waveRight = false;
		wave = false;

		swipePosL = false;
		swipeMidL = false;
		swipeL = false;
		swipePosR = false;
		swipeMidR = false;
		swipeR = false;

		swipePosLU = false;
		swipePosRU = false;
		swipeMidU = false;
		swipeU = false;

		swipePosLD = false;
		swipePosRD = false;
		swipeMidD = false;
		swipeD = false;

		swipePosLB = false;
		swipePosRB = false;
		swipeMidB = false;
		swipeB = false;
	}

}

// detectWave: detects a wave from either the left hand or the right hand (arm should be ~90 deg in the air for opt results)
bool Detector::detectWave()
{
	//cout << "entered detect left wave" << endl;

	if(wave) // if a wave has already occurred, reset
	{
		//cout << "wave true" << endl;
		waveLeft = false;
		waveRight = false;
		wave = false;
	}

	// Check if left arm is in position to wave
	if( abs(lElbow.y-lShoulder.y) <= tolY && abs(lElbow.z-lHand.z)<tolZ )
	{
		//cout << "ready to detect" << endl;
		wavePosL = true; // left hand is in waving position
		wavePosR = false; // force right wave pos to be false, the right hand should not interfere 
	}
	else if( abs(rElbow.y-rShoulder.y) <= tolY && abs(rElbow.z-rHand.z)<tolZ )
	{
		//cout << "ready to detect" << endl;
		wavePosL = false; // force left wave pos to be false, the left hand should not interfere 
		wavePosR = true; // right hand is in waving position
	}
	else
	{
		wave = false;
		wavePosL = false;
		wavePosR = false;
		waveLeft = false;
		waveRight = false;
	}

	// check if both arms are in wave position - this shouldn't be
	if(wavePosL && wavePosR)
	{
		wave = false;
		wavePosL = false;
		wavePosR = false;
		waveLeft = false;
		waveRight = false;
	}

	// Check wave positions (wave to left, wave to right) for left hand
	if(wavePosL)
	{
		if((lElbow.x-lHand.x)>(tolX/2))
			waveLeft = true; // a hand has been waved to the left

		if((lElbow.x-lHand.x)<(-1*tolX/2))
			waveRight = true; // a hand has been waved to the right

		if(waveLeft && waveRight)
			wave = true;
	}

	// Check wave positions (wave to left, wave to right) for right hand
	if(wavePosR)
	{
		if((rElbow.x-rHand.x)>(tolX/2))
			waveLeft = true; // a hand has been waved to the left

		if((rElbow.x-rHand.x)<(-1*tolX/2))
			waveRight = true; // a hand has been waved to the right

		if(waveLeft && waveRight)
			wave = true;
	}

	return wave;

}

// detectSwipeLeft: detects a single swipe using the left hand
bool Detector::detectSwipeLeft()
{
	//if( (swipeL) )
	//{
	//	swipePosL = false;
	//	swipeMidL = false;
	//	swipeL = false;
	//}

	//if( (lHand.y < (lShoulder.y-0.15)) && /*(lHand.y > hip.y) &&*/ (lHand.z < lShoulder.z) )
	//	swipePosL = true;
	//else
	//	swipePosL = false;

	//if( (swipePosL) && (lHand.x < lElbow.x) && (abs(lHand.x-lElbow.x) > 0.2) )
	//	swipeMidL = true;

	//if( (swipePosL) && (swipeMidL) && (lHand.x > lElbow.x) && (abs(lHand.x-lElbow.x) > 0.2) )
	//	swipeL = true;

	//return swipeL;

	if ((swipeL))
	{
		swipePosL = false;
		swipeMidL = false;
		swipeL = false;
	}

	if ( (lHand.y < lShoulder.y) && (lHand.y > hip.y) )
		swipePosL = true;
	else
	{
		swipePosL = false;
		swipeMidL = false;
		swipeL = false;
	}

	if ((swipePosL) && ( (lElbow.x-lHand.x) >= 0.15) )
		swipeMidL = true;

	if ((swipePosL) && (swipeMidL) && (abs(lHand.x - rShoulder.x) <= 0.1) )
		swipeL = true;

	return swipeL;
}

// detectSwipeRight: detects a single swipe using the right hand
bool Detector::detectSwipeRight()
{
	//if( (swipeR) )
	//{
	//	swipePosR = false;
	//	swipeMidR = false;
	//	swipeR = false;
	//}

	//if( (rHand.y < rShoulder.y) && /*(rHand.y > hip.y) &&*/ (rHand.z < rShoulder.z) )
	//	swipePosR = true;
	//else
	//	swipePosR = false;

	//if( (swipePosR) && (rHand.x > rElbow.x) && (abs(rHand.x-rElbow.x) > 0.2) )
	//	swipeMidR = true;

	//if( (swipePosR) && (swipeMidR) && (rHand.x < rElbow.x) && (abs(rHand.x-rElbow.x) > 0.2) )
	//	swipeR = true;

	//return swipeR;

	if ((swipeR))
	{
		swipePosR = false;
		swipeMidR = false;
		swipeR = false;
	}

	if ((rHand.y < rShoulder.y) && (rHand.y > hip.y))
		swipePosR = true;
	else
	{
		swipePosR = false;
		swipeMidR = false;
		swipeR = false;
	}

	if ((swipePosR) && ((rHand.x - rElbow.x) >= 0.15))
		swipeMidR = true;

	if ((swipePosR) && (swipeMidR) && (abs(rHand.x - lShoulder.x) <= 0.1))
		swipeR = true;

	return swipeR;
}

bool Detector::detectSwipeUp()
{
	//if ( swipeU || swipeR || swipeD ){
	//	swipePosRU = false;
	//	swipeMidU = false;
	//	swipeU = false;
	//}

	////Starting Position
	//if ( (abs(rHand.x - rElbow.x) < 0.1) && (rHand.y < hip.y) && (rHand.x < rShoulder.x) ) {
	//	swipePosRU = true;
	//}

	////Mid Position
	//if ( swipePosRU && (abs(rHand.y-rElbow.y) < 0.15 ) && (rHand.x < rShoulder.x) ){
	//	swipeMidU = true;
	//}

	//if ( swipeMidU && (abs(rHand.y-rShoulder.y) < 0.15) ){
	//	swipeU = true;
	//}

	//return swipeU;

	if (swipeU)
	{
		swipePosRU = false;
		swipeMidU = false;
		swipeU = false;
	}

	//Starting Position
	if (abs(rHand.x - rShoulder.x <= 0.15) && (abs(rHand.x-hip.x)>=0.15) )
	{
		swipePosRU = true;
	}
	else
	{
		swipePosRU = false;
		swipeMidU = false;
		swipeU = false;
	}

	//Mid Position
	if ((swipePosRU) && abs(rHand.y - rElbow.y) <= 0.05)
		swipeMidU = true;

	if ((swipePosRU) && (swipeMidU) && ((rHand.y - rShoulder.y)>=0.05))
		swipeU = true;

	return swipeU;
}

bool Detector::detectSwipeDown(){
	//if ( swipeU || swipeR || swipeD ){
	//	swipePosRD = false;
	//	swipeMidD = false;
	//	swipeD = false;
	//}

	////Starting Position
	//if ( (rHand.y > rShoulder.y) && (rHand.x > (rShoulder.x + 0.25))) {
	//	swipePosRD = true;
	//}
	//
	////Mid Position
	//if ( swipePosRD && (abs(rHand.y-rElbow.y) < 0.15)) {
	//	swipeMidD = true;
	//}

	////Final Position
	//if ( swipeMidD && (abs(rHand.y-(hip.y+0.3)) < 0.15)) {
	//	swipeD = true;
	//}

	//return swipeD; 

	if (swipeU || swipeR || swipeD) {
		swipePosRD = false;
		swipeMidD = false;
		swipeD = false;
	}

	//Starting Position
	if ( ( abs(rHand.y - rElbow.y) <= 0.1 ) && ( (rHand.x - rShoulder.x) > 0.15) ) {
		swipePosRD = true;
	}

	//Mid Position
	if (swipePosRD && ( rHand.y > rShoulder.y ) && abs(rHand.x - rShoulder.x <= 0.15)) {
		swipeMidD = true;
	}

	//Final Position
	if (swipeMidD && ( rHand.y < rElbow.y) && abs(rHand.x - rShoulder.x <= 0.15) ) {
		swipeD = true;
	}

	return swipeD;
}

bool Detector::detectSwipeBack(){
	//if ( swipeB || swipeL ){
	//	swipePosLB = false;
	//	swipePosRB = false;
	//	swipeMidB = false;
	//	swipeB = false;
	//}

	////Starting Position
	//if ( (abs(lHand.y-lElbow.y) < 0.10) && (abs(lElbow.y-lShoulder.y) < 0.10) && (lHand.x < (lElbow.x - 0.25)) ){
	//	swipePosLB = true;
	//}

	////Mid Position
	//if( swipePosLB && (abs(lHand.x-lElbow.x) < 0.10) ){
	//	swipeMidB = true;
	//	swipeB = true;
	//}

	//return swipeB;

	if (swipeB || swipeL) {
		swipePosLB = false;
		swipePosRB = false;
		swipeMidB = false;
		swipeB = false;
	}

	//Starting Position
	if ((abs(lHand.y - lElbow.y) < 0.15) && (abs(lElbow.y - lShoulder.y) < 0.15) ) 
	{
		swipePosLB = true;
	}
	else
	{
		swipePosLB = false;
		swipeMidB = false;
		swipeB = false;
	}

	//Mid Position
	if (swipePosLB && (lHand.x < lElbow.x) )
	{
		swipeMidB = true;
	}

	if (swipePosLB && swipeMidB && (abs(lHand.x - lShoulder.x) <= 0.1))
	{
		swipeB = true;
	}

	return swipeB;
}

// This HAS to be calibrated
bool Detector::detectPress() {
	if (swipeU || swipeR || swipeD || press) {
		pressPosB = false;
		pressPosM = false;
		press = false;
	}

	//Starting Position
	if ((rHand.y > spine.y) && (rHand.z < (spine.z - 0.25)) && (abs(rElbow.z - spine.z) < 0.1) ) {
		pressPosB = true;
	}

	//Mid Position
	if (pressPosB && (abs(rElbow.z - spine.z) < 0.1) && rHand.z < (spine.z - 0.1)) {
		pressPosM = true;
	}
	
	//End position
	if (pressPosM && (rElbow.z < (spine.z - 0.25)) && (rHand.z < rElbow.z)) {
		press = true;
	}

	return press;

	//if (swipeU || swipeR || swipeD || press) {
	//	pressPosB = false;
	//	pressPosM = false;
	//	press = false;
	//}

	////Starting Position (rShoulder.z - rHand.z >= 0.2) && && (abs(rHand.x-rShoulder.x)<=0.15) (rHand.y<rShoulder.y) && (rHand.y>rElbow.y) 
	//if ( (abs(rHand.x-cShoulder.x)<=0.15) && (abs(rHand.y - cShoulder.y) <= 0.15) && (cShoulder.z - rHand.z <= 0.3))
	//{
	//	//cout << "1";
	//	pressPosB = true; 
	//}
	////else
	////{
	////	pressPosB = false;
	////	pressPosM = false;
	////	press = false;
	////}

	////Mid Position
	//if ( pressPosB && (cShoulder.z - rHand.z >= 0.4)) {
	//	pressPosM = true;
	//}

	//////End position
	//if ( pressPosM && (cShoulder.z - rHand.z <= 0.4)) {
	//	press = true;
	//}

	//return press;
}