//----------------------------------------------------
// file: TB_RotateObject.h
//----------------------------------------------------

#ifndef _INCDEF_TB_RotateObject_H_
#define _INCDEF_TB_RotateObject_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_RotateObject : public XRayEditorUIToolBox {
protected:

	IVector m_RXv;
	IVector m_RYv;
	IVector m_RZv;

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_RotateObject();
	virtual ~TB_RotateObject();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_RotateObject_H_*/

