//////////////////////////////////////////////////////////////////////
// UIListItem.h: элемент окна списка CUIListWnd
//////////////////////////////////////////////////////////////////////

#ifndef _UI_LIST_ITEM_H_
#define _UI_LIST_ITEM_H_


#pragma once
#include "UIbutton.h"

class CUIListItem :
	public CUIButton
{
public:
	CUIListItem(void);
	~CUIListItem(void);

	virtual void Init(char* str, int x, int y, int width, int height);
	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);
	virtual void Draw();

	virtual void* GetData() {return m_pData;}
	virtual void SetData(void* pData) { m_pData = pData;}

	virtual int GetIndex() {return m_iIndex;}
	virtual void SetIndex(int index) {m_iIndex = index;}

	virtual int GetValue() {return m_iValue;}
	virtual void SetValue(int value) {m_iValue = value;}

protected:
	//указатель на произвольные данные, которые могут
	//присоедениены к элементу
	void* m_pData;
	
	//произвольное число, приписанное объекту
	int m_iValue;
	
	//индекс в списке
	int m_iIndex;
};


#endif