//////////////////////////////////////////////////////////////////////
// UIScrollBox.h: класс перемещаемой каретки в CScrollBar
//////////////////////////////////////////////////////////////////////

#ifndef _UI_SCROLL_BOX_H_
#define _UI_SCROLL_BOX_H_

#pragma once
#include "uibutton.h"

class CUIScrollBox :
	public CUIButton
{
public:
	CUIScrollBox(void);
	virtual ~CUIScrollBox(void);

	virtual void Init(int x, int y, int length, int broad, bool bIsHorizontal);

	//сообщения, отправляемые родительскому окну
//	typedef enum{SCROLLBOX_MOVE, SCROLLBOX_STOP} E_MESSAGE;


	virtual void OnMouse(int x, int y, EUIMessages mouse_action);

	virtual void Draw();
						

protected:
	
	//горизонтальный или вертикальный 
	bool m_bIsHorizontal;

	int m_iOldMouseX;
	int m_iOldMouseY;

};

#endif