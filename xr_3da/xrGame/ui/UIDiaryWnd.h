// CUIDiaryWnd.h:  дневник и все что с ним связано
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

///////////////////////////////////////
// Дневник
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

	// Добавить новую новость
	void AddNewsItem(const char *sData);
	// Вывести заголовок текущего раздела
	void ArticleCaption(LPCSTR caption);

protected:
	// Дочерние окна входящие в окно информации
	CUINewsWnd			UINewsWnd;
	CUIJobsWnd			UIJobsWnd;
	CUIContractsWnd		UIContractsWnd;
	CUIActorDiaryWnd	UIActorDiaryWnd;

	// Текущий активный поддиалог
	CUIDialogWnd		*m_pActiveSubdialog;
	// TreeView
	CUIListWnd			UITreeView;
	// Подложка под TreeView
	CUIFrameWindow		UITreeViewBg;
	CUIFrameLineWnd		UITreeViewHeader;
	// Анимационная иконка
	CUIAnimatedStatic	UIAnimation;

	// Подложка под основное поле
	CUIFrameWindow		UIFrameWnd;
	// Хидер основного поля
	CUIFrameLineWnd		UIFrameWndHeader;
	// Заголовок отображаемого в основном поле текущего раздела
	CUIStatic			UIArticleCaption;

	// Цвета и шрифты элементов тривью
	CGameFont			*m_pTreeRootFont;
	u32					m_uTreeRootColor;
	CGameFont			*m_pTreeItemFont;
	u32					m_uTreeItemColor;

	// Левая горизонтальня линия
	CUIStatic			*m_pLeftHorisontalLine;

	//список торговцев, которые дают заказы на артефакты
	CUITreeViewItem*	m_pContractsTreeItem;
	// Список сттаей дневника
	CUITreeViewItem		*m_pActorDiaryRoot;

	// Инициализируем TreeView
	void				InitTreeView();
	//инициализация перед показом
	void				InitDiary();

	//id торговца, для списка контрактов
	u16					m_TraderID;

	//инициализация информации о торговце и заказах
	void				SetContractTrader();
};