//----------------------------------------------------
// file: TB_AddSound.h
//----------------------------------------------------

#ifndef _INCDEF_TB_AddSound_H_
#define _INCDEF_TB_AddSound_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_AddSound : public XRayEditorUIToolBox {

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_AddSound();
	virtual ~TB_AddSound();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_AddLight_H_*/

