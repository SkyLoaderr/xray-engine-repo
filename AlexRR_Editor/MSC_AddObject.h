//----------------------------------------------------
// file: MSC_AddObject.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_AddObject_H_
#define _INCDEF_MSC_AddObject_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_AddObject : public MouseCallback{
public:

	int m_AddReference;
	int m_GridAlign;
	int m_QCenterAlign;

public:
	
	MouseCallback_AddObject();
	virtual ~MouseCallback_AddObject();

	virtual bool Start();
	virtual bool End();
	virtual void Move();
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_AddObject_H_*/


