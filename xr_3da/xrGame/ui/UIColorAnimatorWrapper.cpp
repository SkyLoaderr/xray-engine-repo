//=============================================================================
//  Filename:   UIColorAnimatorWrapper.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ��������� ��� LightAnimLibrary, � ������� ����������� ����� �������
//	������ � ���������, � ��������� ������ ������� ����������� ��� UI: ���
//	��� UI ���������� �� �� ����� �����, �� ������� ������� ������������
//	Device.fTimeDelta ������ - �������� ����������� ����������. ���������� 
//	��������� ������� ����� ����� ���������.
//=============================================================================

#include "stdafx.h"
#include "UIColorAnimatorWrapper.h"
#include "../../LightAnimLibrary.h"

//////////////////////////////////////////////////////////////////////////

CUIColorAnimatorWrapper::CUIColorAnimatorWrapper()
	:	colorAnimation		(NULL),
		animationTime		(0.0f),
		color				(NULL)
{
	prevGlobalTime	= Device.fTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////

CUIColorAnimatorWrapper::CUIColorAnimatorWrapper(const ref_str &animationName, u32 *colorToModify)
	:	colorAnimation		(LALib.FindItem(*animationName)),
		animationTime		(0.0f),
		color				(colorToModify)
{
	VERIFY(colorAnimation);
	VERIFY(color);
	prevGlobalTime	= Device.fTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////

void CUIColorAnimatorWrapper::SetColorAnimation(const ref_str &animationName)
{
	if (animationName.size() != 0)
	{
		colorAnimation	= LALib.FindItem(*animationName);
		R_ASSERT2(colorAnimation, *animationName);
	}
	else
	{
		colorAnimation	= NULL;
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIColorAnimatorWrapper::SetColorToModify(u32 *colorToModify)
{
	color = colorToModify;
}

//////////////////////////////////////////////////////////////////////////

void CUIColorAnimatorWrapper::Update()
{
	if (colorAnimation)
	{
		if (!isCyclic)
		{
			if (animationTime <= (colorAnimation->iFrameCount / colorAnimation->fFPS))
			{
				int	dummy		= 0;
				currColor		= colorAnimation->CalculateBGR(animationTime, dummy);
//				Msg("frame: %i", dummy);
				currColor		= color_rgba(color_get_B(currColor), color_get_G(currColor), color_get_R(currColor), 255/*color_get_A(currColor)*/);
				// ������� �����
				animationTime	+= Device.fTimeGlobal - prevGlobalTime;
			}
			prevGlobalTime		= Device.fTimeGlobal;
		}
		else
		{
			int dummy	= 0;
			currColor	= colorAnimation->CalculateBGR(Device.fTimeGlobal, dummy);
			currColor	= color_rgba(color_get_B(currColor), color_get_G(currColor), color_get_R(currColor), 255/*color_get_A(currColor)*/);
		}

		if (color)
		{
			*color		= currColor;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIColorAnimatorWrapper::Reset() const
{
	prevGlobalTime	= Device.fTimeGlobal;
	animationTime	= 0.0f;
}