//----------------------------------------------------
// file: TB_SelSndPlane.h
//----------------------------------------------------

#ifndef _INCDEF_TB_SelSndPlane_H_
#define _INCDEF_TB_SelSndPlane_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_SelSndPlane : public XRayEditorUIToolBox {
protected:

	void RunEditor();

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_SelSndPlane();
	virtual ~TB_SelSndPlane();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_SelSndPlane_H_*/

