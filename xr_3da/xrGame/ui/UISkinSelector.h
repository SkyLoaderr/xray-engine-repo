//-----------------------------------------------------------------------------/
//  Окно выбора скина в сетевой игре
//-----------------------------------------------------------------------------/

#ifndef UI_SKIN_SELECTOR_H_
#define UI_SKIN_SELECTOR_H_

// #pragma once

#include "UIDialogWnd.h"
#include "UIFrameWindow.h"
#include "UIStatic.h"
#include "UIButton.h"
#include "UIInventoryUtilities.h"

using namespace InventoryUtilities;

// Количество скинов 
const u8		SKINS_COUNT = 4;

//-----------------------------------------------------------------------------/
//  Класс окна для выбора скинов
//-----------------------------------------------------------------------------/

class CUISkinSelectorWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:
	// Ctor and Dtor
	CUISkinSelectorWnd(const char* strSectionName);
	CUISkinSelectorWnd();
	~CUISkinSelectorWnd();

	// Инициализация
	virtual void	Init(const char *strSectionName);
	virtual void	SendMessage(CUIWindow *pWnd, s16 msg, void *pData = NULL);
	virtual void	OnMouse(int x, int y, E_MOUSEACTION mouse_action);
	virtual bool	OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action);
	virtual void	Draw();

	// Получаем индекс (0 <= Index < SKINS_COUNT) выбранного скина
	u8				GetActiveIndex()		{ return m_uActiveIndex; }

protected:
	// Запоминаем имя секции откуда читать инфу о скинах
	string32		m_strSection;

	// Индекс текущего выбранного скина
	u8				m_uActiveIndex;
	// Переключаем скин
	// Params:	idx - индекс нового скина
	// Return:	индекс предыдущего скина
	u8				SwitchSkin(const u8 idx);
	
	// Вычисление ширины окон со скинами и расстояния между окнами в зависимости
	// от количества скинов. Cхема такая: между окнами расстояние равное половине окна
	// Return:	ширина одного окна
	u32				CalculateSkinWindowWidth() const;

	// Инициализация скинов. Так как пока никто не знает как это будет вышлядеть, то имеем
	// что имеем
	void			InitializeSkins();

	// Отобразить подписи-клавиатурные акселераторы, для каждого скина
	void			DrawKBAccelerators();

	// Кнопки Ок и Отмена
	CUIButton		UIOkBtn, UICancelBtn;

	// Cтруктурка - набор необходимых элементов окошка со скином
	typedef struct tagSkinWindow
	{
		CUIStatic		UIHighlight;
		CUIFrameWindow	UIBackground;
	} SkinWindow;

	// Массив окошек со скинами
	SkinWindow	m_vSkinWindows[SKINS_COUNT];

};

#endif