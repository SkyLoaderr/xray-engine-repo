//----------------------------------------------------
// file: MSC_MoveSndPlane.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_MoveSndPlane_H_
#define _INCDEF_MSC_MoveSndPlane_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_MoveSndPlane : public MouseCallback{
public:

	IVector m_XVector;
	IVector m_YVector;

	bool m_Snap;
	IVector m_MoveRemainder;

	bool m_AxisEnable[3];

public:
	
	MouseCallback_MoveSndPlane();
	virtual ~MouseCallback_MoveSndPlane();

	virtual bool Start();
	virtual bool End();
	virtual void Move();

	virtual bool HiddenMode(){
		return true; }
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_MoveSndPlane_H_*/



