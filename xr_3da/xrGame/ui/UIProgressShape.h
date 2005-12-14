#pragma once

#include "UIWindow.h"

class CUIStatic;

class CUIProgressShape : public CUIWindow {
public:
	CUIProgressShape();
	~CUIProgressShape();
CUIStatic*	InitTexture(LPCSTR texture);
CUIStatic*	InitBackground(LPCSTR texture);
	void	SetPos(int pos, int max);
	void	SetPos(float pos);
	void	SetTextVisible(bool b);

protected:
	CUIStatic*	m_pTexture;
	CUIStatic*	m_pBackground;
	bool		m_bText;
};