// UIDragDropList.h: список элементов Drag&Drop
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_DRAG_DROP_LIST_H_
#define _UI_DRAG_DROP_LIST_H_


#pragma once


#include "uiframewindow.h"
#include "uidragdropitem.h"

#include "uistatic.h"


class CUIDragDropList: public CUIWindow
{
public:
	////////////////////////////////////
	//конструктор/деструктор
	CUIDragDropList();
	virtual ~CUIDragDropList();


	void AttachChild(CUIDragDropItem* pChild);
	void DetachChild(CUIDragDropItem* pChild);
		
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	
	virtual void InitGrid(int iRowsNum, int iColsNum, bool bGridVisible = true);
	int GetCols() {return m_iColsNum;}
	int GetRows() {return m_iRowsNum;}
	
	//размеры клеточки сетки
	int GetCellWidth() {return m_iCellWidth;}
	int GetCellHeight() {return m_iCellHeight;}
	void SetCellWidth(int iCellWidth) {m_iCellWidth = iCellWidth;}
	void SetCellHeight(int iCellHeight) {m_iCellHeight = iCellHeight;}

protected:
	
	bool PlaceItemInGrid(CUIDragDropItem* pItem);
	void RemoveItemFromGrid(CUIDragDropItem* pItem);
	

	typedef enum{CELL_EMPTY, CELL_FULL} E_CELL_STATE;

	//состояние клеточки в сетке
	E_CELL_STATE& 	GetCell(int row, int col) 
					{return m_vGridState[row*GetCols() + col];}

		

	DEFINE_VECTOR	(E_CELL_STATE, GRID_STATE_VECTOR, GRID_STATE_IT);
    
	//состояние клеточек сетки
	GRID_STATE_VECTOR m_vGridState;

	//для изображения клеточек сетки
	DEFINE_VECTOR	(CUIStatic, CELL_STATIC_VECTOR, CELL_STATIC_IT);
	CELL_STATIC_VECTOR m_vCellStatic;

		
	//размеры сетки для элементов
	int m_iColsNum;
	int m_iRowsNum;

	//размеры клеточки сетки в пикслях
	int m_iCellWidth;
	int m_iCellHeight;

};


#endif //_UI_DRAG_DROP_LIST_H_