//----------------------------------------------------
// file: TB_AddLandscape.h
//----------------------------------------------------

#ifndef _INCDEF_TB_AddLandscape_H_
#define _INCDEF_TB_AddLandscape_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_AddLandscape : public XRayEditorUIToolBox {
protected:
	void UpdateBSButtons( HWND hw );

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void DrawItem( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_AddLandscape();
	virtual ~TB_AddLandscape();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_AddLandscape_H_*/

