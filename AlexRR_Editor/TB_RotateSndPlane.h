//----------------------------------------------------
// file: TB_RotateSndPlane.h
//----------------------------------------------------

#ifndef _INCDEF_TB_RotateSndPlane_H_
#define _INCDEF_TB_RotateSndPlane_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_RotateSndPlane : public XRayEditorUIToolBox {
protected:

	IVector m_RXv;
	IVector m_RYv;
	IVector m_RZv;

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_RotateSndPlane();
	virtual ~TB_RotateSndPlane();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_RotateObject_H_*/

