//=============================================================================
//  Filename:   UIEncyclopediaWnd.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Encyclopedia window
//=============================================================================

#ifndef UI_ENCYCLOPEDIA_WND_H_
#define UI_ENCYCLOPEDIA_WND_H_

#include "UIDialogWnd.h"
#include "UIFrameLineWnd.h"
#include "UIFrameWindow.h"
#include "UIAnimatedStatic.h"
#include "UIListWnd.h"
#include "UITreeViewItem.h"

//////////////////////////////////////////////////////////////////////////

class CUIEncyclopediaWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;

	ref_str m_InfosHeaderStr;

	// ��������� �������� ����������
	typedef xr_vector<ref_str>			GroupTree;
	typedef GroupTree::iterator			GroupTree_it;
	// ��������� ����� ������ � ����
	// Return:	��������� �� ��������� ����������� �������
	CUITreeViewItem * AddTreeTail(GroupTree_it it, GroupTree &cont, CUITreeViewItem *pItemToIns);
public:
	CUIEncyclopediaWnd();
	virtual ~CUIEncyclopediaWnd();

	virtual void Init();
	virtual bool OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action);

	// �������� 1 ������������������ ������
	void AddArticle(const ref_str &ID);

protected:
	// �������� ������������ ����������
	CUIFrameWindow		UIEncyclopediaIdxBkg;
	CUIFrameWindow		UIEncyclopediaInfoBkg;
	CUIFrameLineWnd		UIEncyclopediaIdxHeader;
	CUIFrameLineWnd		UIEncyclopediaInfoHeader;
	CUIAnimatedStatic	UIAnimation;

	// Data lists
	CUIListWnd			UIIdxList;
	CUIListWnd			UIInfoList;

	// �������� ������
	struct Article
	{
		CUIStatic	image;
		ref_str		info;
	};

	// ��������� ������
	typedef xr_vector<Article>				ArticlesDB;
	typedef xr_vector<Article>::iterator	ArticlesDB_it;
	ArticlesDB								m_ArticlesDB;
};

#endif	//UI_ENCYCLOPEDIA_WND_H_