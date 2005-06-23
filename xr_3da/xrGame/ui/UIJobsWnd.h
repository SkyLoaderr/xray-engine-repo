//=============================================================================
//  Filename:   UIJobsWnd.h
//	---------------------------------------------------------------------------
//  Jobs dialog subwindow in Diary window
//=============================================================================
#pragma once

#include "UIWindow.h"
#include "../GameTask.h"

class CUIListWnd;

class CUIJobsWnd: public CUIWindow
{
	typedef CUIWindow inherited;
public:

	CUIJobsWnd();
	virtual ~CUIJobsWnd();
	virtual void Init();
	virtual void Show(bool status);
	// �������� 1 ����
	void AddTask(CGameTask * const task);
	// ��� ������ �������� ��������, ��� ��� ��� ����� ���� ��� ����������� ���, ��� ����, �
	// �������� ������ ������� ��� ����� ������������ ��������� ���������
	CUIListWnd*		UIList;
	// ��������� jobs
	void ReloadJobs();

	// ���������� ������
	void SetFilter(ETaskState newFilter);
protected:
	// ��������� ��������� ������� �������
	void AddHeader(shared_str strHeader);

	CGameFont			*pHeaderFnt;
	CGameFont			*pSubTasksFnt;

	// CurrentFilter
	ETaskState			filter;
};
