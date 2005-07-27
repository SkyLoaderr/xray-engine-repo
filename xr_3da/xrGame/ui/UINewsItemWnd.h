#pragma once

#include "UIWindow.h"
class CUIStatic;

class CUINewsItemWnd :public CUIWindow
{
typedef	CUIWindow		inherited;
CUIStatic*				m_UIText;

public:
					CUINewsItemWnd					();
	virtual			~CUINewsItemWnd					();
			void	Init							(LPCSTR xml_name, LPCSTR start_from);
			void	SetText							(const shared_str &text);
};