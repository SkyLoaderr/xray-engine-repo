//////////////////////////////////////////////////////////////////////
// UITabControl.h: класс окна с закладками.
//////////////////////////////////////////////////////////////////////

#ifndef UI_TABCONTROL_H_
#define UI_TABCONTROL_H_

#pragma once

#include "uiwindow.h"
#include "UIButton.h"
#include "../script_export_space.h"

DEF_VECTOR (TABS_VECTOR, CUIButton*)

class CUITabControl: public CUIWindow
{
	typedef CUIWindow inherited;
public:
	// Ctor and Dtor
	CUITabControl();
	virtual ~CUITabControl();

	// Инициализация  из XML
	virtual void Init(int x, int y, int width, int height);
	// обработка нажатий клавиш
	virtual bool OnKeyboard(int dik, EUIMessages keyboard_action);
	// обновление 
	virtual void Update();

	// Сообщение отправляемое родительскому окну, когда произошла смена активной раскладки
//	typedef enum{TAB_CHANGED = 8500} E_MESSAGE;

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
	const int GetTabsCount() const				{ return m_TabsArr.size(); }

	// Сделать новую такущую активную закладку
	void SetNewActiveTab(const int iNewTab);
	
	// Режим клавилатурных акселераторов (вкл/выкл)
	bool GetAcceleratorsMode() const			{ return m_bAcceleratorsEnable; }
	void SetAcceleratorsMode(bool bEnable)		{ m_bAcceleratorsEnable = bEnable; }

	// Цвета
	void SetActiveTextColor(u32 cl)				{ m_cActiveTextColor = cl; m_bChangeColors = true; }
	u32 GetActiveTextColor() const 				{ return m_cActiveTextColor; }
	void SetGlobalTextColor(u32 cl)				{ m_cGlobalTextColor = cl; m_bChangeColors = true; }
	u32 GetGloablTextColor() const 				{ return m_cGlobalTextColor; }

	void SetActiveButtonColor(u32 cl)			{ m_cActiveButtonColor = cl; m_bChangeColors = true; }
	u32 GetActiveButtonColor() const 			{ return m_cActiveButtonColor; }
	void SetGlobalButtonColor(u32 cl)			{ m_cGlobalButtonColor = cl; m_bChangeColors = true; }
	u32 GetGlobalButtonColor() const 			{ return m_cGlobalButtonColor; }

	TABS_VECTOR * GetButtonsVector()			{ return &m_TabsArr; }

protected:
	// Список кнопок - переключателей закладок
	TABS_VECTOR		m_TabsArr;

	// Текущая нажатая кнопка. -1 - ни одна, 0 - первая, 1 - вторая, и т.д.
	int				m_iPushedIndex;

	// Цвет неактивных элементов
	u32				m_cGlobalTextColor;
	u32				m_cGlobalButtonColor;

	// Цвет надписи на активном элементе
	u32				m_cActiveTextColor;
	u32				m_cActiveButtonColor;

	// Разрешаем/запрещаем клавиатурные акселераторы
	bool			m_bAcceleratorsEnable;
	bool			m_bChangeColors;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUITabControl)
#undef script_type_list
#define script_type_list save_type_list(CUITabControl)

#endif