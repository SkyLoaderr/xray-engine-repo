//////////////////////////////////////////////////////////////////////
// UIListWnd.h: окно со списком
//////////////////////////////////////////////////////////////////////

#ifndef _UI_LIST_WND_H_
#define _UI_LIST_WND_H_


#pragma once
#include "uiwindow.h"
#include "uilistitem.h"
#include "uiscrollbar.h"


//DEFINE_LIST (CListItem*, LIST_ITEM_LIST, LIST_ITEM_LIST_IT)

DEF_LIST (LIST_ITEM_LIST, CUIListItem*);


class CUIListWnd :
	public CUIWindow
{
public:
	CUIListWnd(void);
	virtual ~CUIListWnd(void);

	virtual void Init(int x, int y, int width, int height);

	//сообщения, отправляемые родительскому окну
	typedef enum{LIST_ITEM_CLICKED} E_MESSAGE;

				 
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData);
	virtual void Draw();
	
	bool AddItem(char*  str, void* pData = NULL);
	void RemoveItem(int index);
	void RemoveAll();
	//находит первый элемент с заданной pData, иначе -1
	int FindItem(void* pData);

	int GetSize();

	void SetItemWidth(int iItemWidth);
	int GetItemWidth() {return m_iItemWidth;}

	void SetItemHeight(int iItemHeight); 
	int GetItemHeight() {return m_iItemHeight;}

	//подготовить все элементы заново
	void Reset();

protected:

	//полоса прокрутки
	CUIScrollBar m_ScrollBar;

	//обновления елементов списка, вызвается
	//если произошли изменения
	void UpdateList();


	//вектор элементов списка
	LIST_ITEM_LIST m_ItemList; 

	//размеры элемента списка
	int m_iItemHeight;
	int m_iItemWidth;

	//количество рядов для элементов
	int m_iRowNum;
	
	//индекс первого показанного элемента
	int m_iFirstShownIndex;


};

#endif //_UI_LIST_WND_H_