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

class CUIListWnd;

class CUITreeViewItem: public CUIListItem
{
	typedef CUIListItem inherited;
	// �������� �� �� ������� �����������
	bool			isRoot;
	// ���� �� ���, �� ���� ���� ���������� ������� ���� ����������� ��� ���
	bool			isOpened;
	// �������� � ��������
	int				iTextShift;
	// ���� �� �����������
	CUITreeViewItem *pOwner;
public:
	void			SetRoot(bool set);
	bool			IsRoot() const						{ return isRoot; }

	// ������������� �������� ������
	void			SetTextShift(int delta)				{ iTextShift += delta; }

	// ��������
	CUITreeViewItem * GetOwner() const					{ return pOwner; }
	void			SetOwner(CUITreeViewItem *owner)	{ pOwner = owner; }
protected:
	// ������� ���������� ��� ��������� �������� ����
	// ��� ��������� ����������� ������������� ����
	virtual void	OnRootChanged();
public:
	// ��������/�������� ��������� ��������
	void			Open();
	void			Close();
	bool			IsOpened() const					{ return isOpened; }
protected:
	// ������� ���������� ��� ��������� c�������� �������/������� 
	// ��� ��������� ����������� ������������� ����
	virtual void	OnOpenClose();
public:
    
	// ������ ���������, ������� ������� ���� ���
	typedef			xr_vector<CUITreeViewItem*>		SubItems;
	typedef			SubItems::iterator				SubItems_it;
	SubItems		vSubItems;

	CUIStatic		UIBkg;

	// �������� �������
	void AddItem(CUITreeViewItem *pItem);
	// ������� ���
	void DeleteAllSubItems();
	// ����� ������� � �������� ������
	// Return:	��������� �� �������, ���� �����, ���� NULL � ��������� ������
	CUITreeViewItem * Find(LPCSTR text) const;
	// ����� �������� �������
	// Return:	��������� �� �������, ���� �����, ���� NULL � ��������� ������
	CUITreeViewItem * Find(CUITreeViewItem *pItem) const;
	// ������� �������� �� ����� �� �������� �������� � ���� ������-����
	// ������� �������� ������������� �������� "/"
	std::string GetHierarchyAsText();

	// Redefine some stuff
	// ATTENTION! ��� ����������� ���������������� ������� [+-] ����� SetText
	// ������ �������������� SetRoot
	virtual void SetText(LPCSTR str);
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData);

	// Ctor and Dtor
	CUITreeViewItem();
	~CUITreeViewItem();
};

//////////////////////////////////////////////////////////////////////////
//  Function for automatic tree hierarchy creation
//////////////////////////////////////////////////////////////////////////

DEF_VECTOR(GroupTree, ref_str);

//////////////////////////////////////////////////////////////////////////

void CreateTreeBranch(ref_str nestingTree, ref_str leafName, CUIListWnd *pListToAdd, int leafProperty,
					  CGameFont *pRootFont, u32 rootColor, CGameFont *pLeafFont, u32 leafColor);

#endif	//UI_TREE_VIEW_ITEM_H_