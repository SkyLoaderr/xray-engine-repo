// CUIEditBox.h: ввод строки с клавиатуры
// 
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UILabel.h"
#include "../script_export_space.h"
#include "UIOptionsItem.h"

//////////////////////////////////////////////////////////////////////////

class game_cl_GameState;

//////////////////////////////////////////////////////////////////////////

class CUIEditBox : public CUILabel, public CUIOptionsItem {
public:
					CUIEditBox		();
	virtual			~CUIEditBox		();

	// CUIOptionsItem
	virtual void	SetDefaultValue();
	virtual void	SaveValue();

	virtual void	SendMessage		(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual void	OnMouse			(float x, float y, EUIMessages mouse_action);
	virtual bool	OnKeyboard		(int dik, EUIMessages keyboard_action);

	virtual void	Update			();
	virtual void	Draw			();

	void			CaptureFocus	(bool bCapture) { m_bInputFocus = bCapture; }
	virtual	void	SetText			(LPCSTR str);
protected:

	bool KeyPressed(int dik);
	bool KeyReleased(int dik);
	
	void AddLetter(char c);
	void AddChar(char c);

	bool m_bInputFocus;
	bool m_bShift;

	//DIK клавиши, кот. нажата и удерживается, 0 если такой нет
	int m_iKeyPressAndHold;
	bool m_bHoldWaitMode;

	//положение текущее курсора при наборе текста
	u32 m_iCursorPos;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIEditBox)
#undef script_type_list
#define script_type_list save_type_list(CUIEditBox)
