//----------------------------------------------------
// file: TB_SelLight.h
//----------------------------------------------------

#ifndef _INCDEF_TB_SelSound_H_
#define _INCDEF_TB_SelSound_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_SelSound : public XRayEditorUIToolBox {
protected:

	void RunEditor();

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_SelSound();
	virtual ~TB_SelSound();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_SelSound_H_*/

