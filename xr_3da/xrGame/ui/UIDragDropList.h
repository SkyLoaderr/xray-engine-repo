// UIDragDropList.h: список элементов Drag&Drop
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_DRAG_DROP_LIST_H_
#define _UI_DRAG_DROP_LIST_H_


#pragma once


#include "uiframewindow.h"
#include "uidragdropitem.h"
#include "uiscrollbar.h"

#include "uistatic.h"
class CUIDragDropList;

typedef bool (*CHECK_PROC)(CUIDragDropItem* pItem, CUIDragDropList* pList);

typedef enum{CELL_EMPTY, CELL_FULL} E_CELL_STATE;


DEF_LIST (DRAG_DROP_LIST, CUIDragDropItem*);


class CUIDragDropList: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	////////////////////////////////////
	//конструктор/деструктор
	CUIDragDropList();
	virtual ~CUIDragDropList();

	virtual void AttachChild(CUIWindow* pChild);
	virtual void DetachChild(CUIWindow* pChild);

	virtual void DetachAll();

	virtual void DropAll();
		
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	virtual void Init(int x, int y, int width, int height);
	virtual void InitGrid(int iRowsNum, int iColsNum, 
						  bool bGridVisible = true, int iViewRowsNum = 0);
	int GetCols() {return m_iColsNum;}
	int GetRows() {return m_iRowsNum;}
	int GetViewRows() {return m_iViewRowsNum;}
	
	//размеры клеточки сетки
	int GetCellWidth() {return m_iCellWidth;}
	int GetCellHeight() {return m_iCellHeight;}
	void SetCellWidth(int iCellWidth) {m_iCellWidth = iCellWidth;}
	void SetCellHeight(int iCellHeight) {m_iCellHeight = iCellHeight;}

	void OnCustomPlacement() {m_bCustomPlacement = true;}
	void OffCustomPlacement() {m_bCustomPlacement = false;}
	bool GetCustomPlacement() {return m_bCustomPlacement;}

	void BlockCustomPlacement() {m_bBlockCustomPlacement = true;}
	void UnblockCustomPlacement() {m_bBlockCustomPlacement = false;}
	bool IsCustomPlacementBlocked() {return m_bBlockCustomPlacement;}


	//дополнительная проверка
	CHECK_PROC GetCheckProc() {return m_pCheckProc;}
	void SetCheckProc(CHECK_PROC proc) {m_pCheckProc = proc;}

	//установление позиции скролинга
	void SetScrollPos(int iScrollPos);
	int GetScrollPos();

	virtual void Draw();
	virtual void Update();

	DRAG_DROP_LIST& GetDragDropItemsList() {return m_DragDropItemsList;}

	void EnableScrollBar(bool enable);
	bool IsScrollBarEnabled() {return m_bScrollBarEnabled;}

	bool CanPlaceItem(CUIDragDropItem *pDDItem);

	void SetItemsScale(float fItemsScale);
	float GetItemsScale() const { return m_fItemsScale; }

	// Подсветить указанную клеточку в листе
	void HighlightCell(int row, int col, bool on);
	void HighlightAllCells(bool on);

	const int GetCurrentFirstRow() const { return m_iCurrentFirstRow; }

	// Отсортировать элементы в листе, чтобы они компактно разместились в клиентской области
	// листа
	void RearrangeItems();

	// Установить режим работы листа: бесконечная/конечная вместимость
	void SetUnlimitedCapacity(bool value) { m_bUnlimitedCapacity = value; }

protected:
	//полоса прокрутки
	CUIScrollBar m_ScrollBar;
	bool m_bScrollBarEnabled;

	bool PlaceItemInGrid(CUIDragDropItem* pItem);
	void RemoveItemFromGrid(CUIDragDropItem* pItem);

	bool CanPlaceItemInGrid(CUIDragDropItem* pItem, int& place_row, int& place_col);
	void PlaceItemAtPos(int place_row, int place_col, CUIDragDropItem* pItem);
	bool CanPlace(int row, int col, CUIDragDropItem* pItem);

	// Для привязки скроллбара к контенту листа необходимо определить самую
	// нижнюю (правую не надо пока, так как в DDList'е HSCROLL не используется) занятую клеточку
	int GetLastBottomFullCell();

	// Пересчитать состояние скроллбара
	// Params:	needScrollToTop	- true если после пересчета состояние необходимо прокрутить скроллбар
	//							на начало	
	void ScrollBarRecalculate(bool needScrollToTop);

	//состояние клеточки в сетке
	E_CELL_STATE& 	GetCell(int row, int col){VERIFY(row<m_iRowsNum && col<m_iColsNum);return m_vGridState[row*GetCols() + col];}
	E_CELL_STATE    GetCellState(int row, int col);

	DEFINE_VECTOR	(E_CELL_STATE, GRID_STATE_VECTOR, GRID_STATE_IT);
    
	//состояние клеточек сетки
	GRID_STATE_VECTOR m_vGridState;

	//для изображения клеточек сетки
	DEFINE_VECTOR	(CUIStatic, CELL_STATIC_VECTOR, CELL_STATIC_IT);
	CELL_STATIC_VECTOR m_vCellStatic;

	//автоматическое расстановка элементов
	bool m_bCustomPlacement;
	//запрещение ручной расстановки
	bool m_bBlockCustomPlacement;

	//размеры сетки для элементов 
	int m_iColsNum;
	int m_iRowsNum;
	//видимая часть рядков в сетке
	int m_iViewRowsNum;

	//для прокрутки листинга
	int m_iCurrentFirstRow;
	void UpdateList();

	//размеры клеточки сетки в пикселях
	int m_iCellWidth;
	int m_iCellHeight;

	//указатель на произвольную функцию
	//необходимую для дополнительной проверки
	//на прием элемента DragDrop себе
	CHECK_PROC m_pCheckProc;

	//список эл-тов DrapDrop присоединенных в данный момент к списку
	DRAG_DROP_LIST m_DragDropItemsList;

	// Для возможности изменения размера лежащих на листе элементов, запомним скейл
	float		m_fItemsScale;

	// Видима ли сетка?
	bool		m_bGridVisible;
	bool		m_bUnlimitedCapacity;
};

#endif //_UI_DRAG_DROP_LIST_H_