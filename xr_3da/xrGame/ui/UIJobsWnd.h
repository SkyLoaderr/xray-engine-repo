//=============================================================================
//  Filename:   UIJobsWnd.h
//	---------------------------------------------------------------------------
//  Jobs dialog subwindow in Diary window
//=============================================================================

#ifndef UI_JOBS_WND_H_
#define UI_JOBS_WND_H_

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIListWnd.h"
#include "xrXMLParser.h"

#include "../GameTask.h"

class CUIJobsWnd: public CUIDialogWnd
{
	typedef CUIDialogWnd inherited;
public:

	CUIJobsWnd();
	virtual ~CUIJobsWnd();
	virtual void Init();
	virtual void Show();
	// �������� 1 ����
	void AddTask(CGameTask * const task);
//	virtual bool OnKeyboard(int dik, E_KEYBOARDACTION keyboard_action);
	// ��� ������ �������� ��������, ��� ��� ��� ����� ���� ��� ����������� ���, ��� ����, �
	// �������� ������ ������� ��� ����� ������������ ��������� ���������
	CUIListWnd		UIList;
	// ��������� jobs
	void ReloadJobs();

	// ���������� ������
	void SetFilter(ETaskState newFilter);
	// �������� ��������� �������
	virtual ref_str		DialogName();
protected:
	// ��������� ��������� ������� �������
	void AddHeader(ref_str strHeader);

	CGameFont			*pHeaderFnt;
	CGameFont			*pSubTasksFnt;

	// CurrentFilter
	ETaskState			filter;
};

#endif