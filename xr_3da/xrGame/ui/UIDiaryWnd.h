// CUIDiaryWnd.h:  ������� � ��� ��� � ��� �������
// 
//////////////////////////////////////////////////////////////////////

#pragma once

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

///////////////////////////////////////
// �������
///////////////////////////////////////

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

	// �������� ����� �������
	void				AddNewsItem(const char *sData);
	// ������� ��������� �������� �������
	void				ArticleCaption(LPCSTR caption);

	// ������� �������������� ������ ���
	void				SetActiveSubdialog(EPdaSections section);

protected:
	// �������� ���� �������� � ���� ����������
	CUINewsWnd			UINewsWnd;
	CUIJobsWnd			UIJobsWnd;
	CUIContractsWnd		UIContractsWnd;
	CUIActorDiaryWnd	UIActorDiaryWnd;

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

	// ����� ������������� �����
	CUIStatic			*m_pLeftHorisontalLine;

	//������ ���������, ������� ���� ������ �� ���������
	CUITreeViewItem*	m_pContractsTreeItem;
	// ������ ����� ��������
	CUITreeViewItem		*m_pActorDiaryRoot;
	// ��������� �� ������� � ���������
	CUITreeViewItem		*m_pJobsRoot;
	CUITreeViewItem		*m_pActiveJobs;

	// �������������� TreeView
	void				InitTreeView();
	//������������� ����� �������
	void				InitDiary();

	//id ��������, ��� ������ ����������
	u16					m_TraderID;

	//������������� ���������� � �������� � �������
	void				SetContractTrader();
};