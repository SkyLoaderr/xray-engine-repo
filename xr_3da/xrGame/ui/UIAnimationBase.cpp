//=============================================================================
//  Filename:   UIAnimationBase.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Абстрактный класс анимаций для различных потребностей интерфейса
//=============================================================================

#include "stdafx.h"
#include "UIAnimationBase.h"

//////////////////////////////////////////////////////////////////////////

CUIAnimationBase::CUIAnimationBase()
	:	m_iAnimationPeriod		(0),
		m_iPhase				(0),
		m_uPrevGlobalTime		(0),
		m_eAnimationState		(easStopped),
		m_iCurrAnimationTime	(0)
{
}

//////////////////////////////////////////////////////////////////////////

CUIAnimationBase::~CUIAnimationBase()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIAnimationBase::Update()
{
	// Update time
	m_uPrevGlobalTime = Device.dwTimeGlobal;	
}

//////////////////////////////////////////////////////////////////////////

void CUIAnimationBase::Play()
{
	if (0 == m_uPrevGlobalTime) m_uPrevGlobalTime = Device.dwTimeGlobal;
	m_eAnimationState = easPlayed;
}