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
#include "UIEncyclopediaCore.h"

#include "../encyclopedia_article_defs.h"

class CEncyclopediaArticle;

//////////////////////////////////////////////////////////////////////////

class CUIEncyclopediaWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;

	ref_str m_InfosHeaderStr;

	// Изменить размер статика по размеру текстуры
	void RescaleStatic(CUIStatic &s);
public:
	CUIEncyclopediaWnd();
	virtual ~CUIEncyclopediaWnd();

	virtual void Init();
	virtual void Show();
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData = NULL);
	virtual void Draw();

	void AddArticle(ARTICLE_INDEX);
	void DeleteArticles();

protected:

	// Элементы графического оформления
	CUIFrameWindow		UIEncyclopediaIdxBkg;
	CUIFrameWindow		UIEncyclopediaInfoBkg;
	CUIFrameLineWnd		UIEncyclopediaIdxHeader;
	CUIFrameLineWnd		UIEncyclopediaInfoHeader;
	CUIAnimatedStatic	UIAnimation;
	CUIStatic			UIArticleHeader;

	// Data lists
	CUIListWnd			UIIdxList;
	CUIListWnd			UIInfoList;
	CUIEncyclopediaCore	UIInfo;

	// Маска для изображения предмета текущей статьи
	CUIFrameWindow		UIImgMask;
	//положение картинки в энциклопедии
	int m_iItemX;
	int m_iItemY;

//	// Хранилище статей
//	typedef xr_vector<CEncyclopediaArticle*>			ArticlesDB;
//	typedef xr_vector<CEncyclopediaArticle*>::iterator	ArticlesDB_it;
//	ArticlesDB											m_ArticlesDB;

//	CEncyclopediaArticle							   *m_pCurrArticle;
};

#endif	//UI_ENCYCLOPEDIA_WND_H_