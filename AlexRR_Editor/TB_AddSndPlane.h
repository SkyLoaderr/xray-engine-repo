//----------------------------------------------------
// file: TB_AddSndPlane.h
//----------------------------------------------------

#ifndef _INCDEF_TB_AddSndPlane_H_
#define _INCDEF_TB_AddSndPlane_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_AddSndPlane : public XRayEditorUIToolBox {
protected:

	void UpdateBSButtons( HWND hw );

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_AddSndPlane();
	virtual ~TB_AddSndPlane();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_AddSndPlane_H_*/

