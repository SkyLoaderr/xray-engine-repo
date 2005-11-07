
#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"
#include "../encyclopedia_article_defs.h"
class CUINewsWnd;
class CUIFrameLineWnd;
class CUIFrameWindow;
class CUIAnimatedStatic;
class CUIStatic;
class CUITabControl;
class CUIScrollView;
class CUIListWnd;
class CEncyclopediaArticle;
//class CUIVideoPlayerWnd;

class CUIDiaryWnd: public CUIWindow, public CUIWndCallback
{
	typedef CUIWindow inherited;
	enum EDiaryFilter{
			eNews			= 0,
			eInfo,
			eJournal,
			eNone
	};
protected:
	EDiaryFilter		m_currFilter;

	CUINewsWnd*			m_UINewsWnd;
//	CUIVideoPlayerWnd*	m_videoWnd;

	CUIWindow*			m_UILeftWnd;
	CUIWindow*			m_UIRightWnd;
	CUIFrameWindow*		m_UILeftFrame;
	CUIFrameLineWnd*	m_UILeftHeader;
	CUIFrameWindow*		m_UIRightFrame;
	CUIFrameLineWnd*	m_UIRightHeader;
	CUIAnimatedStatic*	m_UIAnimation;
	CUITabControl*		m_FilterTab;
	CUIListWnd*			m_SrcListWnd;
	CUIScrollView*		m_DescrView;
	CGameFont*			m_pTreeRootFont;
	u32					m_uTreeRootColor;
	CGameFont*			m_pTreeItemFont;
	u32					m_uTreeItemColor;

	CUIStatic*			m_updatedSectionImage;

	typedef xr_vector<CEncyclopediaArticle*>			ArticlesDB;
	typedef xr_vector<CEncyclopediaArticle*>::iterator	ArticlesDB_it;
	ArticlesDB				m_ArticlesDB;

			void		OnFilterChanged			(CUIWindow*,void*);
			void		OnSrcListItemClicked	(CUIWindow*,void*);
			void		UnloadJournalTab		();
			void		LoadJournalTab			(ARTICLE_DATA::EArticleType _type);
			void		UnloadInfoTab			();
			void		LoadInfoTab				();
			void		UnloadNewsTab			();
			void		LoadNewsTab				();
			void		Reload					(EDiaryFilter new_filter);
public:
						CUIDiaryWnd				();
	virtual				~CUIDiaryWnd			();

	virtual void		SendMessage				(CUIWindow* pWnd, s16 msg, void* pData);
	virtual	void		Draw					();

			void		Init					();
			void		AddNews					();
			void		MarkNewsAsRead			(bool status);
	virtual void		Show					(bool status);

};

