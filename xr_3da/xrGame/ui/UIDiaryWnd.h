// CUIDiaryWnd.h:  ������� � ��� ��� � ��� �������
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIListWnd.h"
#include "UITabControl.h"
#include "UIFrameWindow.h"

#include "UINewsWnd.h"
#include "UIJobsWnd.h"
#include "UINotesWnd.h"

///////////////////////////////////////
// �������
///////////////////////////////////////

class CUIDiaryWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:
	// Ctor and Dtor
	CUIDiaryWnd();
	virtual ~CUIDiaryWnd();

	virtual void Init();
	virtual void SendMessage(CUIWindow *pWnd, s16 msg, void* pData = NULL);
	// �������� ����� �������
	void AddNewsItem(const char *sData);
private:
	// �������� ���� �������� � ���� ����������
	CUINewsWnd		UINewsWnd;
	CUIJobsWnd		UIJobsWnd;
	CUINotesWnd		UINotesWnd;

	// ������� �������� ��������
	CUIDialogWnd	*m_pActiveSubdialog;
	// ����������-������������� �����������
	CUITabControl	UITabControl;
};