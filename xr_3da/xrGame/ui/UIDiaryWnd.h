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
#include "UINotesWnd.h"

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
	// �������� ����� �������
	void AddNewsItem(const char *sData);
	// ������� ��������� �������� �������
	void ArticleCaption(LPCSTR caption);
protected:
	// �������� ���� �������� � ���� ����������
	CUINewsWnd			UINewsWnd;
	CUIJobsWnd			UIJobsWnd;
	CUINotesWnd			UINotesWnd;

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

	// �������������� TreeView
	void				InitTreeView();
};