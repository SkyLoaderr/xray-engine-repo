//----------------------------------------------------
// file: MSC_SelSndPlane.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_SelSndPlane_H_
#define _INCDEF_MSC_SelSndPlane_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_SelSndPlane : public MouseCallback{
public:

	bool m_Box;
	bool m_Select;

protected:

	bool SelectHereRTL();

public:
	
	MouseCallback_SelSndPlane();
	virtual ~MouseCallback_SelSndPlane();

	virtual bool Start();
	virtual bool End();
	virtual void Move();
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_SelObject_H_*/



