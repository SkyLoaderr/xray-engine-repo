//----------------------------------------------------
// file: MSC_RotateObject.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_RotateObject_H_
#define _INCDEF_MSC_RotateObject_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_RotateObject : public MouseCallback{
public:

	bool m_Local;
	IVector m_RCenter;
	IVector m_RVector;

	bool m_ASnap;
	float m_ARemainder;

public:
	
	MouseCallback_RotateObject();
	virtual ~MouseCallback_RotateObject();

	virtual bool Start();
	virtual bool End();
	virtual void Move();

	virtual bool HiddenMode(){
		return true; }
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_RotateObject_H_*/



