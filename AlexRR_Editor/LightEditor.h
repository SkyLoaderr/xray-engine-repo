//----------------------------------------------------
// file: LightEditor.h
//----------------------------------------------------

#ifndef _INCDEF_LightEditor_H_
#define _INCDEF_LightEditor_H_

#include "SceneClassEditor.h"
#include "MultiObjCheck.h"

//----------------------------------------------------


class LightEditor : public SceneClassEditor {
protected:

	COLORREF m_CustColors[16];

	//comm
	MultiObjCheck m_CtlCastShadows;
	MultiObjCheck m_CtlDirectional;

	// color params
	MultiObjColor m_CtlAmbient;
	MultiObjColor m_CtlDiffuse;
	MultiObjColor m_CtlSpecular;

	// omni
	MultiObjFloat m_CtlRange;
	MultiObjFloat m_CtlAttenuation0;
	MultiObjFloat m_CtlAttenuation1;
	MultiObjFloat m_CtlAttenuation2;

	virtual void GetObjectsInfo();
	virtual void ApplyObjectsInfo();
	virtual void DlgExtract();
	virtual void DlgSet();

protected:

	void ChangeColor( MultiObjColor& color );

protected:

	virtual void DlgInit( HWND hw );
	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void DrawItem( WPARAM wp, LPARAM lp );

public:

	LightEditor();
	virtual ~LightEditor();
};


//----------------------------------------------------
#endif /*_INCDEF_LightEditor_H_*/

