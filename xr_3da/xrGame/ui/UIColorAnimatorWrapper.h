//=============================================================================
//  Filename:   UIColorAnimatorWrapper.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ��������� ��� LightAnimLibrary, � ������� ����������� ����� �������
//	������ � ���������, � ��������� ������ ������� ����������� ��� UI: ���
//	��� UI ���������� �� �� ����� �����, �� ������� ������� ������������
//	Device.fTimeDelta ������ - �������� ����������� ����������. ���������� 
//	��������� ������� ����� ����� ���������.
//=============================================================================

#ifndef UI_COLOR_ANIMATOR_WRAPPER_H_
#define UI_COLOR_ANIMATOR_WRAPPER_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

class CLAItem;

//////////////////////////////////////////////////////////////////////////

class CUIColorAnimatorWrapper
{
public:
	// �������� ����� �������� ����� �������� �������.
	// ����� ������ �� ������ ����������� Reset()
	// ��� ������������ ������ 1 ����� �������� Cyclic(false);

						CUIColorAnimatorWrapper	();
	// colorToModify - ��������� �� ���� ������� ������
	// animationName - ��� �������� ��������
	explicit			CUIColorAnimatorWrapper	(const ref_str &animationName, u32 *colorToModify);

	// ���������� ����� �������� ��������
	void				SetColorAnimation		(const ref_str &animationName);
	void				SetColorToModify		(u32 *colorToModify);
	// ������� ������� ���������� ����������� ��������� � Update �������, ��� ���������� ��������
	virtual void		Update					();
	void				Cyclic					(bool cyclic)	{ isCyclic = cyclic; }
	void				Reset					() const;
	u32					GetColor				() const		{ return currColor; }

private:
	// ���������� ��������
	CLAItem				*colorAnimation;
	// ���������� ���������� �������� ����������� ������� ����
	mutable float		prevGlobalTime;
	// ����� ��������� � ������ ��������
	mutable float		animationTime;
	// ����������� ����������� ������������
	bool				isCyclic;
	// ���� ������� �� ����� �������� ���������� ��� �����
	u32					*color;
	// ������� ���� ��������
	u32					currColor;
	// ������� ���� ��������
	int					currFrame;
};

#endif