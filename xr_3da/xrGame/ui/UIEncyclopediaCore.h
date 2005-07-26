//=============================================================================
//  Filename:   CUIEncyclopediaCore.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ����� ��� ������������� ���� ���������������� ������������
//=============================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIWindow.h"
#include "UITreeViewItem.h"
#include "../encyclopedia_article.h"

//////////////////////////////////////////////////////////////////////////

class CEncyclopediaArticle;
class CUIScrollView;
class CUIFrameWindow;

class CUIEncyclopediaCore: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
					CUIEncyclopediaCore();
	void			Init			(CUIScrollView *infoList, CUIListWnd *idxList);
	// �������� � ��������� ������� ������������ ��� ������ ����������
	// ��������� ������ � ���������� ���� � ���
	shared_str		SetCurrentArtice(CUITreeViewItem *pTVItem);
	// �������� 1 ������������������ ������
	void			AddArticle		(ARTICLE_ID, bool bReaded);
	void			DeleteArticles	();
	bool			HasArticle		(ARTICLE_ID article_id);
	virtual void	SendMessage		(CUIWindow* pWnd, s16 msg, void* pData);
	virtual void	Show			(bool status);
	// Opens tree at item with specified value
//.	void			OpenTree		(ARTICLE_ID value);


	// ��������� ������
	typedef xr_vector<CEncyclopediaArticle*>			ArticlesDB;
	typedef xr_vector<CEncyclopediaArticle*>::iterator	ArticlesDB_it;

	CGameFont		*m_pTreeRootFont;
	u32				m_uTreeRootColor;
	CGameFont		*m_pTreeItemFont;
	u32				m_uTreeItemColor;

private:
//.	void			AdjustImagePos	(CUIStatic &s);
//.	void			OpenTree		(CUITreeViewItem *pItem);

	CUIListWnd*				pIdxList;
	CUIScrollView*			pInfoList;
	ArticlesDB				m_ArticlesDB;
	// ����� ��� ����������� �������� ������� ������
	CUIFrameWindow*			UIImgMask;
	
	// ������� ��������� ������
	CEncyclopediaArticle*	m_pCurrArticle;
	CUIStatic*				pItemImage;

	// ������� ������� ����� � �����������
    int						m_iCurrentInfoListPos;
};
