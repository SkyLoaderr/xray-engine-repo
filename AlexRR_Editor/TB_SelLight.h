//----------------------------------------------------
// file: TB_SelLight.h
//----------------------------------------------------

#ifndef _INCDEF_TB_SelLight_H_
#define _INCDEF_TB_SelLight_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_SelLight : public XRayEditorUIToolBox {
protected:

	void RunEditor();

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_SelLight();
	virtual ~TB_SelLight();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_SelLight_H_*/

