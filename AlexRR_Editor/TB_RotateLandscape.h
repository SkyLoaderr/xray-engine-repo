//----------------------------------------------------
// file: TB_RotateLandscape.h
//----------------------------------------------------

#ifndef _INCDEF_TB_RotateLandscape_H_
#define _INCDEF_TB_RotateLandscape_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_RotateLandscape : public XRayEditorUIToolBox {
protected:

	IVector m_RXv;
	IVector m_RYv;
	IVector m_RZv;

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_RotateLandscape();
	virtual ~TB_RotateLandscape();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_RotateLandscape_H_*/

