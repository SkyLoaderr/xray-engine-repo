// UIPdaWnd.cpp:  диалог PDA
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIPdaWnd.h"
#include "../Pda.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UIJobItem.h"

#include "../HUDManager.h"
#include "../level.h"
#include "../game_cl_base.h"
//////////////////////////////////////////////////////////////////////////

const char * const PDA_XML					= "pda.xml";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIPdaWnd::CUIPdaWnd()
{
	Init();

	Hide();
	
	SetFont(HUD().pFontMedium);
}

//////////////////////////////////////////////////////////////////////////

CUIPdaWnd::~CUIPdaWnd()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$",PDA_XML);
	R_ASSERT3(xml_result, "xml file not found", PDA_XML);

	CUIXmlInit xml_init;

	inherited::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_HEIGHT);

	AttachChild(&UIMainPdaFrame);
	xml_init.InitStatic(uiXml, "background_static", 0, &UIMainPdaFrame);

	//Элементы автоматического добавления
	xml_init.InitAutoStatic(uiXml, "auto_static", this);

	// Main buttons background
	UIMainPdaFrame.AttachChild(&UIMainButtonsBackground);
	xml_init.InitFrameLine(uiXml, "mbbackground_frame_line", 0, &UIMainButtonsBackground);

	// Timer background
	UIMainPdaFrame.AttachChild(&UITimerBackground);
	xml_init.InitFrameLine(uiXml, "timer_frame_line", 0, &UITimerBackground);

	// Oкно коммуникaции
	UIPdaCommunication.Init();
	UIMainPdaFrame.AttachChild(&UIPdaCommunication);

	// Oкно карты
	UIMainPdaFrame.AttachChild(&UIMapWnd);
	UIMapWnd.Init();

	// Oкно новостей
	UIMainPdaFrame.AttachChild(&UIDiaryWnd);
	UIDiaryWnd.Init();

	// Окно энциклопедии
	UIMainPdaFrame.AttachChild(&UIEncyclopediaWnd);
	UIEncyclopediaWnd.Init();
	UIEncyclopediaWnd.Show(false);

	// Окно статистики о актере
	UIMainPdaFrame.AttachChild(&UIActorInfo);
	UIActorInfo.Init();
	UIActorInfo.Show(false);

	// Окно рейтинга сталкеров
	UIMainPdaFrame.AttachChild(&UIStalkersRanking);
	UIStalkersRanking.Init();
	UIStalkersRanking.Show(false);

	m_pActiveDialog = &UIDiaryWnd;

	// Tab control
	UIMainPdaFrame.AttachChild(&UITabControl);
	xml_init.InitTabControl(uiXml, "tab", 0, &UITabControl);
	UITabControl.SetMessageTarget(this);

	// Off button
	UIMainPdaFrame.AttachChild(&UIOffButton);
	xml_init.InitButton(uiXml, "off_button", 0, &UIOffButton);
	UIOffButton.SetPushOffsetX(0);
	UIOffButton.SetPushOffsetY(0);
	UIOffButton.TextureOff();
	UIOffButton.SetMessageTarget(this);

	Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == &UITabControl)
	{
		if (TAB_CHANGED == msg)
		{
			if (m_pActiveDialog)
				m_pActiveDialog->Show(false);

			// Add custom dialogs here
			switch (UITabControl.GetActiveIndex()) 
			{
			case eptEvents:
				m_pActiveDialog = smart_cast<CUIWindow*>(&UIDiaryWnd);
				break;
			case eptComm:
				m_pActiveDialog = smart_cast<CUIWindow*>(&UIPdaCommunication);
				break;
			case eptMap:
				m_pActiveDialog = smart_cast<CUIWindow*>(&UIMapWnd);
				break;
			case eptEncyclopedia:
				m_pActiveDialog = smart_cast<CUIWindow*>(&UIEncyclopediaWnd);
				break;
			case eptActorStatistic:
				m_pActiveDialog = smart_cast<CUIWindow*>(&UIActorInfo);
				break;
			case eptRanking:
				m_pActiveDialog = smart_cast<CUIWindow*>(&UIStalkersRanking);
				break;
			default:
				NODEFAULT;
			}
			m_pActiveDialog->Reset();
			m_pActiveDialog->Show(true);
		}
	}
	// Сменить точку центрирования карты. Новые координаты поступают в pData
	else if (PDA_MAP_SET_ACTIVE_POINT == msg)
	{
		UIMapWnd.SetActivePoint(*reinterpret_cast<Fvector*>(pData));
	}
	else if (&UIOffButton == pWnd)
	{
		if (STATIC_FOCUS_RECEIVED == msg)
			UIOffButton.TextureOn();
		else if (STATIC_FOCUS_LOST == msg)
			UIOffButton.TextureOff();
		else if (BUTTON_CLICKED == msg)
			Game().StartStopMenu(this,true);
			//HUD().GetUI()->UIGame()->StartStopMenu(this);
	}
	else if (PDA_OPEN_ENCYCLOPEDIA_ARTICLE == msg)
	{
		CUIJobItem *pItem = smart_cast<CUIJobItem*>(pWnd);
		if (pItem)
		{
			SetActiveSubdialog(epsEncyclopedia, pItem->GetAdditionalMaterialID());
		}
	}
	else if (PDA_OPEN_DIARY_ARTICLE == msg)
	{
		CUIJobItem *pItem = smart_cast<CUIJobItem*>(pWnd);
		if (pItem)
		{
			SetActiveSubdialog(epsDiaryArticle, pItem->GetAdditionalMaterialID());
		}
	}
	else if (PDA_GO_BACK == msg)
	{
		UITabControl.SetNewActiveTab	(eptEvents);
	}
	else
	{
		R_ASSERT(m_pActiveDialog);
		m_pActiveDialog->SendMessage(pWnd, msg, pData);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::Show()
{
	inherited::Show();
	m_pActiveDialog->Show(true);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::Hide()
{
	inherited::Hide();
	m_pActiveDialog->Show(false);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::FocusOnMap(const int x, const int y, const int z)
{
	Fvector a;
	a.set(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
	SendMessage(this, PDA_MAP_SET_ACTIVE_POINT, &a);
	SetActiveSubdialog(epsMap);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::UpdateDateTime()
{
	static shared_str prevStrTime = " ";
	std::string strTime = *InventoryUtilities::GetGameTimeAsString(InventoryUtilities::etpTimeToMinutes);
				strTime += " ";
				strTime += *InventoryUtilities::GetGameDateAsString(InventoryUtilities::edpDateToDay);

	if (xr_strcmp(strTime.c_str(), prevStrTime))
	{
		UITimerBackground.UITitleText.SetText(strTime.c_str());
		prevStrTime = strTime.c_str();
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::Update()
{
	inherited::Update();
	UpdateDateTime();
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::SetActiveSubdialog(EPdaSections section, int addiotionalValue)
{
	switch (section)
	{
	case epsActiveJobs:
		UITabControl.SetNewActiveTab	(eptEvents);
		UIDiaryWnd.SetActiveSubdialog	(section);
		break;
	case epsMap:
		UITabControl.SetNewActiveTab	(eptMap);
		break;
	case epsContacts:
		UITabControl.SetNewActiveTab	(eptComm);
		break;
	case epsEncyclopedia:
		UITabControl.SetNewActiveTab	(eptEncyclopedia);
		UIEncyclopediaWnd.OpenTree		(addiotionalValue);
		UIEncyclopediaWnd.UIBack.Show	(true);
		break;
	case epsDiaryArticle:
		UITabControl.SetNewActiveTab	(eptEvents);
		UIDiaryWnd.SetActiveSubdialog	(section);
		UIDiaryWnd.OpenDiaryTree		(addiotionalValue);
		UIEncyclopediaWnd.UIBack.Show	(true);
		break;
	default:
		NODEFAULT;
	}
}