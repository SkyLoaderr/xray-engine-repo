//=============================================================================
//  Filename:   UIFrameLine.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Класс аналогичный UIFrameRect за исключением того, что он предназначен для
//	отображения затекстурированного узкого прямоуголника произвольной длинны или
//	ширины. В качестве исходных материалов необходимо 3 текстуры: правая(нижняя),
//	левая(верхняя) и центральная
//=============================================================================

#include "stdafx.h"
#include "UIFrameLine.h"
#include "../hudmanager.h"

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
	// init graphics
	string256		buf;
	// frame
	elements[flBack].CreateShader	(strconcat(buf,base_name,"_back"),	"hud\\default");
	elements[flFirst].CreateShader	(strconcat(buf,base_name,"_b"),		"hud\\default");
	elements[flSecond].CreateShader	(strconcat(buf,base_name,"_e"),		"hud\\default");
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
	CTexture	*T;

//	float		fScale = HUD().GetScale();

	// Left or top texture
	RCache.set_Shader(elements[flFirst].GetShader());
	T				= RCache.get_ActiveTexture(0);
	int f_width		= static_cast<int>(T->get_Width());
	int f_height	= static_cast<int>(T->get_Height());
	elements[flFirst].SetPos(iPos.x, iPos.y);

	// Right or bottom texture
	RCache.set_Shader(elements[flSecond].GetShader());
	T				= RCache.get_ActiveTexture(0);
	int s_width		= static_cast<int>(T->get_Width());
	int s_height	= static_cast<int>(T->get_Height());
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

	RCache.set_Shader(elements[flBack].GetShader());
	T				= RCache.get_ActiveTexture(0);
	int b_width		= static_cast<int>(T->get_Width());
	int b_height	= static_cast<int>(T->get_Height());

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