//----------------------------------------------------
// file: MSC_SelLight.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_SelLight_H_
#define _INCDEF_MSC_SelLight_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_SelLight : public MouseCallback{
public:

	bool m_Box;

	bool m_Select;

protected:

	bool SelectHereRTL();

public:
	
	MouseCallback_SelLight();
	virtual ~MouseCallback_SelLight();

	virtual bool Start();
	virtual bool End();
	virtual void Move();
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_SelLight_H_*/



