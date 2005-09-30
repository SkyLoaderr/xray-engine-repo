#pragma once

#include "UIDialogWnd.h"

class CUIStatic;
class CUIScrollView;
class CUI3tButton;

class CUIMapDesc : public CUIDialogWnd {
public:
	CUIMapDesc();
	~CUIMapDesc();

    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void *pData = 0);

private:
	void	Init();
	

	CUIStatic*		m_pCaption;
	CUIStatic*		m_pBackground;
	CUIStatic*		m_pFrame[3];
	CUIScrollView*	m_pTextDesc;
	CUIStatic*		m_pImage;
	CUI3tButton*	m_pBtnSpectator;
	CUI3tButton*	m_pBtnNext;
};