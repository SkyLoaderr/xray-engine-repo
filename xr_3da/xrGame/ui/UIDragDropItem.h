// UIDragDropItem.h: класс перетаскиваемой картинки
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


	virtual void OnMouse(int x, int y, E_MOUSEACTION mouse_action);

	//сообщения, отправляемые родительскому окну
	
	//сообщения будут идти на TOP так что нужно 
	//обеспечить их уникальность (исправить! и пронумеровать все
	//сообщения где-то в глобальном ENUM)
	typedef enum{BUTTON_CLICKED, 
					ITEM_DRAG = 0xFF0, 
					ITEM_DROP, 
					ITEM_MOVE} E_MESSAGE;


	virtual void Draw();
	virtual void Update();

	void EnableDragDrop(bool bEnable) {m_bDDEnabled = bEnable;}

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
};

#endif // _UI_DRAG_DROP_ITEM_H_
