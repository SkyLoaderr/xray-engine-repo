// UIPdaWnd.cpp:  ������ PDA
// 
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "UIPdaWnd.h"
#include "../Pda.h"

#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UIJobItem.h"
#include "UIInventoryUtilities.h"

#include "../HUDManager.h"
#include "../level.h"
#include "../game_cl_base.h"

#include "UIStatic.h"
#include "UIFrameWindow.h"
#include "UITabControl.h"
#include "UIPdaCommunication.h"
#include "UIMapWnd.h"
#include "UIDiaryWnd.h"
#include "UIFrameLineWnd.h"
#include "UIEncyclopediaWnd.h"
#include "UIStalkersRankingWnd.h"
#include "UIActorInfo.h"

//////////////////////////////////////////////////////////////////////////

const char * const PDA_XML					= "pda.xml";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIPdaWnd::CUIPdaWnd()
{
	Init();

	Hide();
	
	SetFont(HUD().Font().pFontMedium);
}

//////////////////////////////////////////////////////////////////////////

CUIPdaWnd::~CUIPdaWnd()
{
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH,PDA_XML);
	R_ASSERT3(xml_result, "xml file not found", PDA_XML);

	CUIXmlInit xml_init;

	inherited::Init(CUIXmlInit::ApplyAlignX(0, alCenter),
					CUIXmlInit::ApplyAlignY(0, alCenter),
					UI_BASE_WIDTH, UI_BASE_HEIGHT);

	UIMainPdaFrame = xr_new<CUIStatic>(); UIMainPdaFrame->SetAutoDelete(true);
	AttachChild(UIMainPdaFrame);
	xml_init.InitStatic(uiXml, "background_static", 0, UIMainPdaFrame);

	//�������� ��������������� ����������
	xml_init.InitAutoStatic(uiXml, "auto_static", this);

	// Main buttons background
	UIMainButtonsBackground = xr_new<CUIFrameLineWnd>(); UIMainButtonsBackground->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UIMainButtonsBackground);
	xml_init.InitFrameLine(uiXml, "mbbackground_frame_line", 0, UIMainButtonsBackground);

	// Timer background
	UITimerBackground = xr_new<CUIFrameLineWnd>(); UITimerBackground->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UITimerBackground);
	xml_init.InitFrameLine(uiXml, "timer_frame_line", 0, UITimerBackground);

	// O��� ��������a���
	UIPdaCommunication = xr_new<CUIPdaCommunication>(); UIPdaCommunication->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UIPdaCommunication);
	UIPdaCommunication->Init();

	// O��� �����
	UIMapWnd = xr_new<CUIMapWnd>(); UIMapWnd->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UIMapWnd);
	UIMapWnd->Init();

	// O��� ��������
	UIDiaryWnd = xr_new<CUIDiaryWnd>(); UIDiaryWnd->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UIDiaryWnd);
	UIDiaryWnd->Init();

	// ���� ������������
	UIEncyclopediaWnd = xr_new<CUIEncyclopediaWnd>(); UIEncyclopediaWnd->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UIEncyclopediaWnd);
	UIEncyclopediaWnd->Init();
	UIEncyclopediaWnd->Show(false);

	// ���� ���������� � ������
	UIActorInfo = xr_new<CUIActorInfoWnd>(); UIActorInfo->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UIActorInfo);
	UIActorInfo->Init();
	UIActorInfo->Show(false);

	// ���� �������� ���������
	UIStalkersRanking = xr_new<CUIStalkersRankingWnd>(); UIStalkersRanking->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UIStalkersRanking);
	UIStalkersRanking->Init();
	UIStalkersRanking->Show(false);

	m_pActiveDialog = UIDiaryWnd;

	// Tab control
	UITabControl = xr_new<CUITabControl>(); UITabControl->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UITabControl);
	xml_init.InitTabControl(uiXml, "tab", 0, UITabControl);
	UITabControl->SetMessageTarget(this);

	// Off button
	UIOffButton = xr_new<CUIButton>(); UIOffButton->SetAutoDelete(true);
	UIMainPdaFrame->AttachChild(UIOffButton);
	xml_init.InitButton(uiXml, "off_button", 0, UIOffButton);
	UIOffButton->SetPushOffsetX(0);
	UIOffButton->SetPushOffsetY(0);
	UIOffButton->TextureOff();
	UIOffButton->SetMessageTarget(this);

	// Draw();
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	if(pWnd == UITabControl)
	{
		if (TAB_CHANGED == msg)
		{
			if (m_pActiveDialog)
				m_pActiveDialog->Show(false);

			// Add custom dialogs here
			switch (UITabControl->GetActiveIndex()) 
			{
			case eptEvents:
				m_pActiveDialog = smart_cast<CUIWindow*>(UIDiaryWnd);
				InventoryUtilities::SendInfoToActor("ui_pda_events");
				break;
			case eptComm:
				m_pActiveDialog = smart_cast<CUIWindow*>(UIPdaCommunication);
				InventoryUtilities::SendInfoToActor("ui_pda_contacts");
				break;
			case eptMap:
				m_pActiveDialog = smart_cast<CUIWindow*>(UIMapWnd);
				break;
			case eptEncyclopedia:
				m_pActiveDialog = smart_cast<CUIWindow*>(UIEncyclopediaWnd);
				InventoryUtilities::SendInfoToActor("ui_pda_encyclopedia");
				break;
			case eptActorStatistic:
				m_pActiveDialog = smart_cast<CUIWindow*>(UIActorInfo);
				InventoryUtilities::SendInfoToActor("ui_pda_actor_info");
				break;
			case eptRanking:
				m_pActiveDialog = smart_cast<CUIWindow*>(UIStalkersRanking);
				InventoryUtilities::SendInfoToActor("ui_pda_ranking");
				break;
			default:
				NODEFAULT;
			}
			m_pActiveDialog->Reset();
			m_pActiveDialog->Show(true);
		}
	}
	// ������� ����� ������������� �����. ����� ���������� ��������� � pData
	else if (PDA_MAP_SET_ACTIVE_POINT == msg)
	{
		UIMapWnd->SetActivePoint(*reinterpret_cast<Fvector*>(pData));
	}
	else if (UIOffButton == pWnd)
	{
		if (STATIC_FOCUS_RECEIVED == msg)
			UIOffButton->TextureOn();
		else if (STATIC_FOCUS_LOST == msg)
			UIOffButton->TextureOff();
		else if (BUTTON_CLICKED == msg)
			Game().StartStopMenu(this,true);
	}
	else if (PDA_ENCYCLOPEDIA_HAS_ARTICLE == msg)
	{
		CUIJobItem *pItem = smart_cast<CUIJobItem*>(pWnd);
		if (pItem)
		{
			bool* b = (bool*)pData;
			*b = UIEncyclopediaWnd->HasArticle(pItem->GetAdditionalMaterialID());
			//SetActiveSubdialog(epsEncyclopedia, pItem->GetAdditionalMaterialID());
		}
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
		UITabControl->SetNewActiveTab	(eptEvents);
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
	InventoryUtilities::SendInfoToActor("ui_pda");

	inherited::Show();
	m_pActiveDialog->Show(true);
}

//////////////////////////////////////////////////////////////////////////

void CUIPdaWnd::Hide()
{
	inherited::Hide();
	m_pActiveDialog->Show(false);

	InventoryUtilities::SendInfoToActor("ui_pda_hide");
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
	xr_string strTime = *InventoryUtilities::GetGameTimeAsString(InventoryUtilities::etpTimeToMinutes);
				strTime += " ";
				strTime += *InventoryUtilities::GetGameDateAsString(InventoryUtilities::edpDateToDay);

	if (xr_strcmp(strTime.c_str(), prevStrTime))
	{
		UITimerBackground->UITitleText.SetText(strTime.c_str());
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

void CUIPdaWnd::SetActiveSubdialog(EPdaSections section, ARTICLE_ID addiotionalValue)
{
	switch (section)
	{
	case epsActiveJobs:
		UITabControl->SetNewActiveTab	(eptEvents);
		UIDiaryWnd->SetActiveSubdialog	(section);
		break;
	case epsMap:
		UITabControl->SetNewActiveTab	(eptMap);
		break;
	case epsContacts:
		UITabControl->SetNewActiveTab	(eptComm);
		break;
	case epsEncyclopedia:
//		UITabControl.SetNewActiveTab	(eptEncyclopedia);
		UIEncyclopediaWnd->OpenTree		(addiotionalValue);
		UITabControl->SetNewActiveTab	(eptEncyclopedia);
		UIEncyclopediaWnd->UIBack.Show	(true);
		break;
	case epsDiaryArticle:
		UITabControl->SetNewActiveTab	(eptEvents);
		UIDiaryWnd->SetActiveSubdialog	(section);
		UIDiaryWnd->OpenDiaryTree		(addiotionalValue);
		UIEncyclopediaWnd->UIBack.Show	(true);
		break;
	default:
		NODEFAULT;
	}
}

void CUIPdaWnd::OnNewArticleAdded	()
{
	if(UIDiaryWnd->IsShown() &&	UIDiaryWnd->UIActorDiaryWnd.IsShown())
		UIDiaryWnd->ReloadArticles();

	if(UIEncyclopediaWnd->IsShown())
		UIEncyclopediaWnd->ReloadArticles();
}
