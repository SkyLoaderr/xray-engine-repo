#pragma once

#include "UIStatic.h"
#include "../script_export_space.h"


class CUIButton : public CUIStatic
{
private:
	typedef			CUIStatic				inherited;
public:
					CUIButton				();
	virtual			~CUIButton				();

	virtual void	Init					(LPCSTR tex_name, float x, float y, float width, float height);
	virtual void	Init					(float x, float y, float width, float height);

	virtual bool	OnMouse					(float x, float y, EUIMessages mouse_action);
	virtual void	OnClick					();

	//прорисовка окна
	virtual void	DrawTexture				();
	virtual void	DrawText				();
	virtual void	DrawHighlightedText		();

	virtual void	Update					();
	virtual void	Enable					(bool status);

	virtual void	OnFocusLost				();

	//режимы в которых можно нажимать кнопку
	typedef enum{NORMAL_PRESS, //кнопка нажимается при 
							   //нажатии и отпускании на ней мыши
				 DOWN_PRESS    //сразу при нажатии
			} E_PRESS_MODE;

	void			SetPressMode			(E_PRESS_MODE ePressMode)	{m_ePressMode = ePressMode;}
	E_PRESS_MODE	GetPressMode			()							{return m_ePressMode;}

	//заново подготовить состояние
    virtual void	Reset					();

	void			SetPushOffsetX			(float offset_x)			{m_iPushOffsetX = offset_x;}
	void			SetPushOffsetY			(float offset_y)			{m_iPushOffsetY = offset_y;}
	float			GetPushOffsetX			()							{return m_iPushOffsetX;}
	float			GetPushOffsetY			()							{return m_iPushOffsetY;}

	//подсвечен ли текст на кнопке
	// принудительная подсветка
	virtual void	HighlightItem			(bool bHighlight)			{m_bCursorOverWindow = bHighlight; }
	// Цвет подсветки

	//состояния в которых находится кнопка
	typedef enum{BUTTON_NORMAL, //кнопка никак не затрагивается
		BUTTON_PUSHED, //в нажатом сотоянии
		BUTTON_UP      //при удерживаемой кнопки мыши 
	} E_BUTTON_STATE;

	// Установка состояния кнопки: утоплена, не утоплена
	void			SetButtonMode			(E_BUTTON_STATE eBtnState)	{ m_eButtonState = eBtnState; }
	E_BUTTON_STATE	GetButtonsState			()							{ return m_eButtonState;}

	// Поведение кнопки как переключателя реализовано пока только в режиме NORMAL_PRESS
	void			SetButtonAsSwitch		(bool bAsSwitch)			{ m_bIsSwitch = bAsSwitch; }

	// Работа с акселератором
	// Код акселератора берется из файла dinput.h, из DirectX SDK.
	// Например: кнопка A - код 0x1E(DIK_A)
	void			SetAccelerator			(u32 uAccel)				{ m_uAccelerator = uAccel; }
	const u32		GetAccelerator			() const					{ return m_uAccelerator; }
	
	// Смещение подсветки текста относительно самого текста. Может для при дания эффекта тени
	void			SetShadowOffset			(float offsetX, float offsetY) { m_iShadowOffsetX = offsetX; m_iShadowOffsetY = offsetY; }
	shared_str			m_hint_text;
protected:
	
	E_BUTTON_STATE		m_eButtonState;

	bool				m_bIsSwitch;

	//если кнопка была только что нажата
	bool				m_bButtonClicked;


	//режим в котором нажимается кнопка
	E_PRESS_MODE		m_ePressMode;

	//смещение кнопки при нажатии
	float				m_iPushOffsetX;
	float				m_iPushOffsetY;

	// код акселератора
	u32					m_uAccelerator;

	// Смещение подсветки текста
	float				m_iShadowOffsetX;
	float				m_iShadowOffsetY;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};


add_to_type_list(CUIButton)
#undef script_type_list
#define script_type_list save_type_list(CUIButton)
