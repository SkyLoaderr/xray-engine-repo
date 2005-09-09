#pragma once
#include "UIDialogWnd.h"

class CUIListWnd;

class CUISpeechMenu : public CUIDialogWnd{
public:
	CUISpeechMenu(LPCSTR section_name);
	virtual	~CUISpeechMenu();
	virtual void Init			(float x, float y, float width, float height);
			void InitList		(LPCSTR section_name);
	virtual bool NeedCursor		()const {return  false;}
	virtual bool OnKeyboard		(int dik, EUIMessages keyboard_action);
	virtual bool StopAnyMove	() {return false;}
private:
	CUIListWnd*		m_pList;
	u32				m_text_color;
};