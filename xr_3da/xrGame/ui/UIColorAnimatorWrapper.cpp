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
		color				(NULL),
		isDone				(false),
		reverse				(false),
		kRev				(0.0f)
{
	prevGlobalTime	= Device.fTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////

CUIColorAnimatorWrapper::CUIColorAnimatorWrapper(const shared_str &animationName, u32 *colorToModify)
	:	colorAnimation		(LALib.FindItem(*animationName)),
		animationTime		(0.0f),
		color				(colorToModify),
		isDone				(false),
		reverse				(false),
		kRev				(0.0f)
{
	VERIFY(colorAnimation);
	VERIFY(color);
	prevGlobalTime	= Device.fTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////

CUIColorAnimatorWrapper::CUIColorAnimatorWrapper(const shared_str &animationName)
	:	colorAnimation		(LALib.FindItem(*animationName)),
		animationTime		(0.0f),
		color				(NULL),
		isDone				(false),
		reverse				(false),
		kRev				(0.0f)
{
	VERIFY(colorAnimation);
	prevGlobalTime	= Device.fTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////

void CUIColorAnimatorWrapper::SetColorAnimation(const shared_str &animationName)
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
	if (colorAnimation && !isDone)
	{
		if (!isCyclic)
		{
			if (animationTime < (colorAnimation->iFrameCount / colorAnimation->fFPS))
			{
				currColor		= colorAnimation->CalculateBGR(std::abs(animationTime - kRev), currFrame);
//				Msg("frame: %i", dummy);
				currColor		= color_rgba(color_get_B(currColor), color_get_G(currColor), color_get_R(currColor), color_get_A(currColor));
				// обновим время
				animationTime	+= Device.fTimeGlobal - prevGlobalTime;
			}
			else
			{
				// В любом случае (при любом ФПС) последним кадром должен быть последний кадр анимации
				currColor	= colorAnimation->CalculateBGR((colorAnimation->iFrameCount - 1) / colorAnimation->fFPS - kRev, currFrame);
				currColor	= color_rgba(color_get_B(currColor), color_get_G(currColor), color_get_R(currColor), color_get_A(currColor));
				// Индицируем конец анимации
				isDone = true;
			}
		}
		else
		{
			currColor	= colorAnimation->CalculateBGR(Device.fTimeGlobal, currFrame);
			currColor	= color_rgba(color_get_B(currColor), color_get_G(currColor), color_get_R(currColor), color_get_A(currColor));
		}

		if (color)
		{
			*color		= currColor;
		}
	}

	prevGlobalTime = Device.fTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////

void CUIColorAnimatorWrapper::Reset()
{
	prevGlobalTime	= Device.fTimeGlobal;
	animationTime	= 0.0f;
	isDone			= false;
}

//////////////////////////////////////////////////////////////////////////

int CUIColorAnimatorWrapper::TotalFrames() const
{
	if (colorAnimation)
	{
		return colorAnimation->iFrameCount;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CUIColorAnimatorWrapper::Reverese(bool value)
{
	reverse = value;

	if (value)
	{
		kRev = (colorAnimation->iFrameCount - 1) / colorAnimation->fFPS;
	}
	else
	{
		kRev = 0.0f;
	}

	if (!Done())
	{
		animationTime = colorAnimation->iFrameCount / colorAnimation->fFPS - animationTime;
	}
}