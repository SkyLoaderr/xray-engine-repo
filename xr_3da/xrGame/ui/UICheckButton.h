// UICheckButton.h: класс кнопки, имеющей 2 состояния:
// с галочкой и без
//////////////////////////////////////////////////////////////////////

#ifndef _UI_CHECK_BUTTON_H_
#define _UI_CHECK_BUTTON_H_

#pragma once

#include "uibutton.h"

class CUICheckButton :public CUIButton
{
private:
	typedef CUIButton inherited;
public:
	CUICheckButton(void);
	virtual ~CUICheckButton(void);

	virtual void OnMouse(int x, int y, EUIMessages mouse_action);

	//сообщения, отправляемые родительскому окну
//	typedef enum{BUTTON_CLICKED, CHECK_BUTTON_SET, CHECK_BUTTON_RESET} E_MESSAGE;

	//прорисовка окна
	virtual void Draw();
	virtual void Update();


	//состояние кнопки
	bool GetCheck() {return m_bIsChecked;}
	void SetCheck() {m_bIsChecked = true;}
	void ResetCheck() {m_bIsChecked = false;}


protected:
	//флаг присутствия галочки 
	bool m_bIsChecked;
};


#endif // _UI_CHECK_BUTTON_H_