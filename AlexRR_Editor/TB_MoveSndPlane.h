//----------------------------------------------------
// file: TB_MoveSndPlane.h
//----------------------------------------------------

#ifndef _INCDEF_TB_MoveSndPlane_H_
#define _INCDEF_TB_MoveSndPlane_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_MoveSndPlane : public XRayEditorUIToolBox {

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_MoveSndPlane();
	virtual ~TB_MoveSndPlane();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_MoveSndPlane_H_*/

