// CUIDiaryWnd.h:  ������� � ��� ��� � ��� �������
// 
//////////////////////////////////////////////////////////////////////

#pragma once
/*
#include "UIDialogWnd.h"
#include "UIListWnd.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIAnimatedStatic.h"
#include "UINewsWnd.h"
#include "UIJobsWnd.h"
#include "UIContracts.h"
#include "UIActorDiary.h"
#include "UIPdaAux.h"


class CUIDiaryWnd: public CUIWindow
{
	typedef CUIWindow inherited;
public:
	// Ctor and Dtor
	CUIDiaryWnd();
	virtual ~CUIDiaryWnd();

	virtual void		Init();
	virtual void		SendMessage(CUIWindow *pWnd, s16 msg, void* pData = NULL);
	virtual void		Show(bool status);

	// ������� ��������� �������� �������
	void				ArticleCaption(LPCSTR caption);

	// ������� �������������� ������ ���
	void				SetActiveSubdialog(EPdaSections section);
	void				OpenDiaryTree(ARTICLE_ID id);

	void				MarkNewsAsRead (bool status);
	//������ � �������� � ������
	void				ReloadArticles();

public:
	// �������� ���� �������� � ���� ����������
	CUINewsWnd			UINewsWnd;
	CUIJobsWnd			UIJobsWnd;
	CUIContractsWnd		UIContractsWnd;
	CUIActorDiaryWnd	UIActorDiaryWnd;
protected:
	// ������� �������� ���������
	CUIWindow			*m_pActiveSubdialog;
	// TreeView
	CUIListWnd			UITreeView;
	// �������� ��� TreeView
	CUIFrameWindow		UITreeViewBg;
	CUIFrameLineWnd		UITreeViewHeader;
	// ������������ ������
	CUIAnimatedStatic	UIAnimation;

	// �������� ��� �������� ����
	CUIFrameWindow		UIFrameWnd;
	// ����� ��������� ����
	CUIFrameLineWnd		UIFrameWndHeader;
	// ��������� ������������� � �������� ���� �������� �������
	CUIStatic			UIArticleCaption;

	// ����� � ������ ��������� ������
	CGameFont			*m_pTreeRootFont;
	u32					m_uTreeRootColor;
	CGameFont			*m_pTreeItemFont;
	u32					m_uTreeItemColor;
	u32					m_uUnreadColor;

	// ����� ������������� �����
	CUIStatic			*m_pLeftHorisontalLine;

	//������ ���������, ������� ���� ������ �� ���������
//	CUITreeViewItem*	m_pContractsTreeItem;
	// ������ ����� ��������
//	CUITreeViewItem		*m_pActorDiaryRoot;
	// ��������� �� ������� � ���������
	CUITreeViewItem		*m_pJobsRoot;
	CUITreeViewItem		*m_pActiveJobs;
	CUITreeViewItem		*m_pNews;

	// �������������� TreeView
	void				InitTreeView();
	//������������� ����� �������
	void				InitDiary();

	//id ��������, ��� ������ ����������
	u16					m_TraderID;

	//������������� ���������� � �������� � �������
	void				SetContractTrader();
};
*/
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

