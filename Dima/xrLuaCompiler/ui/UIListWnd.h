//////////////////////////////////////////////////////////////////////
// UIListWnd.h: ���� �� �������
//////////////////////////////////////////////////////////////////////

#ifndef _UI_LIST_WND_H_
#define _UI_LIST_WND_H_


#pragma once
#include "uiwindow.h"
#include "uilistitem.h"
#include "uiscrollbar.h"


//DEFINE_LIST (CListItem*, LIST_ITEM_LIST, LIST_ITEM_LIST_IT)

DEF_LIST (LIST_ITEM_LIST, CUIListItem*);


class CUIListWnd :public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUIListWnd(void);
	virtual ~CUIListWnd(void);

	virtual void Init(int x, int y, int width, int height);

	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	//���������, ������������ ������������� ����
	typedef enum{LIST_ITEM_CLICKED} E_MESSAGE;
			 
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData);
	virtual void Draw();
	
	bool AddItem(char*  str, void* pData = NULL, int value = 0);
	void RemoveItem(int index);
	void RemoveAll();
	//������� ������ ������� � �������� pData, ����� -1
	int FindItem(void* pData);
	CUIListItem* GetItem(int index);

	int GetSize();

	void SetItemWidth(int iItemWidth);
	int GetItemWidth() {return m_iItemWidth;}

	void SetItemHeight(int iItemHeight); 
	int GetItemHeight() {return m_iItemHeight;}

	//����������� ��� �������� ������
	void Reset();

protected:

	//������ ���������
	CUIScrollBar m_ScrollBar;

	//���������� ��������� ������, ���������
	//���� ��������� ���������
	void UpdateList();


	//������ ��������� ������
	LIST_ITEM_LIST m_ItemList; 

	//������� �������� ������
	int m_iItemHeight;
	int m_iItemWidth;

	//���������� ����� ��� ���������
	int m_iRowNum;
	
	//������ ������� ����������� ��������
	int m_iFirstShownIndex;
};

#endif //_UI_LIST_WND_H_