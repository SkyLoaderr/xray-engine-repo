//=============================================================================
//  Filename:   UIGlobalMapLocation.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ����� c����� ��� ����������� ��������������� ��������� ����� �� ����������
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
	// ����� ��� ���������� �������
	CUIFrameWindow	UIBorder;

	// ��� �����
	ref_str			m_strMapName;
	// C����� �������
};


#endif //UI_GLOBAL_MAP_LOCATION_H_