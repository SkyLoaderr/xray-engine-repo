// UIDragDropItem.h: класс перетаскиваемой картинки
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_DRAG_DROP_ITEM_H_
#define _UI_DRAG_DROP_ITEM_H_

#pragma once


#include "uibutton.h"
#include "UIInventoryUtilities.h"

class CUIDragDropItem;

typedef void (*CUSTOM_UPDATE_PROC)(CUIDragDropItem* pItem);
typedef void (*CUSTOM_DRAW_PROC)(CUIDragDropItem* pItem);


class CUIDragDropItem : public CUIButton
{
public:
	CUIDragDropItem();
	virtual ~ CUIDragDropItem();


	virtual void Init(LPCSTR tex_name, int x, int y, int width, int height);

	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	//сообщения, отправляемые родительскому окну
	
	//сообщения будут идти на TOP так что нужно 
	//обеспечить их уникальность (исправить! и пронумеровать все
	//сообщения где-то в глобальном ENUM или отслеживать через DynamicCast)
	typedef enum{BUTTON_CLICKED, 
					ITEM_DRAG = 0xFF0, 
					ITEM_DROP, 
					ITEM_MOVE,
					ITEM_DB_CLICK,
					ITEM_RBUTTON_CLICK} E_MESSAGE;


	virtual void Draw();
	virtual void Update();

	void EnableDragDrop(bool bEnable)	{m_bDDEnabled = bEnable;}
	bool IsDragDropEnabled() const		{ return m_bDDEnabled; }

	POINT GetPreviousPos() {return m_previousPos;}


	//размеры в сетке Drag&Drop
	void SetGridWidth(int iGridWidth) {m_iGridWidth = iGridWidth;}
	void SetGridHeight(int iGridHeight) {m_iGridHeight = iGridHeight;}
	int GetGridWidth() {return m_iGridWidth;}
	int GetGridHeight() {return m_iGridHeight;}

	//положение в сетке
	int GetGridRow() {return m_iGridRow;}
	int GetGridCol() {return m_iGridCol;}
	void SetGridRow(int iGridRow) {m_iGridRow = iGridRow;}
	void SetGridCol(int iGridCol) {m_iGridCol = iGridCol;}

	virtual void* GetData() {return m_pData;}
	virtual void SetData(void* pData) {m_pData = pData;}

	void SetCustomUpdate(CUSTOM_UPDATE_PROC pCustomUpdateProc) 
								{m_pCustomUpdateProc = pCustomUpdateProc;}
	void SetCustomDraw(CUSTOM_DRAW_PROC pCustomDrawProc) 
								{m_pCustomDrawProc = pCustomDrawProc;}

	//размеры клеточки сетки
	int GetCellWidth() {return m_iCellWidth;}
	int GetCellHeight() {return m_iCellHeight;}
	void SetCellWidth(int iCellWidth) {m_iCellWidth = iCellWidth;}
	void SetCellHeight(int iCellHeight) {m_iCellHeight = iCellHeight;}

	bool m_bInFloat;

	// Специальные функции для перемещения вещи посредством посылки сообщения
	// ITEM_DROP даже если она не находится над контролом в который бросают.
	// Используется для принудительного вытаскивания вещи из слота.
	void MoveOnNextDrop() { m_bMoveOnNextDrop = true; }
	// Проверка необходимости перемещения вещи без проверки находжения вещи над контролом
	bool NeedMoveWithoutRectCheck() { return m_bMoveOnNextDrop ? m_bMoveOnNextDrop = false, true : false; }

protected:
	
	int m_iOldMouseX;
	int m_iOldMouseY;

	//включен ли drag drop
	bool m_bDDEnabled;

	POINT m_previousPos;

	//размер и положение в сетке
	//элементов Drag&Drop 
	int m_iGridWidth;
	int m_iGridHeight;

	int m_iGridCol;
	int m_iGridRow;

	//размеры клеточки сетки в пикселях
	int m_iCellWidth;
	int m_iCellHeight;


	//указатель на произвольные данные которые могут быть
	//присоеденены к элементу
	void* m_pData; 

	CUSTOM_UPDATE_PROC m_pCustomUpdateProc;
	CUSTOM_DRAW_PROC m_pCustomDrawProc;

	// Признак перемещения вещи в другой слот, при следующей посылке ITEM_DROP сообщения.
	// см. функцию MoveOnNextDrop()
	bool m_bMoveOnNextDrop;
};

#endif // _UI_DRAG_DROP_ITEM_H_
