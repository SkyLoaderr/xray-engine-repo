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
						 m_bTexturePresent	(false)
	{}

	// ������������� ������ � �� ������
	void			SetIcon(ref_str textureName, u32 uIconSize);
	virtual void	Draw();
	virtual void	Show(bool status);
	// ����� ����� ������� ����� �������� ��������� ��������
	virtual void	Update();

	// �������� ������
	void SetIconOffset(const int oX, const int oY);
protected:
	CUIStaticItem	m_Icon;
	// ������ ������
	u32				m_uIconSize;
	// ������ ������������
	bool			m_bTexturePresent;
	// ������ ������������
	bool			m_bIconDraw;
	// ���������� ���������� AbsoluteRect
	RECT			m_OldAbsoluteWndRect;
};

#endif	//UI_ICONED_LIST_ITEM_H_