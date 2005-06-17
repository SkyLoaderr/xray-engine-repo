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
#include "UIWindow.h"
//#include "UIFrameWindow.h"
//#include "UIListWnd.h"
//#include "UICharacterInfo.h"
//#include "UIAnimatedStatic.h"
//#include "UIFrameLineWnd.h"

class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIAnimatedStatic;
class CUIStatic;
class CUICharacterInfo;
class CUIListWnd;
//////////////////////////////////////////////////////////////////////////
/*
SELECT xray_oles.result, count(xray_oles.result)
FROM xray_oles
group by xray_oles.result
order by count(xray_oles.result) desc
*/
class CUIActorInfoWnd: public CUIWindow
{
	typedef CUIWindow inherited;

public:
						CUIActorInfoWnd		();
	virtual void		Init				();
	virtual void		Show				(bool status);
	void				SetLineOfText		(int idx, LPCSTR text);

protected:
	// ������ ������������� ����������
	CUIFrameWindow*		UIInfoFrame;
	CUIFrameWindow*		UICharIconFrame;
	CUIFrameLineWnd*	UIInfoHeader;
	CUIFrameLineWnd*	UICharIconHeader;
	CUIAnimatedStatic*	UIAnimatedIcon;
	CUIStatic*			UIArticleHeader;

	//���������� � ���������
	CUIWindow*			UICharacterWindow;
	CUICharacterInfo*	UICharacterInfo;
	CUIFrameWindow*		UIMask;
	//��� ���������
	CUIListWnd*			UIInfoList;
};

//////////////////////////////////////////////////////////////////////////

#endif