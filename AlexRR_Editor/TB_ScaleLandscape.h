//----------------------------------------------------
// file: TB_ScaleLandscape.h
//----------------------------------------------------

#ifndef _INCDEF_TB_ScaleLandscape_H_
#define _INCDEF_TB_ScaleLandscape_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_ScaleLandscape : public XRayEditorUIToolBox {

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_ScaleLandscape();
	virtual ~TB_ScaleLandscape();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_ScaleLandscape_H_*/

