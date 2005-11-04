#pragma once
#include "UIWindow.h"

class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
class CUIStatic;
class CUICharacterInfo;
class CUIScrollView;

class CUIStalkersRankingWnd: public CUIWindow
{
	typedef CUIWindow inherited;

public:
	virtual void			Init				();
	virtual void			Show				(bool status);
	virtual void			SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	
protected:
	CUIFrameWindow*			UIInfoFrame;
	CUIFrameWindow*			UICharIconFrame;
	CUIFrameLineWnd*		UIInfoHeader;
	CUIFrameLineWnd*		UICharIconHeader;
	CUIAnimatedStatic*		UIAnimatedIcon;

	// информация о персонаже
	CUIWindow*				UICharacterWindow;
	CUICharacterInfo*		UICharacterInfo;
	// Список сталкеров
	CUIScrollView*			UIList;
};
