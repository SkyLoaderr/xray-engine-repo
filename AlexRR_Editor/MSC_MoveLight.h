//----------------------------------------------------
// file: MSC_MoveLight.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_MoveLight_H_
#define _INCDEF_MSC_MoveLight_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_MoveLight : public MouseCallback{
public:

	IVector m_XVector;
	IVector m_YVector;

	bool m_Snap;
	IVector m_MoveRemainder;

	bool m_AxisEnable[3];

public:
	
	MouseCallback_MoveLight();
	virtual ~MouseCallback_MoveLight();

	virtual bool Start();
	virtual bool End();
	virtual void Move();

	virtual bool HiddenMode(){
		return true; }
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_MoveLight_H_*/



