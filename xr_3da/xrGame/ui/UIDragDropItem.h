// UIDragDropItem.h: ����� ��������������� ��������
//
//////////////////////////////////////////////////////////////////////

#ifndef _UI_DRAG_DROP_ITEM_H_
#define _UI_DRAG_DROP_ITEM_H_

#pragma once


#include "uibutton.h"



class CUIDragDropItem : public CUIButton
{
public:
	CUIDragDropItem();
	virtual ~ CUIDragDropItem();


	virtual void Init(LPCSTR tex_name, int x, int y, int width, int height);

	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	//���������, ������������ ������������� ����
	
	//��������� ����� ���� �� TOP ��� ��� ����� 
	//���������� �� ������������ (���������! � ������������� ���
	//��������� ���-�� � ���������� ENUM)
	typedef enum{BUTTON_CLICKED, 
					ITEM_DRAG = 0xFF0, 
					ITEM_DROP, 
					ITEM_MOVE,
					ITEM_DB_CLICK} E_MESSAGE;


	virtual void Draw();
	virtual void Update();

	void EnableDragDrop(bool bEnable) {m_bDDEnabled = bEnable;}

	POINT GetPreviousPos() {return m_previousPos;}


	//������� � ����� Drag&Drop
	void SetGridWidth(int iGridWidth) {m_iGridWidth = iGridWidth;}
	void SetGridHeight(int iGridHeight) {m_iGridHeight = iGridHeight;}
	int GetGridWidth() {return m_iGridWidth;}
	int GetGridHeight() {return m_iGridHeight;}

	//��������� � �����
	int GetGridRow() {return m_iGridRow;}
	int GetGridCol() {return m_iGridCol;}
	void SetGridRow(int iGridRow) {m_iGridRow = iGridRow;}
	void SetGridCol(int iGridCol) {m_iGridCol = iGridCol;}

	void* GetData() {return m_pData;}
	void SetData(void* pData) {m_pData = pData;}


protected:
	
	int m_iOldMouseX;
	int m_iOldMouseY;

	//������� �� drag drop
	bool m_bDDEnabled;

	POINT m_previousPos;

	//������ � ��������� � �����
	//��������� Drag&Drop 
	int m_iGridWidth;
	int m_iGridHeight;

	int m_iGridCol;
	int m_iGridRow;


	//��������� �� ������������ ������ ������� ����� ����
	//������������ � ��������
	void* m_pData; 
};

#endif // _UI_DRAG_DROP_ITEM_H_
