//----------------------------------------------------
// file: MSC_ScaleLandscape.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_ScaleLandscape_H_
#define _INCDEF_MSC_ScaleLandscape_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_ScaleLandscape : public MouseCallback{
public:

	IVector m_Center;
	bool m_AxisEnable[3];

public:
	
	MouseCallback_ScaleLandscape();
	virtual ~MouseCallback_ScaleLandscape();

	virtual bool Start();
	virtual bool End();
	virtual void Move();

	virtual bool HiddenMode(){
		return true; }
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_ScaleLandscape_H_*/



