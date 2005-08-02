// CUIEditBox.h: ���� ������ � ����������
// 
//////////////////////////////////////////////////////////////////////

#pragma once
#include "UILabel.h"
#include "../script_export_space.h"
#include "UIOptionsItem.h"

//////////////////////////////////////////////////////////////////////////

class game_cl_GameState;
class CUIColorAnimatorWrapper;

//////////////////////////////////////////////////////////////////////////

class CUIEditBox : public CUILabel, public CUIOptionsItem {
public:
					CUIEditBox		();
	virtual			~CUIEditBox		();

	// CUIOptionsItem
	virtual void	SetCurrentValue();
	virtual void	SaveValue();

	virtual void	SendMessage		(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual bool	OnMouse			(float x, float y, EUIMessages mouse_action);
	virtual bool	OnKeyboard		(int dik, EUIMessages keyboard_action);
	virtual void	OnFocusLost		();

	virtual void	Update			();
	virtual void	Draw			();

	void			CaptureFocus	(bool bCapture) { m_bInputFocus = bCapture; }
	virtual	void	SetText			(LPCSTR str);
	virtual void	Enable			(bool status);
			void	SetNumbersOnly	(bool status);
			void	SetFloatNumbers	(bool status);
			void	SetPasswordMode	(bool mode = true);
			void	SetDbClickMode	(bool mode = true) {m_bFocusByDbClick = mode;}
protected:

	bool KeyPressed(int dik);
	bool KeyReleased(int dik);
	
	void AddLetter(char c);
	virtual void AddChar(char c);

	bool m_bInputFocus;
	bool m_bShift;

	bool m_bNumbersOnly;
	bool m_bFloatNumbers;
	bool m_bFocusByDbClick;

	//DIK �������, ���. ������ � ������������, 0 ���� ����� ���
	int m_iKeyPressAndHold;
	bool m_bHoldWaitMode;

	//��������� ������� ������� ��� ������ ������
	u32 m_iCursorPos;

	static CUIColorAnimatorWrapper* m_pAnimation;
	u32	m_cursorColor;

	DECLARE_SCRIPT_REGISTER_FUNCTION

};

add_to_type_list(CUIEditBox)
#undef script_type_list
#define script_type_list save_type_list(CUIEditBox)
