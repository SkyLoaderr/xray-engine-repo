//----------------------------------------------------
// file: TB_AddObject.h
//----------------------------------------------------

#ifndef _INCDEF_TB_AddObject_H_
#define _INCDEF_TB_AddObject_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_AddObject : public XRayEditorUIToolBox {

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_AddObject();
	virtual ~TB_AddObject();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_AddObject_H_*/

