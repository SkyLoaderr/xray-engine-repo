//=============================================================================
//  Filename:   UIGlobalMapLocation.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Класс cлужит для отображения прямоугольничка локальной карты на глобальной
//=============================================================================

#ifndef UI_GLOBAL_MAP_LOCATION_H_
#define UI_GLOBAL_MAP_LOCATION_H_

#pragma once

#include "UIFrameWindow.h"
#include "UIMapSpot.h"

//////////////////////////////////////////////////////////////////////////

class CUIGlobalMapLocation: public CUIMapSpot
{
	typedef CUIButton inherited;
public:
	// Ctor and Dtor
	CUIGlobalMapLocation();
	virtual ~CUIGlobalMapLocation();

	virtual void Init(int width, int height, RECT clipRect);
	void Draw();
protected:
	// Фрейм для оторажения рамочки
	CUIFrameWindow	UIBorder;

	// Имя карты
	ref_str			m_strMapName;
	// Cписок локаций
};


#endif //UI_GLOBAL_MAP_LOCATION_H_