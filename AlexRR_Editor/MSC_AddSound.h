//----------------------------------------------------
// file: MSC_AddSound.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_AddSound_H_
#define _INCDEF_MSC_AddSound_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_AddSound : public MouseCallback{
protected:
	bool AddHere();
public:
	
	MouseCallback_AddSound();
	virtual ~MouseCallback_AddSound();

	virtual bool Start();
	virtual bool End();
	virtual void Move();
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_AddSndPlane_H_*/



