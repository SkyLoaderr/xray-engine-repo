//=============================================================================
//  Filename:   UITreeViewItem.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  TreeView Item class
//=============================================================================

#ifndef UI_TREE_VIEW_ITEM_H_
#define UI_TREE_VIEW_ITEM_H_

#pragma once

#include "UIListItem.h"

class CUITreeViewItem: public CUIListItem
{
	typedef CUIListItem inherited;
	// Являемся ли мы началом подиерархии
	bool			isRoot;
	// Если мы рут, то этот флаг показывает открыта наша подиерархия или нет
	bool			isOpened;
	// Смещение в пробелах
	int				iTextShift;
public:
	void			SetRoot(bool set);
	bool			IsRoot() const			{ return isRoot; }

	// Устанавливаем смещение текста
	void			SetTextShift(int delta)	{ iTextShift += delta; }
protected:
	// Функция вызываемая при изменении свойства рута
	// для изменения визуального представления себя
	virtual void	OnRootChanged();
public:
	// Раксрыть/свернуть локальнцю иерархию
	void			Open();
	void			Close();
	bool			IsOpened() const		{ return isOpened; }
protected:
	// Функция вызываемая при изменении cостояния открыто/закрыто 
	// для изменения визуального представления себя
	virtual void	OnOpenClose();
public:
    
	// Список элементов, которые уровнем ниже нас
	typedef			xr_vector<CUITreeViewItem*>		SubItems;
	typedef			SubItems::iterator				SubItems_it;
	SubItems		vSubItems;

	// Добавить элемент
	void AddItem(CUITreeViewItem *pItem);
	// Удалить все
	void DeleteAllSubItems();
	// Найти элемент с заданным именем
	// Return:	указатель на элемент, если нашли, либо NULL в противном случае
	CUITreeViewItem * Find(LPCSTR text) const;

	// Redefine some stuff
	// ATTENTION! Для корректного функционирования значков [+-] вызов SetText
	// Должен предшествовать SetRoot
	virtual void SetText(LPCSTR str);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);

	// Ctor and Dtor
	CUITreeViewItem();
	~CUITreeViewItem();
};

#endif	//UI_TREE_VIEW_ITEM_H_