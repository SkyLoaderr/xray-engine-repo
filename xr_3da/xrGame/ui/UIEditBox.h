// CUIEditBox.h: ввод строки с клавиатуры
// 
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UIStatic.h"

class CUIEditBox : public CUIStatic
{
public:
	CUIEditBox(void);
	virtual ~CUIEditBox(void);

	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);
	virtual bool OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action);

protected:
	bool m_bInputFocus;
	
	void AddLetter(char c);
	void AddChar(char c);

	DEFINE_VECTOR	(char, STRING, STRING_IT);
	STRING m_sEdit;

	bool m_bShift;
};
