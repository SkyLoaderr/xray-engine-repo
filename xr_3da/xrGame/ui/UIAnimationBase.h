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

class CUIAnimationBase
{
public:
	CUIAnimationBase();
	virtual ~CUIAnimationBase() = 0;

	// ������� ������� ���������� ����������� ��������� � Update �������, ��� ���������� ��������
	virtual void	Update();
	// ���������� ������� ��������� ��������
	int				GetCurrentPhase() const { return m_iPhase; }
	// ���������� ��������� ��������
	// Params:
	// 1. period - ������ � ������������
	// 2. initialState - ��������� ��������� ��������
	void			SetAnimationPeriod(int period) { m_iAnimationPeriod = period; }
	void			SetCurrentPhase(int phase) { m_iPhase = phase; }

	// ��������� ��������
	enum EAnimationState
	{
		easPlayed,
		easStopped
	};
	// ������� ���������� ���������
	void		Play();
	void		Stop()		{ m_eAnimationState = easStopped; }	
	void		Reset()		{ m_uPrevGlobalTime = 0; }	

protected:
	// ������ ��������
	int				m_iAnimationPeriod;
	// ���������� ����������� �������� ����������� �������
	int				m_uPrevGlobalTime;
	// ������� ����� ��������
	int				m_iCurrAnimationTime;
	// ������� ���� ��������
	int				m_iPhase;
	// ��������� ��������
	EAnimationState m_eAnimationState;
};

#endif