//////////////////////////////////////////////////////////////////////
// UIListWnd.h: ���� �� �������
//////////////////////////////////////////////////////////////////////

#ifndef _UI_LIST_WND_H_
#define _UI_LIST_WND_H_


#pragma once
#include "uiwindow.h"
#include "uilistitem.h"
#include "uiscrollbar.h"
#include "UIXmlInit.h"

#include "../script_export_space.h"

#define DEFAULT_ITEM_HEIGHT 30


DEF_LIST (LIST_ITEM_LIST, CUIListItem*);


class CUIListWnd :public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	CUIListWnd();
	virtual ~CUIListWnd();

	virtual void Init(int x, int y, int width, int height);
	virtual void Init(int x, int y, int width, int height, int item_height);

	virtual void OnMouse(int x, int y, EUIMessages mouse_action);

	//���������, ������������ ������������� ����
//	typedef enum{LIST_ITEM_CLICKED} E_MESSAGE;
			 
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData);
	virtual void Draw();
	virtual void Update();

	// ���������� ��������� � ��������
	template <class Element>
	bool AddItem			(const char*  str, const int shift = 0, void* pData = NULL,
							 int value = 0, int insertBeforeIdx = -1);

	virtual bool AddText_script (LPCSTR str, int shift, u32 color, CGameFont* pFont, bool doPreProcess);
	virtual bool AddItem_script(CUIListItem* item);

	template <class Element>
	bool AddParsedItem		(const CUIString &str, const int shift,
							 const u32 &MsgColor, CGameFont* pFont = NULL,
							 void* pData = NULL, int value = 0, int insertBeforeIdx = -1);

	//////////////////////////////////////////////////////////////////////////
	// �������� ������� � ����
	// Params:
	// 1.	pItem - ��������� �� ������� ����������
	// 2.	insertBeforeIdx - ������ �������� ����� ������� ���������.
	//		-1 �������� �������� � �����
	//////////////////////////////////////////////////////////////////////////
	
	template <class Element>
	bool AddItem			(Element* pItem, int insertBeforeIdx = -1);
	
	void RemoveItem(int index);
	void RemoveAll();
	//������� ������ ������� � �������� pData, ����� -1
	int FindItem(void* pData);
	CUIListItem* GetItem(int index);
	// �������� ������ �������� �� ������. ���� -1 ���� ��� ������
	int GetItemPos(CUIListItem *pItem);

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
	void ScrollToPos(int position);
	
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
	int GetSelectedItem() { return m_iSelectedItem; }
	
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
	xr_vector<int> AddInteractiveItem(const char *str2, const int shift = 0,
		const u32 &MsgColor = 0xffffffff, CGameFont* pFont = 0, int pushAfter = -1);

	void	SetNewRenderMethod	(bool value)	{ m_bNewRenderMethod = value; }
	int		GetListPosition		() const		{ return m_iFirstShownIndex; }

	// ������ ������ �� ���� ����� �� ������
	void	SetRightIndention	(int value)		{ m_iRightIndention = value; }
	int		GetRightIndention	() const		{ return m_iRightIndention; }

	/* new_code_here
	CUIListItem* GetClickedItem();
	*/

protected:

	//������ ���������
	CUIScrollBar	m_ScrollBar;
	bool	m_bScrollBarEnabled;

	//���������� ��������� ������, ���������
	//���� ��������� ���������
	void	UpdateList();

	//������ ��������� �����
	LIST_ITEM_LIST	m_ItemList; 

	//������� �������� ������
	int		m_iItemHeight;
	int		m_iItemWidth;

	//���������� ����� ��� ���������
	int		m_iRowNum;
	
	//������ ������� ����������� ��������
	int		m_iFirstShownIndex;

	//������� ��� ������� ������ � ������ ������ ��� -1, ���� ������ ���
	int		m_iFocusedItem;
	int		m_iFocusedItemGroupID;
	int     m_iSelectedItem;
	int     m_iSelectedItemGroupID;

	// ���� ����� ������������� ���������� �����, �� ������� ���� ����
	bool	m_bForceFocusedItem;

	//��������� ��������� ��������
	CUIStaticItem	m_StaticActiveBackground;
	bool	m_bActiveBackgroundEnable;

	//������� ���� ������
	u32		m_dwFontColor;
	bool	m_bListActivity;

	//��������� ������ �� ���������
	bool	m_bVertFlip;
	
	// ������� ����, ��� ���� ���������
	bool	m_bUpdateMouseMove;

	// ������� ���������� �������������
	int		m_iLastUniqueID;
	bool	m_bNewRenderMethod;

	// ������ ������ �� ���� ����� �� ������
	int		m_iRightIndention;
	/* new_code_here
	CUIListItem* m_pClickedListItem;
	*/

	DECLARE_SCRIPT_REGISTER_FUNCTION

	// ��������������� ������ ���������� ������ ���������� ����� ������� � ������� �������
	LPSTR	FindNextWord(LPSTR currPos) const;
	// ��������������� ������ ����������� ������ ������ �� ����� ����� ������� � ������� �������
	int		WordTailSize(LPCSTR currPos, CGameFont *font, int &charsCount) const;
	bool	IsEmptyDelimiter(const char c) const;
};

#include "UIListWnd_inline.h"

add_to_type_list(CUIListWnd)
#undef script_type_list
#define script_type_list save_type_list(CUIListWnd)

#endif //_UI_LIST_WND_H_