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
#include "UITaskWnd.h"
#include "UINewsWnd.h"

class CInventoryOwner;

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
//	virtual void Update();
		
	virtual void Show();

protected:
	// ���������
	CUIFrameWindow UIMainPdaFrame;

	// ������� �������� ������
	CUIDialogWnd *m_pActiveDialog;

	// ���������� PDA
	CUIPdaCommunication	UIPdaCommunication;
	CUIMapWnd			UIMapWnd;
	CUITaskWnd			UITaskWnd;
	CUINewsWnd			UINewsWnd;

	//�������� ������������� ����������
	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;

//	//�������� PDA ��� �������� ������������ ������
//	CUIStatic			UIPDAHeader;

	// ������ PDA
	CUITabControl		UITabControl;
};
