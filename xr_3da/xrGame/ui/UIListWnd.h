//////////////////////////////////////////////////////////////////////
// UIListWnd.h: ���� �� �������
//////////////////////////////////////////////////////////////////////

#ifndef _UI_LIST_WND_H_
#define _UI_LIST_WND_H_


#pragma once
#include "uiwindow.h"
#include "uilistitem.h"
#include "uiscrollbar.h"


#define DEFAULT_ITEM_HEIGHT 30


DEF_LIST (LIST_ITEM_LIST, CUIListItem*);


class CUIListWnd :public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUIListWnd(void);
	virtual ~CUIListWnd(void);

	virtual void Init(int x, int y, int width, int height,
		              int item_height = DEFAULT_ITEM_HEIGHT);

	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	//���������, ������������ ������������� ����
	typedef enum{LIST_ITEM_CLICKED} E_MESSAGE;
			 
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData);
	virtual void Draw();
	
	bool AddItem(const char*  str, void* pData = NULL, int value = 0);
	bool AddItem(CUIListItem* pItem);
	
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

	void EnableScrollBar(bool enable);
	bool IsScrollBarEnabled() {return m_bScrollBarEnabled;}
	
	bool IsActiveBackgroundEnabled() {return m_bActiveBackgroundEnable;}
	void EnableActiveBackground(bool enable) {m_bActiveBackgroundEnable = enable;}

	//���������� ������ � �������� ����� ������ ������� �� ���������
	int GetLongestSignWidth();

	virtual void SetWidth(int width);
protected:

	//������ ���������
	CUIScrollBar m_ScrollBar;
	bool m_bScrollBarEnabled;

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

	//������� ��� ������� ������ � ������ ������ ��� -1, ���� ������ ���
	int m_iFocusedItem;
		
	//��������� ��������� ��������
	CUIStaticItem m_StaticActiveBackground;
	bool m_bActiveBackgroundEnable;
};

#endif //_UI_LIST_WND_H_