//----------------------------------------------------
// file: MSC_AddLight.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_AddLight_H_
#define _INCDEF_MSC_AddLight_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_AddLight : public MouseCallback{
public:

	int m_Directional;
	int m_GridAlign;

public:
	
	MouseCallback_AddLight();
	virtual ~MouseCallback_AddLight();

	virtual bool Start();
	virtual bool End();
	virtual void Move();
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_AddLight_H_*/


