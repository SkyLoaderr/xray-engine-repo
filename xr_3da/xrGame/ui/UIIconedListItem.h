//=============================================================================
//  Filename:   UIIconedListItem.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Лист айтем с иконкой в начале первой строки текста
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

	// Устанавливаем иконку и ее размер
	void SetIcon(ref_str textureName, u32 uIconSize);
	virtual void Draw();
protected:
	CUIStaticItem	m_Icon;
	// Размер иконки
	u32				m_uIconSize;
	// Иконка присутствует
	bool			m_bTexturePresent;
};

#endif	//UI_ICONED_LIST_ITEM_H_