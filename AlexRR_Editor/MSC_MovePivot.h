//----------------------------------------------------
// file: MSC_MovePivot.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_MovePivot_H_
#define _INCDEF_MSC_MovePivot_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_MovePivot : public MouseCallback{
public:

	IVector m_XVector;
	IVector m_YVector;

public:
	
	MouseCallback_MovePivot();
	virtual ~MouseCallback_MovePivot();

	virtual bool Start();
	virtual bool End();
	virtual void Move();

	virtual bool HiddenMode(){
		return true; }

	virtual bool Compare( int _Target, int _Action );
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_MovePivot_H_*/



