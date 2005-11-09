#pragma once
#include "UIWindow.h"

class CUIStatic;
class CUIFrameWindow;

class CUIButtonHint :public CUIWindow, public pureRender
{
	CUIWindow*			m_ownerWnd;

	CUIStatic*			m_text;
	CUIFrameWindow*		m_border;
	bool				m_enabledOnFrame;
public:
					CUIButtonHint	();
	virtual			~CUIButtonHint	();
	CUIWindow*		Owner			()	{return m_ownerWnd;}
	void			Discard			()	{m_ownerWnd=NULL;};
	virtual void	OnRender		();
	void			Draw_			()	{m_enabledOnFrame = true;};
	void			SetHintText		(CUIWindow* w, LPCSTR text);
};

extern CUIButtonHint* g_btnHint; 
