// UIPropertiesBox.h: 
//
// окшко с собщением для выбора действий, вызывается по нажатию правой 
// клавиши мыши
//////////////////////////////////////////////////////////////////////

#pragma once


#include "uiframewindow.h"
#include "uilistwnd.h"


class CUIPropertiesBox: public CUIFrameWindow
{
private:
	typedef CUIFrameWindow inherited; 
public:
	////////////////////////////////////
	//конструктор/деструктор
	CUIPropertiesBox();
	virtual ~CUIPropertiesBox();


	////////////////////////////////////
	//инициализация
	virtual void Init(LPCSTR base_name, int x, int y, int width, int height);


	//сообщения, отправляемые родительскому окну
	typedef enum{PROPERTY_CLICKED} E_MESSAGE;

	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	bool AddItem(char*  str, void* pData = NULL);
	void RemoveItem(int index);

	void Show(int x, int y);
	void Hide();

	//возращает индекс нажатого элемента или -1 если
	//такого элемента не было
	int GetClickedIndex() {return m_iClickedElement;}

protected:
	CUIListWnd m_UIListWnd;

	int m_iClickedElement;
};