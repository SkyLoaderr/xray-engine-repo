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

	virtual void Init(int x, int y, int width, int height);
	virtual void Init(int x, int y, int width, int height, int item_height);

	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	//���������, ������������ ������������� ����
	typedef enum{LIST_ITEM_CLICKED} E_MESSAGE;
			 
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData);
	virtual void Draw();
	virtual void Update();
	
	bool AddItem(const char*  str, void* pData = NULL, int value = 0, bool push_front = false);
	bool AddParsedItem(const CUIString &str, const char StartShift, const u32 &MsgColor, CGameFont* pFont = NULL, void* pData = NULL, int value = 0, bool push_front = false);
	bool AddItem(CUIListItem* pItem, bool push_front = false);
	
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
	void UpdateScrollBar();

	void ScrollToBegin();
	void ScrollToEnd();
	
	bool IsActiveBackgroundEnabled() {return m_bActiveBackgroundEnable;}
	void EnableActiveBackground(bool enable) {m_bActiveBackgroundEnable = enable;}

	//���������� ������ � �������� ����� ������ ������� �� ���������
	int GetLongestSignWidth();

	virtual void SetWidth(int width);

	void SetTextColor(u32 color) {m_dwFontColor = color;} 
	u32 GetTextColor() {return m_dwFontColor;}

	//������ ��������� (��� ������) �������� ������
	void ActivateList(bool activity);
	bool IsListActive() {return m_bListActivity;}

	void SetVertFlip(bool vert_flip) {m_bVertFlip = vert_flip;}
	bool GetVertFlip() {return m_bVertFlip;}

	// �������������� ��������� ������
	void SetFocusedItem(int iNewFocusedItem);
	int GetFocusedItem() { return m_iFocusedItem; }
	void ResetFocusCapture() { m_bForceFocusedItem = false; }

	// ������������� ������ ������������ ����� 
	// ������ � ������, ������� ����� ������. ��� �������� ������������� ������ ������� ������ 
	// ������ ����� ���: "���� %������������� �������% ����2 %������������� �������2%...",
	// ��� '%' - �SeparatorChar (�����������).
	// ���� ����������� � ����������� ����� �������������� �����������, �� ������� ����� ������.
	// ��������� �������������� ������������� ���� (�� ����� AddParsedItem) ������.

	// ��� �������� �������� ����, ������ ������ ������ ���� ��� ������� ���������� ������ "%"
	// NOTE: �� ������������� ��������� ���������, ������ �� ����������! �������: ���������� �������� 
	// ������ ������� � ��������
	// ���������� ������ ���������� ��������������� ��� ������������� ��������� � ������, ������� ��� 
	// ����� ���������� � ���� pData ��� ������� �� ������������� �������
	xr_vector<int> AddInteractiveItem(const char *str2, const char StartShift = 0,
		const u32 &MsgColor = 0xffffffff, CGameFont* pFont = 0);

protected:

	//������ ���������
	CUIScrollBar m_ScrollBar;
	bool m_bScrollBarEnabled;

	//���������� ��������� ������, ���������
	//���� ��������� ���������
	void UpdateList();

	//������ ��������� �����
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
	int m_iFocusedItemGroupID;
	// ���� ����� ������������� ���������� �����, �� ������� ���� ����
	bool m_bForceFocusedItem;

	//��������� ��������� ��������
	CUIStaticItem m_StaticActiveBackground;
	bool m_bActiveBackgroundEnable;

	//������� ���� ������
	u32 m_dwFontColor;

	bool m_bListActivity;

	//��������� ������ �� ���������
	bool m_bVertFlip;
	
	// ������� ����, ��� ���� ���������
	bool m_bUpdateMouseMove;

	// ������� ���������� �������������
	int m_iLastUniqueID;
};

#endif //_UI_LIST_WND_H_