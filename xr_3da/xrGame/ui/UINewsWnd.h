//=============================================================================
//  Filename:   UINewsWnd.h
//  News subwindow in PDA dialog
//=============================================================================

#ifndef UINEWSWND_H_
#define UINEWSWND_H_

// #pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIListWnd.h"
#include "UITabControl.h"

class CUINewsWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:
	// Ctor and Dtor
	CUINewsWnd();
	virtual ~CUINewsWnd();

	virtual void Init();
	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	virtual void Draw();
	virtual void Update();
private:
	// �������� �������� � ���� ����������

	// �������� ����
	CUIStatic		UIStaticCaption;
	// ����������-������������� �����������
	CUITabControl	UITabControl;
	// ���� ������ ����������
	CUIListWnd		UIListWnd;

	// ������ �����������
	
};

#endif