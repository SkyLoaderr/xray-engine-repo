// UIButton.h: класс нажимаемой кнопки
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_BUTTON_H_
#define _UI_BUTTON_H_

#pragma once


#include "UIWindow.h"


#include "../uistaticitem.h"
#include "UIStatic.h"



class CUIButton : public CUIStatic
{
private:
	typedef CUIStatic inherited;
public:
	CUIButton();
	virtual ~ CUIButton();


	virtual void	Init(LPCSTR tex_name, int x, int y, int width, int height);
	virtual void	Init(int x, int y, int width, int height);
	
	virtual void	OnMouse(int x, int y, EUIMessages mouse_action);

	//сообщения, отправляемые родительскому окну
//	typedef enum{BUTTON_CLICKED, BUTTON_FOCUS_RECEIVED, BUTTON_FOCUS_LOST, BUTTON_DOWN} E_MESSAGE;

	//прорисовка окна
	virtual void	Draw();
	//обновление перед прорисовкой
	virtual void	Update();


	//режимы в которых можно нажимать кнопку
	typedef enum{NORMAL_PRESS, //кнопка нажимается при 
							   //нажатии и отпускании на ней мыши
				 DOWN_PRESS,   //сразу при нажатии
				 UP_PRESS      //сразу при отпускании
			} E_PRESS_MODE;

	void			SetPressMode(E_PRESS_MODE ePressMode) {m_ePressMode = ePressMode;}
	E_PRESS_MODE	GetPressMode() {return m_ePressMode;}

	//заново подготовить состояние
    virtual void	Reset();

	void			SetPushOffsetX(int offset_x) {m_iPushOffsetX = offset_x;}
	void			SetPushOffsetY(int offset_y) {m_iPushOffsetY = offset_y;}
	int				GetPushOffsetX() {return m_iPushOffsetX;}
	int				GetPushOffsetY() {return m_iPushOffsetY;}

	virtual	void	UpdateTextAlign();

	//подсвечен ли текст на кнопке
	virtual bool	IsHighlightText();
	// принудительная подсветка
	virtual void	HighlightItem(bool bHighlight) { m_bCursorOverButton = bHighlight; }
	// Цвет подсветки
	virtual void	SetHighlightColor(const u32 uColor)	{ m_HighlightColor = uColor; }
	void			EnableTextHighlighting(bool value)		{ m_bEnableTextHighlighting = value; }

	//состояния в которых находится кнопка
	typedef enum{BUTTON_NORMAL, //кнопка никак не затрагивается
		BUTTON_PUSHED, //в нажатом сотоянии
		BUTTON_UP      //при удерживаемой кнопки мыши 
	} E_BUTTON_STATE;

	// Установка состояния кнопки: утоплена, не утоплена
	void			SetButtonMode(E_BUTTON_STATE eBtnState) { m_eButtonState = eBtnState; }

	// Поведение кнопки как переключателя реализовано пока только в режиме NORMAL_PRESS
	void			SetButtonAsSwitch(bool bAsSwitch) { m_bIsSwitch = bAsSwitch; }

	// Работа с акселератором
	// Код акселератора берется из файла dinput.h, из DirectX SDK.
	// Например: кнопка A - код 0x1E(DIK_A)
	void			SetAccelerator(u32 uAccel)	{ m_uAccelerator = uAccel; }
	const u32		GetAccelerator() const		{ return m_uAccelerator; }
	
	// Метод рендеринга: старый - вся строке скопом, но одним цветом, или по словам, с переносами и
	// цветовым отделением отдельных блоков
	void			SetNewRenderMethod(bool newMethod) { m_bNewRenderMethod = newMethod; }

	// Смещение подсветки текста относительно самого текста. Может для при дания эффекта тени
	void			SetShadowOffset(int offsetX, int offsetY) { m_iShadowOffsetX = offsetX; m_iShadowOffsetY = offsetY; }

protected:
	
	E_BUTTON_STATE	m_eButtonState;

	bool			m_bIsSwitch;

	//если кнопка была только что нажата
	bool			m_bButtonClicked;

	//если курсор над мышкой
	bool			m_bCursorOverButton;

	// Включена ли подсветка текста
	bool			m_bEnableTextHighlighting;

	//режим в котором нажимается кнопка
	E_PRESS_MODE	m_ePressMode;

	//смещение кнопки при нажатии
	int				m_iPushOffsetX;
	int				m_iPushOffsetY;

	// Цвет подсветки
	u32				m_HighlightColor;

	// код акселератора
	u32				m_uAccelerator;
	bool			m_bNewRenderMethod;

	// Смещение подсветки текста
	int				m_iShadowOffsetX;
	int				m_iShadowOffsetY;
};

#endif // _UI_BUTTON_H_
