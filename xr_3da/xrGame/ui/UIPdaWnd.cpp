// UIPdaWnd.cpp:  диалог PDA
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIPdaWnd.h"
#include "../Pda.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"

#include "../HUDManager.h"

#define PDA_XML "pda.xml"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIPdaWnd::CUIPdaWnd()
{
	Init();

	Hide();
	
//	m_pInvOwner = NULL;
//	m_pPda = NULL;

	m_pActiveDialog = &UIPdaCommunication;
	SetFont(HUD().pFontMedium);
}

CUIPdaWnd::~CUIPdaWnd()
{
}

void CUIPdaWnd::Init()
{
	CUIXml uiXml;
	uiXml.Init("$game_data$",PDA_XML);
	CUIXmlInit xml_init;

	inherited::Init(0,0, Device.dwWidth, Device.dwHeight);

	AttachChild(&UIStaticTop);
	UIStaticTop.Init("ui\\ui_top_background", 0,0,1024,128);
	AttachChild(&UIStaticBottom);
	UIStaticBottom.Init("ui\\ui_bottom_background", 0,Device.dwHeight-32,1024,32);
	
	AttachChild(&UIMainPdaFrame);
	xml_init.InitFrameWindow(uiXml, "frame_window", 0, &UIMainPdaFrame);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);

	// Tab control
	AttachChild(&UITabControl);
	xml_init.InitTabControl(uiXml, "tab", 0, &UITabControl);

	// Oкно коммуникaции
	UIPdaCommunication.Init();
	UIMainPdaFrame.AttachChild(&UIPdaCommunication);

	// Oкно карты
	UIMainPdaFrame.AttachChild(&UIMapWnd);
	UIMapWnd.Init();

	// Окно задач
	UIMainPdaFrame.AttachChild(&UITaskWnd);
	UITaskWnd.Init();
	
	// Oкно новостей
	UIMainPdaFrame.AttachChild(&UIDiaryWnd);
	UIDiaryWnd.Init();
	UIDiaryWnd.SetMessageTarget(this);
}

void CUIPdaWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == &UITabControl)
	{
		if (CUITabControl::TAB_CHANGED == msg)
		{
			m_pActiveDialog->Hide();

			// Add custom dialogs here
			switch (UITabControl.GetActiveIndex()) 
			{
			case 0:
				m_pActiveDialog = dynamic_cast<CUIDialogWnd*>(&UIPdaCommunication);
				break;
			case 1:
				m_pActiveDialog = dynamic_cast<CUIDialogWnd*>(&UIMapWnd);
				break;
			case 2:
				m_pActiveDialog = dynamic_cast<CUIDialogWnd*>(&UIDiaryWnd);
				break;
			case 3:
				m_pActiveDialog = dynamic_cast<CUIDialogWnd*>(&UITaskWnd);
				break;
			}
			m_pActiveDialog->Reset();
			m_pActiveDialog->Show();
		}
	}
	// Сменить точку центрирования карты. Новые координаты поступают в pData
	else if (PDA_MAP_SET_ACTIVE_POINT == msg)
	{
		UIMapWnd.SetActivePoint(*reinterpret_cast<Fvector*>(pData));
	}
	else
	{
		R_ASSERT(m_pActiveDialog);
		m_pActiveDialog->SendMessage(pWnd, msg, pData);
	}
}

void CUIPdaWnd::Show()
{
	inherited::Show();
	m_pActiveDialog->Show();
}

void CUIPdaWnd::ChangeActiveTab(E_PDA_TABS tabNewTab)
{
	UITabControl.SetNewActiveTab(tabNewTab);
}

void CUIPdaWnd::FocusOnMap(const int x, const int y, const int z)
{
	Fvector a;
	a.set(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
	SendMessage(this, CUIPdaWnd::PDA_MAP_SET_ACTIVE_POINT, &a);
	ChangeActiveTab(CUIPdaWnd::TAB_MAP);
}

