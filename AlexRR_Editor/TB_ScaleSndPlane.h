//----------------------------------------------------
// file: TB_ScaleSndPlane.h
//----------------------------------------------------

#ifndef _INCDEF_TB_ScaleSndPlane_H_
#define _INCDEF_TB_ScaleSndPlane_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_ScaleSndPlane : public XRayEditorUIToolBox {

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_ScaleSndPlane();
	virtual ~TB_ScaleSndPlane();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_ScaleSndPlane_H_*/

