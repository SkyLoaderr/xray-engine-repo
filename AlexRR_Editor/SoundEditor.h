// SoundEditor.h: interface for the CSoundEditor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUNDEDITOR_H__E4FD8971_5CBB_4873_897B_D063A5DEBE96__INCLUDED_)
#define AFX_SOUNDEDITOR_H__E4FD8971_5CBB_4873_897B_D063A5DEBE96__INCLUDED_
#pragma once

#include "SceneClassEditor.h"
#include "MultiObjCheck.h"

class CSoundEditor : public SceneClassEditor  
{
protected:
	MultiObjFloat m_Range;

	virtual void GetObjectsInfo();
	virtual void ApplyObjectsInfo();
	virtual void DlgExtract();
	virtual void DlgSet();

protected:

	virtual void DlgInit( HWND hw );
	virtual void Command( WPARAM wp, LPARAM lp );

public:
	CSoundEditor();
	virtual ~CSoundEditor();

};

#endif // !defined(AFX_SOUNDEDITOR_H__E4FD8971_5CBB_4873_897B_D063A5DEBE96__INCLUDED_)
