// CUIPdaWnd.h:  диалог PDA
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
// Подложка и основные кнопки PDA
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
	// Бэкграунд
	CUIFrameWindow UIMainPdaFrame;

	// Текущий активный диалог
	CUIDialogWnd *m_pActiveDialog;

	// Поддиалоги PDA
	CUIPdaCommunication	UIPdaCommunication;
	CUIMapWnd			UIMapWnd;
	CUITaskWnd			UITaskWnd;
	CUINewsWnd			UINewsWnd;

	//элементы декоративного интерфейса
	CUIStatic			UIStaticTop;
	CUIStatic			UIStaticBottom;

//	//верхушка PDA для кнопочек переключения режима
//	CUIStatic			UIPDAHeader;

	// кнопки PDA
	CUITabControl		UITabControl;
};
