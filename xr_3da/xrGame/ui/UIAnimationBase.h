//=============================================================================
//  Filename:   UIAnimationBase.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ����������� ����� �������� ��� ��������� ������������ ����������
//=============================================================================

#ifndef UI_ANIMATION_BASE_H_
#define UI_ANIMATION_BASE_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIWindow.h"

//////////////////////////////////////////////////////////////////////////

class CUIAnimationBase
{
public:
	CUIAnimationBase();
	virtual ~CUIAnimationBase() = 0;

	// ������� ������� ���������� ����������� ��������� � Update �������, ��� ���������� ��������
	virtual void		Update();
	// ���������� ������� ��������� ��������
	int					GetCurrentPhase() const			{ return m_iPhase; }
	// ���������� ��������� ��������
	// Params:
	// 1. period - ������ � ������������
	// 2. initialState - ��������� ��������� ��������
	void				SetAnimationPeriod(int period)	{ m_iAnimationPeriod = period; }
	void				SetCurrentPhase(int phase)		{ m_iPhase = phase; }

	// ��������� ��������
	enum EAnimationState
	{
		easPlayed,
		easStopped
	};
	// ������� ���������� ���������
	void				Play();
	void				Stop();
	void				Reset()							{ m_uPrevGlobalTime = 0; m_uTotalTime = 0; m_iCurrAnimationTime = 0; }
	EAnimationState		GetState() const				{ return m_eAnimationState; }

	void				SetPlayDuration(int duration)	{ m_iAnimationDuration = duration; }
	void				SetMessageTarget(CUIWindow *pWnd) { m_pMessageTarget = pWnd; }

protected:
	// ������ ��������
	int					m_iAnimationPeriod;
	// ���������� ����������� �������� ����������� �������
	int					m_uPrevGlobalTime;
	// ������� ����� ��������
	int					m_iCurrAnimationTime;
	// ������� ���� ��������
	int					m_iPhase;
	// ����� ����� ������������ �������� ��� ��������� ���������� �� �������
	// ���� -1 �� ������ ����������
	u32					m_uTotalTime;
	int					m_iAnimationDuration;
	// ��������� ��������
	EAnimationState		m_eAnimationState;

	CUIWindow			*m_pMessageTarget;
};

#endif