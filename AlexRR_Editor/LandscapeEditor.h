//----------------------------------------------------
// file: LandscapeEditor.h
//----------------------------------------------------

#ifndef _INCDEF_LandscapeEditor_H_
#define _INCDEF_LandscapeEditor_H_

#include "ObjectOptionPack.h"
#include "SceneClassEditor.h"
#include "MultiObjCheck.h"

//----------------------------------------------------


class SLandscapeEditor : public SceneClassEditor {
protected:

	COLORREF m_CustColors[16];

	MultiObjColor m_Ambient;
	MultiObjColor m_Diffuse;
	MultiObjColor m_Specular;
	MultiObjColor m_Emission;
	MultiObjFloat m_Power;

	ObjectOptionPack m_Ops;

	virtual void GetObjectsInfo();
	virtual void ApplyObjectsInfo();
	virtual void DlgExtract();
	virtual void DlgSet();

protected:

	void ChangeColor( MultiObjColor& color );

protected:

	virtual void DlgInit( HWND hw );
	virtual void Command( WPARAM wp, LPARAM lp );
	virtual void Notify( WPARAM wp, LPARAM lp );
	virtual void DrawItem( WPARAM wp, LPARAM lp );

public:

	SLandscapeEditor();
	virtual ~SLandscapeEditor();
};


//----------------------------------------------------
#endif /*_INCDEF_LandscapeEditor_H_*/

