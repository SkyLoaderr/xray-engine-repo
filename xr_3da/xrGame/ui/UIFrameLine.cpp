//=============================================================================
//  Filename:   UIFrameLine.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ����� ����������� UIFrameRect �� ����������� ����, ��� �� ������������ ���
//	����������� ������������������� ������ ������������� ������������ ������ ���
//	������. � �������� �������� ���������� ���������� 3 ��������: ������(������),
//	�����(�������) � �����������
//=============================================================================

#include "stdafx.h"
#include "UIFrameLine.h"
#include "../hudmanager.h"
#include "UITextureMaster.h"

//////////////////////////////////////////////////////////////////////////

CUIFrameLine::CUIFrameLine()
	:	uFlags					(0),
		iSize					(0),
		bHorizontalOrientation	(true)
{
	iPos.set(0, 0);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLine::Init(LPCSTR base_name, int x, int y, int size, bool horizontal, DWORD align)
{
	SetPos			(x, y);
	SetSize			(size);
	SetAlign		(align);
	SetOrientation	(horizontal);

	InitTexture(base_name);
}

void CUIFrameLine::InitTexture(const char* texture){
	string256		buf;

	CUITextureMaster::InitTexture(strconcat(buf,texture,"_back"),	&elements[flBack]);
	CUITextureMaster::InitTexture(strconcat(buf,texture,"_b"),		&elements[flFirst]);
	CUITextureMaster::InitTexture(strconcat(buf,texture,"_e"),		&elements[flSecond]);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLine::SetColor(u32 cl)
{
	for (int i = 0; i < flMax; ++i)
		elements[i].SetColor(cl);
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLine::UpdateSize()
{
	VERIFY(g_bRendering);
//	CTexture	*T;

//	float		fScale = HUD().GetScale();

	// Left or top texture
//	RCache.set_Shader(elements[flFirst].GetShader());
//	T				= RCache.get_ActiveTexture(0);
	int f_width		= elements[flFirst].GetOriginalRect().width();//static_cast<int>(T->get_Width());
	int f_height	= elements[flFirst].GetOriginalRect().height();//static_cast<int>(T->get_Height());
	elements[flFirst].SetPos(iPos.x, iPos.y);

	// Right or bottom texture
//	RCache.set_Shader(elements[flSecond].GetShader());
//	T				= RCache.get_ActiveTexture(0);
	int s_width		= elements[flSecond].GetOriginalRect().width();// static_cast<int>(T->get_Width());
	int s_height	= elements[flSecond].GetOriginalRect().height();//static_cast<int>(T->get_Height());
	bHorizontalOrientation ?
		elements[flSecond].SetPos(iPos.x + iSize - s_width, iPos.y) :
		elements[flSecond].SetPos(iPos.x, iPos.y + iSize - s_height);

	// Dimentions of element textures must be the same
	if (bHorizontalOrientation)
		R_ASSERT(s_height == f_height);
	else
		R_ASSERT(f_width == s_width);


	// Now stretch back texture to remaining space
	int back_width, back_height;

	if (bHorizontalOrientation)
	{
		back_width	= iSize - f_width - s_width;
		back_height	= f_height;

		// Size of frameline must be equal or greater than sum of size of two side textures
		R_ASSERT(back_width > 0);
	}
	else
	{
		back_width	= f_width;
		back_height	= iSize - f_height - s_height;

		// Size of frameline must be equal or greater than sum of size of two side textures
		R_ASSERT(back_height > 0);
	}

	// Now resize back texture
	int rem, tile;

//	RCache.set_Shader(elements[flBack].GetShader());
//	T				= RCache.get_ActiveTexture(0);
	int b_width		= elements[flBack].GetOriginalRect().width();//static_cast<int>(T->get_Width());
	int b_height	= elements[flBack].GetOriginalRect().height();//static_cast<int>(T->get_Height());

	if (bHorizontalOrientation)
	{
		rem			= back_width % b_width;
		tile		= iFloor(static_cast<float>(back_width) / b_width);	
		elements[flBack].SetPos(iPos.x + f_width, iPos.y);
		elements[flBack].SetTile(tile, 1, rem, 0);
	}
	else
	{
		rem			= back_height % b_height;
		tile		= iFloor(static_cast<float>(back_height) / b_height);
		elements[flBack].SetPos(iPos.x, iPos.y + f_height);
		elements[flBack].SetTile(1, tile, 0, rem);
	}

	uFlags |= flValidSize;
}

//////////////////////////////////////////////////////////////////////////

void CUIFrameLine::Render()
{
	// If size changed - update size
	if (!(uFlags & flValidSize)) UpdateSize();
	// Now render all statics
	for (int i = 0; i < flMax; ++i)
	{
		elements[i].Render();
	}
}