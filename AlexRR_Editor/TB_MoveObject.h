//----------------------------------------------------
// file: TB_MoveObject.h
//----------------------------------------------------

#ifndef _INCDEF_TB_MoveObject_H_
#define _INCDEF_TB_MoveObject_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_MoveObject : public XRayEditorUIToolBox {

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_MoveObject();
	virtual ~TB_MoveObject();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_MoveObject_H_*/

