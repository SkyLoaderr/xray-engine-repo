//----------------------------------------------------
// file: SndPlaneEditor.h
//----------------------------------------------------

#ifndef _INCDEF_SndPlaneEditor_H_
#define _INCDEF_SndPlaneEditor_H_

#include "SceneClassEditor.h"
#include "MultiObjCheck.h"

//----------------------------------------------------


class SndPlaneEditor : public SceneClassEditor {
protected:

	MultiObjFloat m_Dencity;

	virtual void GetObjectsInfo();
	virtual void ApplyObjectsInfo();
	virtual void DlgExtract();
	virtual void DlgSet();

protected:

	virtual void DlgInit( HWND hw );
	virtual void Command( WPARAM wp, LPARAM lp );

public:

	SndPlaneEditor();
	virtual ~SndPlaneEditor();
};


//----------------------------------------------------
#endif /*_INCDEF_LandscapeEditor_H_*/

