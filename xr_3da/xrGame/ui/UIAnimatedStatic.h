//=============================================================================
//  Filename:   UIAnimatedStatic.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ������ ��� ����������� ������������� ������
//=============================================================================

#ifndef UI_ANIMATED_STATIC_H_
#define UI_ANIMATED_STATIC_H_

#pragma once

#include "UIStatic.h"

class CUIAnimatedStatic: public CUIStatic
{
	typedef CUIStatic inherited;
	// ����������� ������ ��������
	u32		m_uFrameCount;
	// ������� �����
	u32		m_uCurFrame;
	// ������� �������� � ��������� � ������
	u32		m_uAnimRows, m_uAnimCols;
	// ������� ����� �� �������
	u32		m_uFrameWidth, m_uFrameHeight;
	// ����� ������ ���� �������� � ms.
	u32		m_uAnimationDuration;
	// ����� ��������� � ������ ��������
	u32		m_uTimeElapsed;
	// ����-������� ������������� ��������� ��������� ���������� ��������
	bool	m_bParamsChanged;
	// ������� ������������ ��������
	bool	m_bPlaying;

	// ������������� ������� �����
	// Params:	frameNum	- ����� �����: [0..m_uFrameCount)
	void SetFrame(const u32 frameNum);
public:
	CUIAnimatedStatic();
	
	// ������������� ���������
	void SetFramesCount(u32 frameCnt)					{ m_uFrameCount = frameCnt; m_bParamsChanged = true; }
	void SetAnimCols(u32 animCols)						{ m_uAnimCols = animCols; m_bParamsChanged = true; }
	void SetAnimationDuration(u32 animDur)				{ m_uAnimationDuration = animDur; m_bParamsChanged = true; }
	void SetFrameDimentions(u32 frameW, u32 frameH)		{ m_uFrameHeight = frameH; m_uFrameWidth = frameW; m_bParamsChanged = true; }
	// ����������
	void Play()											{ m_bPlaying = true; }
	void Stop()											{ m_bPlaying = false; }
	void Rewind(u32 delta = 0)							{ m_uCurFrame = 0xffffffff; m_uTimeElapsed = delta; }
	// ����-������� ������������ ������������
	bool m_bCyclic;

	virtual void Update();
};

#endif	//UI_ANIMATED_STATIC_H_