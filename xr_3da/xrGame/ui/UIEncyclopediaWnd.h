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

#include "../encyclopedia_article_defs.h"

//////////////////////////////////////////////////////////////////////////

class CUIEncyclopediaWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;

	ref_str m_InfosHeaderStr;

	// Вложенная иерархия заголовков
	typedef xr_vector<ref_str>			GroupTree;
	typedef GroupTree::iterator			GroupTree_it;
	// Вставляем хвост списка в лист
	// Return:	Указатель на последний вставленный элемент
	CUITreeViewItem * AddTreeTail(GroupTree_it it, GroupTree &cont, CUITreeViewItem *pItemToIns);

	// Изменить размер статика по размеру текстуры
	void RescaleStatic(CUIStatic &s);
public:
	CUIEncyclopediaWnd();
	virtual ~CUIEncyclopediaWnd();

	virtual void Init();
	virtual bool OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action);
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData = NULL);
	virtual void Draw();

	// Добавлем 1 энциклопедиционную статью
	void AddArticle(const ref_str &ID);

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

	// Маска для изображения предмета текущей статьи
	CUIFrameWindow		UIImgMask;

	// Стуктура статьи
	struct Article
	{
		CUIStatic	image;
		ref_str		info;
	};

	// Хранилище статей
	typedef xr_vector<Article>				ArticlesDB;
	typedef xr_vector<Article>::iterator	ArticlesDB_it;
	ArticlesDB								m_ArticlesDB;

	Article				*m_pCurrArticle;
};

#endif	//UI_ENCYCLOPEDIA_WND_H_