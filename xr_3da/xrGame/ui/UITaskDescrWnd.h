#pragma once
#include "UIWindow.h"

class CUIScrollView;
class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIXml;
class CEncyclopediaArticle;

class CUITaskDescrWnd:public CUIWindow
{
	typedef CUIWindow			inherited;
protected:
	CUIScrollView*				m_UITaskInfoWnd;
	CUIFrameWindow*				m_UIMainFrame;
	CUIFrameLineWnd*			m_UIMainHeader;

public:
				CUITaskDescrWnd				();
	virtual		~CUITaskDescrWnd			();
	void		Init						(CUIXml* doc, LPCSTR start_from);
	void		ClearAll					();
	void		AddArticle					(LPCSTR article);
	void		AddArticle					(CEncyclopediaArticle* article);
};