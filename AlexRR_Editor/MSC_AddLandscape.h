//----------------------------------------------------
// file: MSC_AddLandscape.h
//----------------------------------------------------

#ifndef _INCDEF_MSC_AddLandscape_H_
#define _INCDEF_MSC_AddLandscape_H_

#include "MSC_Main.h"

//----------------------------------------------------


class MouseCallback_AddLandscape : public MouseCallback{
public:

	int m_BrushSize;

protected:

	bool AddHere();
	void AddTo( int x, int z );

public:
	
	MouseCallback_AddLandscape();
	virtual ~MouseCallback_AddLandscape();

	virtual bool Start();
	virtual bool End();
	virtual void Move();
};


//----------------------------------------------------

#endif /*_INCDEF_MSC_AddLandscape_H_*/



