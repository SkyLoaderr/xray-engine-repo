//=============================================================================
//  Filename:   UIActorInfo.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ���� ���������� � ������
//=============================================================================

#ifndef UI_ACTOR_INFO_H_
#define UI_ACTOR_INFO_H_

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "UIFrameWindow.h"
#include "UIListWnd.h"
#include "UICharacterInfo.h"
#include "UIAnimatedStatic.h"
#include "UIFrameLineWnd.h"

//////////////////////////////////////////////////////////////////////////

class CUIActorInfoWnd: public CUIWindow
{
	typedef CUIWindow inherited;

public:
	virtual void		Init				();
	virtual void		Show				(bool status);
	void				SetLineOfText		(int idx, LPCSTR text);

protected:
	// ������ ������������� ����������
	CUIFrameWindow		UIInfoFrame;
	CUIFrameWindow		UICharIconFrame;
	CUIFrameLineWnd		UIInfoHeader;
	CUIFrameLineWnd		UICharIconHeader;
	CUIAnimatedStatic	UIAnimatedIcon;
	CUIStatic			UIArticleHeader;

	//���������� � ���������
	CUIWindow			UICharacterWindow;
	CUICharacterInfo	UICharacterInfo;
	CUIFrameWindow		UIMask;
	//��� ���������
	CUIListWnd			UIInfoList;
};

//////////////////////////////////////////////////////////////////////////

#endif