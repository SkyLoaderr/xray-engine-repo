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

	bool AddItem(char*  str, void* pData = NULL, int value = 0);
	void RemoveItem(int index);
	void RemoveAll();

	void Show(int x, int y);
	void Hide();

	//возращает индекс нажатого элемента или -1 если
	//такого элемента не было
	int GetClickedIndex();
	//возвращает нажатый элемент или NULL если нет такого
	CUIListItem* GetClickedItem();

	//автоматическое изменение высоты в зависимости от количества
	//элементов
	void AutoUpdateHeight();

protected:
	CUIListWnd m_UIListWnd;

	int m_iClickedElement;
};