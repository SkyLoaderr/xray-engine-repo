
#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"

class CUINewsWnd;
class CUIFrameLineWnd;
class CUIFrameWindow;
class CUIAnimatedStatic;
class CUIStatic;
class CUITabControl;

class CUIDiaryWnd: public CUIWindow, public CUIWndCallback
{
	typedef CUIWindow inherited;
	enum EDiaryFilter{
			eGame			= 0,
			eInfo,
			eNews,
			eNone
	};
protected:
	EDiaryFilter		m_currFilter;

	CUINewsWnd*			m_UINewsWnd;

	CUIWindow*			m_UILeftWnd;
	CUIWindow*			m_UIRightWnd;
	CUIFrameWindow*		m_UILeftFrame;
	CUIFrameLineWnd*	m_UILeftHeader;
	CUIFrameWindow*		m_UIRightFrame;
	CUIFrameLineWnd*	m_UIRightHeader;
	CUIAnimatedStatic*	m_UIAnimation;
	CUITabControl*		m_FilterTab;

			void		OnFilterChanged	(CUIWindow*,void*);
			void		UnloadGameTab	();
			void		LoadGameTab		();
			void		UnloadInfoTab	();
			void		LoadInfoTab		();
			void		UnloadNewsTab	();
			void		LoadNewsTab		();
			void		Reload			(EDiaryFilter new_filter);
public:
						CUIDiaryWnd		();
	virtual				~CUIDiaryWnd	();

	virtual void		SendMessage		(CUIWindow* pWnd, s16 msg, void* pData);

			void		Init			();
			void		AddNews			();
			void		MarkNewsAsRead	(bool status);
	virtual void		Show			(bool status);

};

