// UIButton.h: класс нажимаемой кнопки
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_BUTTON_H_
#define _UI_BUTTON_H_

#pragma once


#include "UIWindow.h"


#include "..\uistaticitem.h"



class CUIButton : public CUIWindow  
{
public:
	 CUIButton();
	virtual ~ CUIButton();


	virtual void Init(LPCSTR tex_name, int x, int y, int width, int height);
	
	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	//сообщения, отправляемые родительскому окну
	typedef enum{BUTTON_CLICKED} E_MESSAGE;

	//прорисовка окна
	virtual void Draw();
	//обновление перед прорисовкой
	virtual void Update();


	//режимы в которых можно нажимать кнопку
	typedef enum{NORMAL_PRESS, //кнопка нажимается при 
							   //нажатии и отпускании на ней мыши
				 DOWN_PRESS,   //сразу при нажатии
				 UP_PRESS      //сразу при отпускании
			} E_PRESS_MODE;

	void SetPressMode(E_PRESS_MODE ePressMode) {m_ePressMode = ePressMode;}
	E_PRESS_MODE GetPressMode() {return m_ePressMode;}


	void SetText(LPSTR str) {m_str = str;}
	LPSTR GetText() {return m_str;}


protected:
	
	//состояния в которых находиться кнопка
	typedef enum{BUTTON_NORMAL, //кнопка никак не затрагивается
				 BUTTON_PUSHED,   //в нажатом сотоянии
				 BUTTON_UP      //при удерживаемой кнопки мыши 
			} E_BUTTON_STATE;
	
	E_BUTTON_STATE m_eButtonState;

	//если кнопка была только что нажата
	bool m_bButtonClicked;

	//если курсор над мышкой
	bool m_bCursorOverButton;

	//режим в котором нажимается кнопка
	E_PRESS_MODE m_ePressMode;

	//текст на кнопке
	LPSTR m_str;



	///////////////////////////////////////	
	//Графический интрефейс для рисования
	///////////////////////////////////////
	CUIStaticItem m_UIStaticItem;
	
};

#endif // _UI_BUTTON_H_
