//=============================================================================
//  Filename:   UIColorAnimatorWrapper.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Оберточка над LightAnimLibrary, в которой реализуется более удобная
//	работа с анимацией, и прикручен апдейт времени специфичный для UI: так
//	как UI апдейтится не на кажом кадре, то кормить функции интерполяции
//	Device.fTimeDelta нельзя - анимация тормознутая получается. Приходится 
//	постоянно считать время между апдейтами.
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
			if (animationTime < (colorAnimation->iFrameCount / colorAnimation->fFPS))
			{
				currColor		= colorAnimation->CalculateBGR(animationTime, currFrame);
//				Msg("frame: %i", dummy);
				currColor		= color_rgba(color_get_B(currColor), color_get_G(currColor), color_get_R(currColor), 255/*color_get_A(currColor)*/);
				// обновим время
				animationTime	+= Device.fTimeGlobal - prevGlobalTime;
				Msg("%i", currFrame);

			}
			// В любом случае (при любом ФПС) последним кадром должен быть последний кадр анимации
			else if (currFrame != colorAnimation->iFrameCount - 1)
			{
				currColor		= colorAnimation->CalculateBGR(colorAnimation->iFrameCount / colorAnimation->fFPS, currFrame);
				currColor		= color_rgba(color_get_B(currColor), color_get_G(currColor), color_get_R(currColor), 255/*color_get_A(currColor)*/);
			}

			prevGlobalTime		= Device.fTimeGlobal;
		}
		else
		{
			currColor	= colorAnimation->CalculateBGR(Device.fTimeGlobal, currFrame);
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