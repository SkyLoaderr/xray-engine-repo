//////////////////////////////////////////////////////////////////////
// UIListWnd.h: окно со списком
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

	//сообщения, отправляемые родительскому окну
	typedef enum{LIST_ITEM_CLICKED} E_MESSAGE;
			 
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData);
	virtual void Draw();
	virtual void Update();
	
	bool AddItem(const char*  str, void* pData = NULL, int value = 0, bool push_front = false);
	bool AddParsedItem(const CUIString &str, const char StartShift, const u32 &MsgColor, CGameFont* pFont = NULL, void* pData = NULL, int value = 0, bool push_front = false);
	bool AddItem(CUIListItem* pItem, bool push_front = false);
	
	void RemoveItem(int index);
	void RemoveAll();
	//находит первый элемент с заданной pData, иначе -1
	int FindItem(void* pData);
	CUIListItem* GetItem(int index);

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
	xr_vector<int> AddInteractiveItem(const char *str2, const char StartShift = 0,
		const u32 &MsgColor = 0xffffffff, CGameFont* pFont = 0);

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
};

#endif //_UI_LIST_WND_H_