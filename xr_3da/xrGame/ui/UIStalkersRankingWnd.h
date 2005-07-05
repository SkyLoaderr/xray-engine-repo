//=============================================================================
//  Filename:   UIStalkersRankingWnd.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ������� �������� ��� - ������� ��������� ���� ����
//=============================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////
#include "UIWindow.h"

class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
class CUIStatic;
class CUICharacterInfo;
class CUIListWnd;

class CUIStalkersRankingWnd: public CUIWindow
{
	typedef CUIWindow inherited;

public:
	virtual void		Init				();
	void				AddStalkerInfo		();
	void				RemoveStalkers		();
	virtual void		Show				(bool status);
	virtual void		SendMessage			(CUIWindow* pWnd, s16 msg, void* pData /* = NULL */);
	
protected:
	// ������ ������������� ����������
	CUIFrameWindow*		UIInfoFrame;
	CUIFrameWindow*		UICharIconFrame;
	CUIFrameLineWnd*	UIInfoHeader;
	CUIFrameLineWnd*	UICharIconHeader;
	CUIAnimatedStatic*	UIAnimatedIcon;
	CUIStatic*			UIArticleHeader;

	// ���������� � ���������
	CUIWindow*			UICharacterWindow;
	CUICharacterInfo*	UICharacterInfo;
	CUIFrameWindow*		UIMask;
	// ������ ���������
	CUIListWnd*			UIStalkersList;
};
