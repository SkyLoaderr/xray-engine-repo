//----------------------------------------------------
// file: MSC_ScaleSndPlane.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_ScaleSndPlane_H_
#define _INCDEF_MSC_ScaleSndPlane_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_ScaleSndPlane : public MouseCallback{
public:

	bool m_Local;
	IVector m_Center;
	bool m_AxisEnable[3];

public:
	
	MouseCallback_ScaleSndPlane();
	virtual ~MouseCallback_ScaleSndPlane();

	virtual bool Start();
	virtual bool End();
	virtual void Move();

	virtual bool HiddenMode(){
		return true; }
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_ScaleSndPlane_H_*/



