//----------------------------------------------------
// file: MSC_SelLight.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_SelSound_H_
#define _INCDEF_MSC_SelSound_H_

#include "MSC_Main.h"

//----------------------------------------------------

class MouseCallback_SelSound : public MouseCallback{
public:

	bool m_Box;

	bool m_Select;

protected:

	bool SelectHereRTL();

public:
	
	MouseCallback_SelSound();
	virtual ~MouseCallback_SelSound();

	virtual bool Start();
	virtual bool End();
	virtual void Move();
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_SelSound_H_*/



