//----------------------------------------------------
// file: TB_SelObject.h
//----------------------------------------------------

#ifndef _INCDEF_TB_SelObject_H_
#define _INCDEF_TB_SelObject_H_

#include "TB_Main.h"

//----------------------------------------------------

class TB_SelObject : public XRayEditorUIToolBox {
protected:

	void TSelObject( bool flag );
	void TSelRefObject( bool flag );
	void TSelClassObject( bool flag );
	void RunEditor();
	void ResolveObjects();

public:

	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Init( HWND hw );

	TB_SelObject();
	virtual ~TB_SelObject();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_SelObject_H_*/

