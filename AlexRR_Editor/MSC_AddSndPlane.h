//----------------------------------------------------
// file: MSC_AddSndPlane.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_AddSndPlane_H_
#define _INCDEF_MSC_AddSndPlane_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_AddSndPlane : public MouseCallback{
public:

	// meters
	int m_PlaneSize;

protected:

	bool AddHere();

public:
	
	MouseCallback_AddSndPlane();
	virtual ~MouseCallback_AddSndPlane();

	virtual bool Start();
	virtual bool End();
	virtual void Move();
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_AddSndPlane_H_*/



