// CUIPdaWnd.h:  ������ PDA
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIDialogWnd.h"
#include "UIStatic.h"
#include "UIFrameWindow.h"
#include "UITabControl.h"
#include "UIPdaCommunication.h"
#include "UIMapWnd.h"
#include "UIDiaryWnd.h"
#include "UIFrameLineWnd.h"
#include "UIEncyclopediaWnd.h"

class CInventoryOwner;

extern const char * const ALL_PDA_HEADER_PREFIX;
extern const char * const PDA_XML;

///////////////////////////////////////
// �������� � �������� ������ PDA
///////////////////////////////////////

class CUIPdaWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd inherited;
public:
	CUIPdaWnd();
	virtual ~CUIPdaWnd();

	virtual void Init();

	virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);

//	virtual void Draw();
	virtual void Update();
		
	virtual void Show();
	
	// ����������� ��� ������������� ���� PDA ���������:
	// ����� ����� ������������� �����
	typedef enum { PDA_MAP_SET_ACTIVE_POINT = 8010 } E_MESSAGE;
	// ������ ��������
	typedef enum { TAB_PDACOMM, TAB_TASK, TAB_MAP, TAB_NEWS } E_PDA_TABS;
	// ������������� ������� �������� ��������
	void ChangeActiveTab(E_PDA_TABS tabNewTab);
	// ������������� �� ����� � ��������������� �� �������� �����
	void FocusOnMap(const int x, const int y, const int z);
	void AddNewsItem(const char *sData);

protected:
	// ���������
	CUIStatic UIMainPdaFrame;

	// ������� �������� ������
	CUIDialogWnd *m_pActiveDialog;

	// ���������� PDA
	CUIPdaCommunication	UIPdaCommunication;
	CUIMapWnd			UIMapWnd;
	CUIEncyclopediaWnd	UIEncyclopediaWnd;
	CUIDiaryWnd			UIDiaryWnd;

	//�������� ������������� ����������
	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;
	CUIFrameLineWnd		UIMainButtonsBackground;
	CUIFrameLineWnd		UITimerBackground;

//	//�������� PDA ��� �������� ������������ ������
//	CUIStatic			UIPDAHeader;

	// ������ PDA
	CUITabControl		UITabControl;

	// ���������� ������� �����
	void UpdateDateTime();
};
