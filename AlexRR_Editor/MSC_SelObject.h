//----------------------------------------------------
// file: MSC_SelObject.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_SelObject_H_
#define _INCDEF_MSC_SelObject_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_SelObject : public MouseCallback{
public:

	bool m_Box;
	bool m_Select;

protected:

	bool SelectHere();
	bool SelectHereRTL();

public:
	
	MouseCallback_SelObject();
	virtual ~MouseCallback_SelObject();

	virtual bool Start();
	virtual bool End();
	virtual void Move();
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_SelObject_H_*/



