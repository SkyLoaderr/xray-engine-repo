//////////////////////////////////////////////////////////////////////
// UIListWnd.h: окно со списком
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

	//сообщения, отправляемые родительскому окну
//	typedef enum{LIST_ITEM_CLICKED} E_MESSAGE;
			 
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData);
	virtual void Draw();
	virtual void Update();
	
	//////////////////////////////////////////////////////////////////////////
	
	
	template <class Element>
	bool AddItem(const char*  str, const int shift = 0, void* pData = NULL,

				 int value = 0, int insertBeforeIdx = -1)
	{
		//создать новый элемент и добавить его в список
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
			// '\n' - переход на новую строку
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
	// Добавить элемент в лист
	// Params:
	// 1.	pItem - указатель на элемент добавления
	// 2.	insertBeforeIdx - индекс элемента перед которым вставляем.
	//		-1 означает добавить в конец
	//////////////////////////////////////////////////////////////////////////
	
	template <class Element>
	bool AddItem(Element* pItem, int insertBeforeIdx = -1)
	{	
		AttachChild(pItem);

		pItem->Init(pItem->GetWndRect().left, m_bVertFlip?GetHeight()-GetSize()* m_iItemHeight-m_iItemHeight:GetSize()* m_iItemHeight, 
			m_iItemWidth, m_iItemHeight);

		pItem->SetNewRenderMethod(m_bNewRenderMethod);

		//добавление в конец или начало списка
		if(-1 == insertBeforeIdx)
		{
			m_ItemList.push_back(pItem);
			pItem->SetIndex(m_ItemList.size()-1);
		}
		else
		{
			//изменить значения индексов уже добавленых элементов
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

		//обновить полосу прокрутки
		m_ScrollBar.SetRange(0,s16(m_ItemList.size()-1));
		m_ScrollBar.SetPageSize(s16(
			(u32)m_iRowNum<m_ItemList.size()?m_iRowNum:m_ItemList.size()));
		m_ScrollBar.SetScrollPos(s16(m_iFirstShownIndex));

		UpdateScrollBar();

		return true;
	}
	
	void RemoveItem(int index);
	void RemoveAll();
	//находит первый элемент с заданной pData, иначе -1
	int FindItem(void* pData);
	CUIListItem* GetItem(int index);
	// Получить индекс элемента по адресу. Либо -1 если нет такого
	int GetItemPos(CUIListItem *pItem);

	int GetSize();

	void SetItemWidth(int iItemWidth);
	int GetItemWidth() {return m_iItemWidth;}

	void SetItemHeight(int iItemHeight); 
	int GetItemHeight() {return m_iItemHeight;}

	//подготовить все элементы заново
	void Reset();

	void EnableScrollBar(bool enable);
	bool IsScrollBarEnabled() {return m_bScrollBarEnabled;}
	void UpdateScrollBar();

	void ScrollToBegin();
	void ScrollToEnd();
	
	bool IsActiveBackgroundEnabled() {return m_bActiveBackgroundEnable;}
	void EnableActiveBackground(bool enable) {m_bActiveBackgroundEnable = enable;}

	//возвращает ширину в пикселях самой длиной надписи на элементах
	int GetLongestSignWidth();

	virtual void SetWidth(int width);

	void SetTextColor(u32 color) {m_dwFontColor = color;} 
	u32 GetTextColor() {return m_dwFontColor;}

	//делает активными (как кнопки) элементы списка
	void ActivateList(bool activity);
	bool IsListActive() {return m_bListActivity;}

	void SetVertFlip(bool vert_flip) {m_bVertFlip = vert_flip;}
	bool GetVertFlip() {return m_bVertFlip;}

	// Принудительная установка фокуса
	void SetFocusedItem(int iNewFocusedItem);
	int GetFocusedItem() { return m_iFocusedItem; }
	void ResetFocusCapture() { m_bForceFocusedItem = false; }

	// Интерактивная запись представляет собой 
	// запись с полями, которые можно нажать. Для создания интерактивной записи входная строка 
	// должна иметь вид: "инфа %интерактивный элемент% инфа2 %интерактивный элемент№2%...",
	// где '%' - сSeparatorChar (разделитель).
	// Поля заключенные в разделители будут интерактивными элементками, на которые можно нажать.
	// Создавать мультистрочные интерактивные поля (на манер AddParsedItem) нельзя.

	// Для создания обычного поля, строка записи должна быть без четного количества знаков "%"
	// NOTE: До инициализации рендеринг контекста, строки не добавляють! Причина: невозможно получить 
	// ширину надписи в пискелях
	// Возвращаем вектор уникальных идентификаторов для интерактивных элементов в строке, которые нам 
	// будут посылаться в поле pData при нажатии на интерактивный элемент
	xr_vector<int> AddInteractiveItem(const char *str2, const int shift = 0,
		const u32 &MsgColor = 0xffffffff, CGameFont* pFont = 0, int pushAfter = -1);

	void	SetNewRenderMethod(bool value) { m_bNewRenderMethod = value; }
	int		GetListPosition() const { return m_iFirstShownIndex; }

protected:

	//полоса прокрутки
	CUIScrollBar m_ScrollBar;
	bool m_bScrollBarEnabled;

	//обновления елементов списка, вызвается
	//если произошли изменения
	void UpdateList();

	//список элементов листа
	LIST_ITEM_LIST m_ItemList; 

	//размеры элемента списка
	int m_iItemHeight;
	int m_iItemWidth;

	//количество рядов для элементов
	int m_iRowNum;
	
	//индекс первого показанного элемента
	int m_iFirstShownIndex;

	//элемент над которым курсор в данный момент или -1, если такого нет
	int m_iFocusedItem;
	int m_iFocusedItemGroupID;
	// Если хотим принудительно выставлять фокус, то поднять этот флаг
	bool m_bForceFocusedItem;

	//подсветка активного элемента
	CUIStaticItem m_StaticActiveBackground;
	bool m_bActiveBackgroundEnable;

	//текущий цвет текста
	u32 m_dwFontColor;

	bool m_bListActivity;

	//переворот списка по вертикали
	bool m_bVertFlip;
	
	// Признак того, что мышь подвинули
	bool m_bUpdateMouseMove;

	// Текущий уникальный идентификатор
	int m_iLastUniqueID;
	bool m_bNewRenderMethod;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIListWnd)
#undef script_type_list
#define script_type_list save_type_list(CUIListWnd)

#endif //_UI_LIST_WND_H_