// UIDragDropList.h: ������ ��������� Drag&Drop
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

class CUIDragDropList: public CUIWindow
{
private:
	typedef CUIWindow inherited;
public:
	////////////////////////////////////
	//�����������/����������
	CUIDragDropList();
	virtual ~CUIDragDropList();

	virtual void AttachChild(CUIDragDropItem* pChild);
	virtual void DetachChild(CUIDragDropItem* pChild);
	
	virtual void AttachChild(CUIWindow* pChild);
	virtual void DetachChild(CUIWindow* pChild);

	void DropAll();
		
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);

	virtual void Init(int x, int y, int width, int height);
	virtual void InitGrid(int iRowsNum, int iColsNum, 
						  bool bGridVisible = true, int iViewRowsNum = 0);
	int GetCols() {return m_iColsNum;}
	int GetRows() {return m_iRowsNum;}
	int GetViewRows() {return m_iViewRowsNum;}
	
	//������� �������� �����
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


	//�������������� ��������
	CHECK_PROC GetCheckProc() {return m_pCheckProc;}
	void SetCheckProc(CHECK_PROC proc) {m_pCheckProc = proc;}

	//������������ ������� ���������
	void SetScrollPos(int iScrollPos);
	int GetScrollPos();

	virtual void Draw();
	virtual void Update();
protected:
	//������ ���������
	CUIScrollBar m_ScrollBar;

	bool PlaceItemInGrid(CUIDragDropItem* pItem);
	void RemoveItemFromGrid(CUIDragDropItem* pItem);

	bool CanPlace(int row, int col, CUIDragDropItem* pItem);

	//��������� �������� � �����
	E_CELL_STATE& 	GetCell(int row, int col){return m_vGridState[row*GetCols() + col];}
	E_CELL_STATE    GetCellState(int row, int col);

	DEFINE_VECTOR	(E_CELL_STATE, GRID_STATE_VECTOR, GRID_STATE_IT);
    
	//��������� �������� �����
	GRID_STATE_VECTOR m_vGridState;

	//��� ����������� �������� �����
	DEFINE_VECTOR	(CUIStatic, CELL_STATIC_VECTOR, CELL_STATIC_IT);
	CELL_STATIC_VECTOR m_vCellStatic;

	//�������������� ����������� ���������
	bool m_bCustomPlacement;
	//���������� ������ �����������
	bool m_bBlockCustomPlacement;

	//������� ����� ��� ��������� 
	int m_iColsNum;
	int m_iRowsNum;
	//������� ����� ������ � �����
	int m_iViewRowsNum;

	//��� ��������� ��������
	int m_iCurrentFirstRow;
	void UpdateList();

	//������� �������� ����� � ��������
	int m_iCellWidth;
	int m_iCellHeight;

	//��������� �� ������������ �������
	//����������� ��� �������������� ��������
	//�� ����� �������� DragDrop ����
	CHECK_PROC m_pCheckProc;
};

#endif //_UI_DRAG_DROP_LIST_H_