//----------------------------------------------------
// file: TB_MoveLandscape.h
//----------------------------------------------------

#ifndef _INCDEF_TB_MoveLandscape_H_
#define _INCDEF_TB_MoveLandscape_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_MoveLandscape : public XRayEditorUIToolBox {

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_MoveLandscape();
	virtual ~TB_MoveLandscape();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_MoveLandscape_H_*/

