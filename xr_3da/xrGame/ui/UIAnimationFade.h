//=============================================================================
//  Filename:   UIAnimationFade.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ��� �������� �������� ��������/���������� ��������
//=============================================================================

#ifndef UI_ANIMATION_FADE_H_
#define UI_ANIMATION_FADE_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include <utility>
#include "UIAnimationBase.h"

//////////////////////////////////////////////////////////////////////////

class CUIAnimationFade: public CUIAnimationBase
{
	typedef	CUIAnimationBase inherited;
public:
	CUIAnimationFade();
	virtual				~CUIAnimationFade() {}
	// Update
	virtual void		Update();

	typedef				std::pair<int, int> FadeBounds;
	// ���������� ������� �����
	void				SetFadeBounds(FadeBounds bounds) { VERIFY(bounds.first < bounds.second); m_FadeBounds = bounds; }

	// ����������� �����
	enum EFadeDirection
	{
		efdFadeIn = 0,
		efdFadeOut
	};

	// ��������� ����������� ��������
	void				SetAnimationDirection(EFadeDirection dir) { m_eCurrFadeDirection = dir; }

protected:
	// ������� �����
	FadeBounds			m_FadeBounds;
	// ������� ����������� �����
	EFadeDirection		m_eCurrFadeDirection;
	// ������� ����������� �����
	void				SwitchFadeDirection();
};

#endif	//UI_ANIMATION_FADE_H_