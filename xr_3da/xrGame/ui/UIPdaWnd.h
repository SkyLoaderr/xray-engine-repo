// CUIPdaWnd.h:  диалог PDA
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIFrameWindow.h"
#include "UITabControl.h"
#include "UIPdaCommunication.h"
#include "UIMapWnd.h"
#include "UITaskWnd.h"
#include "UIDiaryWnd.h"

class CInventoryOwner;

///////////////////////////////////////
// Подложка и основные кнопки PDA
///////////////////////////////////////

class CUIPdaWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUIPdaWnd();
	virtual ~CUIPdaWnd();

	virtual void Init();

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

//	virtual void Draw();
//	virtual void Update();
		
	virtual void Show();
	
	// Специфичные для родительского окна PDA сообщения:
	// Смена точки центрирования карты
	typedef enum { PDA_MAP_SET_ACTIVE_POINT = 8010 } E_MESSAGE;
	// Список закладок
	typedef enum { TAB_PDACOMM, TAB_MAP, TAB_TASK, TAB_NEWS } E_PDA_TABS;
	// Принудительно сменить активную закладку
	void ChangeActiveTab(E_PDA_TABS tabNewTab);
	// Переключиться на карту и сфокусироваться на заданной точке
	void FocusOnMap(const int x, const int y, const int z);

protected:
	// Бэкграунд
	CUIFrameWindow UIMainPdaFrame;

	// Текущий активный диалог
	CUIDialogWnd *m_pActiveDialog;

	// Поддиалоги PDA
	CUIPdaCommunication	UIPdaCommunication;
	CUIMapWnd			UIMapWnd;
	CUITaskWnd			UITaskWnd;
	CUIDiaryWnd			UIDiaryWnd;

	//элементы декоративного интерфейса
	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;

//	//верхушка PDA для кнопочек переключения режима
//	CUIStatic			UIPDAHeader;

	// кнопки PDA
	CUITabControl		UITabControl;
};
