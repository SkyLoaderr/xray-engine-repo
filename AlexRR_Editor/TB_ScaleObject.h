//----------------------------------------------------
// file: TB_ScaleObject.h
//----------------------------------------------------

#ifndef _INCDEF_TB_ScaleObject_H_
#define _INCDEF_TB_ScaleObject_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_ScaleObject : public XRayEditorUIToolBox {

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_ScaleObject();
	virtual ~TB_ScaleObject();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_ScaleObject_H_*/

