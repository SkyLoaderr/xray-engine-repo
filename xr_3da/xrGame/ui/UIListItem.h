//////////////////////////////////////////////////////////////////////
// UIListItem.h: элемент окна списка CUIListWnd
//////////////////////////////////////////////////////////////////////

#ifndef _UI_LIST_ITEM_H_
#define _UI_LIST_ITEM_H_


#pragma once
#include "UIbutton.h"

class CUIListItem :	public CUIButton
{
private:
	typedef CUIButton inherited;
public:
	CUIListItem(void);
	virtual ~CUIListItem(void);

	virtual void Init(int x, int y, int width, int height);
	virtual void Init(const char* str, int x, int y, int width, int height);
	
	virtual void OnMouse(int x, int y, EUIMessages mouse_action);
	virtual void Draw();

	virtual void* GetData() {return m_pData;}
	virtual void SetData(void* pData) { m_pData = pData;}

	virtual int GetIndex() {return m_iIndex;}
	virtual void SetIndex(int index) {m_iIndex = index; m_iGroupID = index;}

	virtual int GetValue() {return m_iValue;}
	virtual void SetValue(int value) {m_iValue = value;}

	virtual int	GetGroupID() { return m_iGroupID; }
	virtual void SetGroupID(int ID) { m_iGroupID = ID; }

	//возвращает длину в пикселях надписи
	virtual int GetSignWidht();

	// переопределяем критерий подсвечивания текста
	virtual bool IsHighlightText();
	virtual void SetHighlightText(bool Highlight)		{ m_bHighlightText = Highlight; }

	// Автоматическое обновление
	void	ManualDelete(bool value) { m_bManualDelete = value; }
	bool	IsManualDelete() { return m_bManualDelete; }

protected:
	//указатель на произвольные данные, которые могут
	//присоедениены к элементу
	void* m_pData;
	
	//произвольное число, приписанное объекту
	int m_iValue;
	
	//индекс в списке
	int m_iIndex;

	// идентификатор группы
	int m_iGroupID;

	// подсвечивается кнопка или нет?
	bool m_bHighlightText;

	// удалять item при детаче?
	bool m_bManualDelete;
};

#endif