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

///////////////////////////////////////
// �������
///////////////////////////////////////

class CUIDiaryWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:
	// Ctor and Dtor
	CUIDiaryWnd();
	virtual ~CUIDiaryWnd();

	virtual void Init();
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData = NULL);
	virtual void Show();

	// �������� ����� �������
	void AddNewsItem(const char *sData);
	// ������� ��������� �������� �������
	void ArticleCaption(LPCSTR caption);


protected:
	// �������� ���� �������� � ���� ����������
	CUINewsWnd			UINewsWnd;
	CUIJobsWnd			UIJobsWnd;
	CUIContractsWnd		UIContractsWnd;

	// ������� �������� ���������
	CUIDialogWnd		*m_pActiveSubdialog;
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

	// �������������� TreeView
	void				InitTreeView();
	//������������� ����� �������
	void				InitDiary();

	//id ��������, ��� ������ ����������
	u32					m_TraderID;
};