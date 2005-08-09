#pragma once

#include "UILines.h"
#include "UIWindow.h"

class CUICustomEdit : public CUIWindow, public CUILinesOwner {
public:
	CUICustomEdit();
	virtual ~CUICustomEdit();
	// IUITextControl : public IUIFontControl{
	virtual void			SetFont(CGameFont* pFont)					{CUILinesOwner::SetFont(pFont);}
	virtual CGameFont*		GetFont()									{return CUILinesOwner::GetFont();}

	virtual void	Init			(float x, float y, float width, float height);
	virtual void	SendMessage		(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	virtual bool	OnMouse			(float x, float y, EUIMessages mouse_action);
	virtual bool	OnKeyboard		(int dik, EUIMessages keyboard_action);
	virtual void	OnFocusLost		();

	virtual void	Update			();
	virtual void	Draw			();

			void	CaptureFocus	(bool bCapture) { m_bInputFocus = bCapture; }
	virtual	void	SetText			(LPCSTR str);
	virtual const char* GetText();
	virtual void	Enable			(bool status);
			void	SetNumbersOnly	(bool status);
			void	SetFloatNumbers	(bool status);
			void	SetPasswordMode	(bool mode = true);
			void	SetDbClickMode	(bool mode = true) {m_bFocusByDbClick = mode;}
			
			void	SetLightAnim			(LPCSTR lanim);

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

	//DIK клавиши, кот. нажата и удерживается, 0 если такой нет
	int m_iKeyPressAndHold;
	bool m_bHoldWaitMode;

//	//положение текущее курсора при наборе текста
//	u32 m_iCursorPos;

//	static CUIColorAnimatorWrapper m_animation;
	u32	m_cursorColor;

	CLAItem*				m_lanim;
};