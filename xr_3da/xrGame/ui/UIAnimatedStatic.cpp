//=============================================================================
//  Filename:   UIAnimatedStatic.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Статик для отображения анимированной иконки
//=============================================================================

#include "stdafx.h"
#include "UIAnimatedStatic.h"

//////////////////////////////////////////////////////////////////////////

CUIAnimatedStatic::CUIAnimatedStatic()
	:	m_uFrameCount			(0),
		m_uAnimationDuration	(0),
		m_uTimeElapsed			(0),
		m_uAnimCols				(0xffffffff),
		m_bCyclic				(true),
		m_bParamsChanged		(true),
		m_uFrameWidth			(0),
		m_uFrameHeight			(0),
		m_uCurFrame				(0xffffffff),
		m_bPlaying				(false)
{
	m_pos.set(0,0);
	ClipperOn();
}

//////////////////////////////////////////////////////////////////////////

void CUIAnimatedStatic::Update()
{
	if (!m_bPlaying) return;

	static u32 oneFrameDuration = 0;

	// Пересчитаем пааметры анимации
	if (m_bParamsChanged && 0 != m_uFrameCount)
	{
		// Пересчитаем время одного кадра
		oneFrameDuration = iCeil(m_uAnimationDuration / static_cast<float>(m_uFrameCount));

		SetFrame(0);

		m_bParamsChanged = false;
	}

	// Прибавляем время кадра
	static u32 aa = 0;
	m_uTimeElapsed += Device.dwTimeGlobal - aa;
	aa = Device.dwTimeGlobal;

	// Если анимация закончилась
	if (m_uTimeElapsed > m_uAnimationDuration)
	{
		Rewind(m_uAnimationDuration - m_uAnimationDuration);
		if (!m_bCyclic)
			Stop();
	}

	// Теперь вычисляем кадры в зависимости от времени
	u32 curFrame = m_uTimeElapsed / oneFrameDuration;

	if (curFrame != m_uCurFrame)
	{
		m_uCurFrame = curFrame;
		SetFrame(m_uCurFrame);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIAnimatedStatic::SetFrame(const u32 frameNum)
{
	//static u32 currRow = 0xffffffff, currCol = 0xffffffff;
	int currRow = frameNum / m_uAnimCols;
	int currCol = frameNum % m_uAnimCols;
	GetUIStaticItem().SetOriginalRect(m_pos.x + float(currCol*m_uFrameWidth), m_pos.y + float(currRow*m_uFrameHeight), float(m_uFrameWidth), float(m_uFrameHeight));
}