//----------------------------------------------------
// file: MSC_ScaleObject.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_ScaleObject_H_
#define _INCDEF_MSC_ScaleObject_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_ScaleObject : public MouseCallback{
public:

	bool m_Local;
	IVector m_Center;
	bool m_AxisEnable[3];

public:
	
	MouseCallback_ScaleObject();
	virtual ~MouseCallback_ScaleObject();

	virtual bool Start();
	virtual bool End();
	virtual void Move();

	virtual bool HiddenMode(){
		return true; }
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_ScaleObject_H_*/



