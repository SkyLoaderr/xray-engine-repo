//=============================================================================
//  Filename:   UIContracts.h
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Contracts UI Window
//=============================================================================

#ifndef UI_CONTRACTS_WND_H_
#define UI_CONTRACTS_WND_H_

// #pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIListWnd.h"
#include "UICharacterInfo.h"

//////////////////////////////////////////////////////////////////////////

class CUIContractsWnd: public CUIWindow
{
	typedef CUIWindow inherited;
public:
	CUIContractsWnd() {}
	virtual ~CUIContractsWnd() {}
	virtual void Init();
	virtual void Show(bool status);
	// ���� ��� ���������� ������� �� ��������
	CUIListWnd			UIListWnd;
	// ���������� � ��������
	CUICharacterInfo	UICharInfo;

	// �������� ��������� �������
	virtual ref_str	WindowName();
protected:
	// �������� ����
	CUIStatic		UIStaticCaption;
};

#endif