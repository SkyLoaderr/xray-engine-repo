//=============================================================================
//  Filename:   CUIEncyclopediaCore.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ����� ��� ������������� ���� ���������������� ������������
//=============================================================================

#ifndef	UI_ENCYCLOPEDIA_CORE_H_
#define	UI_ENCYCLOPEDIA_CORE_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIDialogWnd.h"
#include "UIListWnd.h"
#include "UITreeViewItem.h"
#include "UIFrameWindow.h"
#include "../encyclopedia_article.h"

//////////////////////////////////////////////////////////////////////////

class CEncyclopediaArticle;

//////////////////////////////////////////////////////////////////////////

class CUIEncyclopediaCore: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUIEncyclopediaCore();

	void			Init(CUIListWnd *infoList, CUIListWnd *idxList);

	// �������� � ��������� ������� ������������ ��� ������ ����������
	// ��������� ������ � ���������� ���� � ���
	ref_str			SetCurrentArtice(CUITreeViewItem *pTVItem);
	// �������� 1 ������������������ ������
	void			AddArticle(ARTICLE_INDEX);
	void			DeleteArticles();
	virtual void	SendMessage(CUIWindow* pWnd, s16 msg, void* pData);
	virtual void	Show(bool status);


	// ��������� ������
	typedef xr_vector<CEncyclopediaArticle*>			ArticlesDB;
	typedef xr_vector<CEncyclopediaArticle*>::iterator	ArticlesDB_it;

	CGameFont		*m_pTreeRootFont;
	u32				m_uTreeRootColor;
	CGameFont		*m_pTreeItemFont;
	u32				m_uTreeItemColor;

private:
	void			AdjustImagePos(CUIStatic &s);

	CUIListWnd		*pInfoList, *pIdxList;
	ArticlesDB		m_ArticlesDB;
	// ����� ��� ����������� �������� ������� ������
	CUIFrameWindow	UIImgMask;
	
	// ������� ��������� ������
	CEncyclopediaArticle	*m_pCurrArticle;
	CUIStatic		*pItemImage;

	// ������� ������� ����� � �����������
    int				m_iCurrentInfoListPos;
};

#endif	//UI_ENCYCLOPEDIA_CORE_H_