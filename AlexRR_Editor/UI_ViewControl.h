//----------------------------------------------------
// file: UI_ViewControl.h
//----------------------------------------------------

#ifndef _INCDEF_UI_ViewControl_H_
#define _INCDEF_UI_ViewControl_H_

//----------------------------------------------------

class UI_ViewControl {
protected:
	
	friend LRESULT CALLBACK VPControlWndProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp);

	bool m_Captured;
	bool m_Alternate;
	bool m_Accelerated;

	HWND m_Window;
	WNDPROC m_OriginalWndProc;

	POINT m_SaveCursor;
	POINT m_MoveStart;
	POINT m_RelMove;

public:

	virtual void Create( HWND _Window );
	virtual void Restore();

	UI_ViewControl();
	virtual ~UI_ViewControl();

	virtual void MouseStartMove(){};
	virtual void MouseEndMove(){};
	virtual void MouseMove(){};
};

//----------------------------------------------------

#endif /*_INCDEF_UI_ViewControl_H_*/

