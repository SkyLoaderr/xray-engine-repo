//----------------------------------------------------
// file: MSC_RotateLandscape.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_RotateLandscape_H_
#define _INCDEF_MSC_RotateLandscape_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_RotateLandscape : public MouseCallback{
public:

	IVector m_RCenter;
	IVector m_RVector;

	bool m_ASnap;
	float m_ARemainder;

public:
	
	MouseCallback_RotateLandscape();
	virtual ~MouseCallback_RotateLandscape();

	virtual bool Start();
	virtual bool End();
	virtual void Move();

	virtual bool HiddenMode(){
		return true; }
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_RotateLandscape_H_*/



