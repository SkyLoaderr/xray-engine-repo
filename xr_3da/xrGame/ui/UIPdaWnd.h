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
#include "UIDiaryWnd.h"
#include "UIFrameLineWnd.h"
#include "UIEncyclopediaWnd.h"

class CInventoryOwner;

extern const char * const ALL_PDA_HEADER_PREFIX;
extern const char * const PDA_XML;

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

	virtual void Init		();

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

//	virtual void Draw();
	virtual void Update();
		
	virtual void Show();
	
	// Специфичные для родительского окна PDA сообщения:
	// Смена точки центрирования карты
//	typedef enum { PDA_MAP_SET_ACTIVE_POINT = 8010 } E_MESSAGE;
	// Список закладок
	typedef enum { TAB_EVENTS, TAB_COMM, TAB_MAP, TAB_NEWS } E_PDA_TABS;
	// Принудительно сменить активную закладку
	void ChangeActiveTab(E_PDA_TABS tabNewTab);
	// Переключиться на карту и сфокусироваться на заданной точке
	void FocusOnMap(const int x, const int y, const int z);
	void AddNewsItem(const char *sData);

protected:
	// Бэкграунд
	CUIStatic UIMainPdaFrame;

	// Текущий активный диалог
	CUIWindow			 *m_pActiveDialog;

	// Поддиалоги PDA
public:
	CUIPdaCommunication	UIPdaCommunication;

protected:
	CUIMapWnd			UIMapWnd;
	CUIEncyclopediaWnd	UIEncyclopediaWnd;
	CUIDiaryWnd			UIDiaryWnd;

	//элементы декоративного интерфейса
	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;
	CUIFrameLineWnd		UIMainButtonsBackground;
	CUIFrameLineWnd		UITimerBackground;

	// Кнопочка выключения ПДА
	CUIButton			UIOffButton;

	// кнопки PDA
	CUITabControl		UITabControl;

	// Установить игровое время
	void UpdateDateTime();
};
