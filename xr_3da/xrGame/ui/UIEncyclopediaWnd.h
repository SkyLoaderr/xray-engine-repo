//=============================================================================
//  Filename:   UIEncyclopediaWnd.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Encyclopedia window
//=============================================================================

#ifndef UI_ENCYCLOPEDIA_WND_H_
#define UI_ENCYCLOPEDIA_WND_H_

#include "UIWindow.h"
/*
#include "UIFrameLineWnd.h"
#include "UIFrameWindow.h"
#include "UIAnimatedStatic.h"
#include "UIListWnd.h"
#include "UITreeViewItem.h"
#include "UIEncyclopediaCore.h"
*/
#include "../encyclopedia_article_defs.h"

class CEncyclopediaArticle;
class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
class CUIStatic;
class CUIListWnd;
class CUIEncyclopediaCore;
//////////////////////////////////////////////////////////////////////////

class CUIEncyclopediaWnd: public CUIWindow
{
private:
	typedef CUIWindow inherited;

	shared_str m_InfosHeaderStr;

	// �������� ������ ������� �� ������� ��������
	void RescaleStatic(CUIStatic &s);
public:
					CUIEncyclopediaWnd();
	virtual			~CUIEncyclopediaWnd();

	virtual void	Init();
	virtual void	Show(bool status);
	virtual void	SendMessage(CUIWindow *pWnd, s16 msg, void* pData = NULL);
	virtual void	Draw();

	void			AddArticle(ARTICLE_ID, bool bReaded);
	void			DeleteArticles();
	bool			HasArticle(ARTICLE_ID);

//.	void			OpenTree(ARTICLE_ID id) { UIInfo.OpenTree(id); }
// ������ �������� � ���� �������
//	CUIButton			UIBack;

	void			ReloadArticles	();
protected:
	u32					prevArticlesCount;
	// �������� ������������ ����������
	CUIFrameWindow*		UIEncyclopediaIdxBkg;
	CUIFrameWindow*		UIEncyclopediaInfoBkg;
	CUIFrameLineWnd*	UIEncyclopediaIdxHeader;
	CUIFrameLineWnd*	UIEncyclopediaInfoHeader;
	CUIAnimatedStatic*	UIAnimation;
	CUIStatic*			UIArticleHeader;

	// Data lists
	CUIListWnd*				UIIdxList;
	CUIListWnd*				UIInfoList;
	CUIEncyclopediaCore*	UIInfo;
};

#endif	//UI_ENCYCLOPEDIA_WND_H_