//----------------------------------------------------
// file: MSC_RotateSndPlane.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_RotateSndPlane_H_
#define _INCDEF_MSC_RotateSndPlane_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_RotateSndPlane : public MouseCallback{
public:

	bool m_Local;
	IVector m_RCenter;
	IVector m_RVector;

	bool m_ASnap;
	float m_ARemainder;

public:
	
	MouseCallback_RotateSndPlane();
	virtual ~MouseCallback_RotateSndPlane();

	virtual bool Start();
	virtual bool End();
	virtual void Move();

	virtual bool HiddenMode(){
		return true; }
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_RotateSndPlane_H_*/



