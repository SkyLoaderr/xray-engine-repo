#pragma once

#include "UIWindow.h"
class CUIScrollView;

class CUINewsWnd: public CUIWindow
{
	typedef CUIWindow inherited;
public:
					CUINewsWnd	();
	virtual			~CUINewsWnd	();

			void	Init		();
			void	Init		(LPCSTR xml_name, LPCSTR start_from);
	void			AddNews		();
	virtual void	Show		(bool status);

	CUIScrollView*	UIScrollWnd;


private:
	void			AddNewsItem	(const shared_str &text);
};
