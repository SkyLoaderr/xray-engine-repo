//=============================================================================
//  Filename:   UINotesWnd.h
//	---------------------------------------------------------------------------
//  Notes subwindow in Diary window
//=============================================================================

#ifndef UI_NOTES_WND_H_
#define UI_NOTES_WND_H_

// #pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIListWnd.h"
#include "xrXMLParser.h"

class CUINotesWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:
	CUINotesWnd() {}
	virtual ~CUINotesWnd() {}
	virtual void Init();
	// ��� ������ �������� ��������, ��� ��� ��� ����� ���� ��� ����������� ���, ��� ����, �
	// �������� ������ ������� ��� ����� ������������ ��������� ���������
	CUIListWnd		UIListWnd;
protected:
	// �������� ����
	CUIStatic		UIStaticCaption;
};

#endif