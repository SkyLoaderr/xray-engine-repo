//----------------------------------------------------
// file: MSC_SelLandscape.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_SelLandscape_H_
#define _INCDEF_MSC_SelLandscape_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_SelLandscape : public MouseCallback{
public:

	bool m_Box;
	bool m_ByGround;

	bool m_Select;

protected:

	bool SelectHere();
	bool SelectHereRTL();

public:
	
	MouseCallback_SelLandscape();
	virtual ~MouseCallback_SelLandscape();

	virtual bool Start();
	virtual bool End();
	virtual void Move();
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_SelLandscape_H_*/



