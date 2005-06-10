//=============================================================================
//  Filename:   UIIconedListItem.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ���� ����� � ������� � ������ ������ ������ ������
//=============================================================================

#ifndef UI_ICONED_LIST_ITEM_H_
#define UI_ICONED_LIST_ITEM_H_

#pragma once

#include "UIListItem.h"
#include "../UIStaticItem.h"

//////////////////////////////////////////////////////////////////////////

class CUIIconedListItem: public CUIListItem
{
	typedef CUIListItem inherited;
public:
	CUIIconedListItem(): m_uIconSize		(0),
						 m_bTexturePresent	(false),
						 m_bIconDraw		(true)
	{}

	// ������������� ������ � �� ������
	void			SetIcon(shared_str textureName, u32 uIconSize);
	virtual void	Draw();
	virtual void	Show(bool status);
	// ����� ����� ������� ����� �������� ��������� ��������
	virtual void	Update();

	// �������� ������
	void SetIconOffset(const float oX, const float oY);
protected:
	CUIStaticItem	m_Icon;
	// ������ ������
	u32				m_uIconSize;
	// ������ ������������
	bool			m_bTexturePresent;
	// ������ ������������
	bool			m_bIconDraw;
	// ���������� ���������� AbsoluteRect
	Frect			m_OldAbsoluteWndRect;
};

#endif	//UI_ICONED_LIST_ITEM_H_