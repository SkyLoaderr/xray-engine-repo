//////////////////////////////////////////////////////////////////////
// UIListWnd.h: ���� �� �������
//////////////////////////////////////////////////////////////////////

#ifndef _UI_LIST_WND_H_
#define _UI_LIST_WND_H_


#pragma once
#include "uiwindow.h"
#include "uilistitem.h"
#include "uiscrollbar.h"

#include "../script_export_space.h"

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

	virtual void OnMouse(int x, int y, EUIMessages mouse_action);

	//���������, ������������ ������������� ����
//	typedef enum{LIST_ITEM_CLICKED} E_MESSAGE;
			 
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData);
	virtual void Draw();
	virtual void Update();
	
	//////////////////////////////////////////////////////////////////////////
	
	
	template <class Element>
	bool AddItem(const char*  str, const int shift = 0, void* pData = NULL,

				 int value = 0, int insertBeforeIdx = -1)
	{
		//������� ����� ������� � �������� ��� � ������
		Element* pItem = NULL;
		pItem = xr_new<Element>();

		if(!pItem) return false;


		pItem->Init(str, shift, m_bVertFlip?GetHeight()-GetSize()* m_iItemHeight-m_iItemHeight:GetSize()* m_iItemHeight, 
			m_iItemWidth, m_iItemHeight);

		pItem->SetData(pData);
		pItem->SetValue(value);
		pItem->SetTextColor(m_dwFontColor);

		return AddItem<Element>(pItem, insertBeforeIdx);
	}

	//////////////////////////////////////////////////////////////////////////
	bool AddText_script(LPCSTR str, int shift, u32 color, CGameFont* pFont);
	template <class Element>
	bool AddParsedItem(const CUIString &str, const int shift,
					   const u32 &MsgColor, CGameFont* pFont = NULL,
					   void* pData = NULL, int value = 0, int insertBeforeIdx = -1)
	{
		bool ReturnStatus = true;
		const STRING& text = str.m_str;
		STRING buf;

		u32 last_pos = 0;

		int GroupID = GetSize();

		for(int i = 0; i < static_cast<int>(text.size()) - 2; ++i)
		{
			// '\n' - ������� �� ����� ������
			if((text[i] == '\\' && text[i+1]== 'n') || (10 == text[i] && 13 == text[i + 1]))
			{	
				buf.clear();
				buf.insert(buf.begin(), text.begin()+last_pos, text.begin()+i);
				buf.push_back(0);
				ReturnStatus &= AddItem<Element>(&buf.front(), shift, pData, value, insertBeforeIdx);
				Element *pLocalItem = smart_cast<Element*>(GetItem(GetSize() - 1));
				pLocalItem->SetGroupID(GroupID);
				pLocalItem->SetTextColor(MsgColor);
				pLocalItem->SetFont(pFont);
				++i;
				last_pos = i+1;
			}	
		}

		if(last_pos<text.size())
		{
			buf.clear();
			buf.insert(buf.begin(), text.begin()+last_pos, text.end());
			buf.push_back(0);
			AddItem<Element>(&buf.front(), shift, pData, value, insertBeforeIdx);
			GetItem(GetSize() - 1)->SetGroupID(GroupID);
			Element *pLocalItem = smart_cast<Element*>(GetItem(GetSize() - 1));
			pLocalItem->SetGroupID(GroupID);
			pLocalItem->SetTextColor(MsgColor);
			pLocalItem->SetFont(pFont);
		}

		return ReturnStatus;
	}

	//////////////////////////////////////////////////////////////////////////
	// �������� ������� � ����
	// Params:
	// 1.	pItem - ��������� �� ������� ����������
	// 2.	insertBeforeIdx - ������ �������� ����� ������� ���������.
	//		-1 �������� �������� � �����
	//////////////////////////////////////////////////////////////////////////
	
	template <class Element>
	bool AddItem(Element* pItem, int insertBeforeIdx = -1)
	{	
		AttachChild(pItem);

		pItem->Init(pItem->GetWndRect().left, m_bVertFlip?GetHeight()-GetSize()* m_iItemHeight-m_iItemHeight:GetSize()* m_iItemHeight, 
			m_iItemWidth, m_iItemHeight);

		pItem->SetNewRenderMethod(m_bNewRenderMethod);

		//���������� � ����� ��� ������ ������
		if(-1 == insertBeforeIdx)
		{
			m_ItemList.push_back(pItem);
			pItem->SetIndex(m_ItemList.size()-1);
		}
		else
		{
			//�������� �������� �������� ��� ���������� ���������
			if (!m_ItemList.empty())
				R_ASSERT(static_cast<u32>(insertBeforeIdx) <= m_ItemList.size());

			LIST_ITEM_LIST_it it2 = m_ItemList.begin();
			std::advance(it2, insertBeforeIdx);
			for(LIST_ITEM_LIST_it it = it2; m_ItemList.end() != it; ++it)
			{
				(*it)->SetIndex((*it)->GetIndex()+1);
			}
			m_ItemList.insert(it2, pItem);
			pItem->SetIndex(insertBeforeIdx);
		}

		UpdateList();

		//�������� ������ ���������
		m_ScrollBar.SetRange(0,s16(m_ItemList.size()-1));
		m_ScrollBar.SetPageSize(s16(
			(u32)m_iRowNum<m_ItemList.size()?m_iRowNum:m_ItemList.size()));
		m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));

		UpdateScrollBar();

		return true;
	}
	
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
	xr_vector<int> AddInteractiveItem(const char *str2, const int shift = 0,
		const u32 &MsgColor = 0xffffffff, CGameFont* pFont = 0, int pushAfter = -1);

	void	SetNewRenderMethod(bool value) { m_bNewRenderMethod = value; }
	int		GetListPosition() const { return m_iFirstShownIndex; }

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
	bool m_bNewRenderMethod;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIListWnd)
#undef script_type_list
#define script_type_list save_type_list(CUIListWnd)

#endif //_UI_LIST_WND_H_