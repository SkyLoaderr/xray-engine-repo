//----------------------------------------------------
// file: TB_Main.h
//----------------------------------------------------

#ifndef _INCDEF_TB_Main_H_
#define _INCDEF_TB_Main_H_

//----------------------------------------------------

class XRayEditorUIToolBox {
protected:
	
	HINSTANCE m_Instance;
	HWND m_Window;
	HWND m_ChildDialog;

public:

	virtual void EraseBack( HDC hdc, RECT *rect );

	virtual void Command( WPARAM wp, LPARAM lp ){};
	virtual void DrawItem( WPARAM wp, LPARAM lp ){};
	virtual void Init( HWND hw ){};

	virtual void SelectionChanged(){};
	virtual void VisibilityChanged(){};

	virtual void Create( HINSTANCE _Instance, HWND _Window, int _ChildRes );
	virtual void Restore();

	XRayEditorUIToolBox();
	virtual ~XRayEditorUIToolBox();
};

//----------------------------------------------------

#endif /*_INCDEF_TB_Main_H_*/

