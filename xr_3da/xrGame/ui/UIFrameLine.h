//=============================================================================
//  Filename:   UIFrameLine.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ����� ����������� UIFrameRect �� ����������� ����, ��� �� ������������ ���
//	����������� ������������������� ������ ������������� ������������ ������ ���
//	������. � �������� �������� ���������� ���������� 3 ��������: ������(������),
//	�����(�������) � �����������
//=============================================================================

#ifndef UI_FRAME_LINE_H_
#define UI_FRAME_LINE_H_

#pragma once

#include "../UIStaticItem.h"

class CUIFrameLine: public CUICustomItem
{
	enum
	{
		flFirst = 0,	// Left or top
		flSecond,		// Right or bottom
		flBack,			// Center texture
		flMax
	};

	// Drawing elements
	CUIStaticItem	elements[flMax];

	enum
	{
		flValidSize = 1
	};

protected:
	int			iSize;
	Ivector2	iPos;
	u8			uFlags;
	bool		bHorizontalOrientation;

	void		UpdateSize		();
public:
				CUIFrameLine	();
	void		Init			(LPCSTR base_name, int x, int y, int size, bool horizontal, DWORD align);
	void		InitTexture		(const char* texture);
	void		SetColor		(u32 cl);
	IC void		SetPos			(int left, int top)		{ iPos.set(left,top);	uFlags &=~ flValidSize; }
	IC void		SetSize			(int size)				{ iSize = size;			uFlags &=~ flValidSize; }
	IC void		SetOrientation	(bool bIsHorizontal)	{ bHorizontalOrientation = bIsHorizontal; uFlags &=~ flValidSize; }
	void		Render			();
};

#endif	// UI_FRAME_LINE_H_