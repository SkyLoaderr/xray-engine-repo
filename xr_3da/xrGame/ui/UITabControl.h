//////////////////////////////////////////////////////////////////////
// UITabControl.h: класс окна с закладками.
//////////////////////////////////////////////////////////////////////

#ifndef UI_TABCONTROL_H_
#define UI_TABCONTROL_H_

#pragma once

#include "uiwindow.h"
#include "UIButton.h"

DEF_VECTOR (TABS_VECTOR, CUIButton*)

class CUITabControl: public CUIWindow
{
public:
	// Ctor and Dtor
	CUITabControl();
	virtual ~CUITabControl();

	// Инициализация  из XML
	virtual void Init(int x, int y, int width, int height);

	// Сообщение отправляемое родительскому окну, когда произошла смена активной раскладки
	typedef enum{TAB_CHANGED} E_MESSAGE;

	// Добавление кнопки-закладки в список закладок контрола
	bool AddItem(const char *pItemName, const char *pTexName, int x, int y, int width, int height);
	bool AddItem(CUIButton *pButton);

	// Удаление элементов
	void RemoveItem(const u32 Index);
	void RemoveAll();

	// При нажатии на одну из кнопок то происходит переключение закладок.
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	int GetActiveIndex() { return m_iPushedIndex; }

	// Общее количество закладок
	const int GetTabsCount() const { return m_TabsArr.size(); }

	// Сделать новую такущую активную закладку
	void SetNewActiveTab(const int iNewTab);

protected:
	// Список кнопок - переключателей закладок
	TABS_VECTOR m_TabsArr;

	// Текущая нажатая кнопка. -1 - ни одна, 0 - первая, 1 - вторая, и т.д.
	int m_iPushedIndex;

	// Цвет надписи на неактивных элементах
	u32 m_cGlobalColor;

	// Цвет надписи на активном элементе
	u32 m_cActiveColor;
};

#endif