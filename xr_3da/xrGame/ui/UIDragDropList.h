// UIDragDropList.h: список элементов Drag&Drop
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_DRAG_DROP_LIST_H_
#define _UI_DRAG_DROP_LIST_H_


#pragma once


#include "uiframewindow.h"
#include "uidragdropitem.h"

#include "uistatic.h"
class CUIDragDropList;

typedef bool (*CHECK_PROC)(CUIDragDropItem* pItem, CUIDragDropList* pList);

typedef enum{CELL_EMPTY, CELL_FULL} E_CELL_STATE;

class CUIDragDropList: public CUIWindow
{
public:
	////////////////////////////////////
	//конструктор/деструктор
	CUIDragDropList();
	virtual ~CUIDragDropList();


	void AttachChild(CUIDragDropItem* pChild);
	void DetachChild(CUIDragDropItem* pChild);
	void DropAll();
		
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	
	virtual void InitGrid(int iRowsNum, int iColsNum, bool bGridVisible = true);
	int GetCols() {return m_iColsNum;}
	int GetRows() {return m_iRowsNum;}
	
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


protected:

	bool PlaceItemInGrid(CUIDragDropItem* pItem);
	void RemoveItemFromGrid(CUIDragDropItem* pItem);

	bool CanPlace(int row, int col, CUIDragDropItem* pItem);

	//состояние клеточки в сетке
	E_CELL_STATE& 	GetCell(int row, int col){return m_vGridState[row*GetCols() + col];} 
	E_CELL_STATE    GetCellState(int row, int col);
						

	DEFINE_VECTOR	(E_CELL_STATE, GRID_STATE_VECTOR, GRID_STATE_IT);
    
	//состояние клеточек сетки
	GRID_STATE_VECTOR m_vGridState;

	//для изображения клеточек сетки
	DEFINE_VECTOR	(CUIStatic, CELL_STATIC_VECTOR, CELL_STATIC_IT);
	CELL_STATIC_VECTOR m_vCellStatic;

	//автоматическое размещение элементов 
	bool m_bCustomPlacement;
	//запрещение ручного размещения
	bool m_bBlockCustomPlacement;

	//размеры сетки для элементов
	int m_iColsNum;
	int m_iRowsNum;

	//размеры клеточки сетки в пикслях
	int m_iCellWidth;
	int m_iCellHeight;


	

	//указатель на произвольную функцию
	//необходимую для дополнительной проверки 
	//на прием элемента DragDrop себе
	CHECK_PROC m_pCheckProc;


};


#endif //_UI_DRAG_DROP_LIST_H_