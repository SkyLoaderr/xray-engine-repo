//----------------------------------------------------
// file: TB_AddLight.h
//----------------------------------------------------

#ifndef _INCDEF_TB_AddLight_H_
#define _INCDEF_TB_AddLight_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_AddLight : public XRayEditorUIToolBox {

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_AddLight();
	virtual ~TB_AddLight();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_AddLight_H_*/

