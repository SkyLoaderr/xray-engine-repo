//----------------------------------------------------
// file: TB_SelLandscape.h
//----------------------------------------------------

#ifndef _INCDEF_TB_SelLandscape_H_
#define _INCDEF_TB_SelLandscape_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_SelLandscape : public XRayEditorUIToolBox {
protected:

	void ConnectSelection();
	void TSelLandscape( bool flag );
	void RunEditor();

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_SelLandscape();
	virtual ~TB_SelLandscape();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_SelLandscape_H_*/

