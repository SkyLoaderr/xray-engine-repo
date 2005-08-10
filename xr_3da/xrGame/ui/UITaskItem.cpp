#include "stdafx.h"
#include "UITaskItem.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "../gametask.h"
#include "../string_table.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "UIEventsWnd.h"
#include "UIEditBox.h"
#include "../map_location.h"

CUITaskItem::CUITaskItem			(CUIEventsWnd* w)
:m_GameTask			(NULL),
m_TaskObjectiveIdx	(-1),
m_EventsWnd(w)
{
}

CUITaskItem::~CUITaskItem			()
{
}

void CUITaskItem::SetGameTask(CGameTask* gt, int obj_idx)				
{ 
	m_GameTask = gt;
	m_TaskObjectiveIdx = obj_idx;
}

void CUITaskItem::SendMessage				(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

SGameTaskObjective*	CUITaskItem::Objective	()	
{
	return &m_GameTask->m_Objectives[m_TaskObjectiveIdx];
}

void CUITaskItem::Update				()
{
	inherited::Update();
}

void CUITaskItem::Init				()
{
	SetWindowName					("job_item");
	AddCallback						("job_item",BUTTON_CLICKED,boost::bind(&CUITaskItem::OnClick,this));
}

void CUITaskItem::OnClick				()
{
	m_EventsWnd->ShowDescription						(GameTask(), ObjectiveIdx());
}


CUITaskRootItem::CUITaskRootItem	(CUIEventsWnd* w)
:inherited(w)
{
	Init();
}

CUITaskRootItem::~CUITaskRootItem		()
{}

void CUITaskRootItem::Init			()
{
	inherited::Init					();
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, "job_item.xml");
	R_ASSERT3(xml_result, "xml file not found", "job_item.xml");

	m_taskImage			= xr_new<CUIStatic>();		m_taskImage->SetAutoDelete(true);			AttachChild(m_taskImage);
	m_captionStatic		= xr_new<CUIStatic>();		m_captionStatic->SetAutoDelete(true);		AttachChild(m_captionStatic);
	m_showLocationBtn	= xr_new<CUI3tButton>();	m_showLocationBtn->SetAutoDelete(true);		AttachChild(m_showLocationBtn);
	m_switchDescriptionBtn= xr_new<CUI3tButton>();	m_switchDescriptionBtn->SetAutoDelete(true); AttachChild(m_switchDescriptionBtn);
	m_showLocationBtn->SetWindowName("m_showLocationBtn");
	Register(m_showLocationBtn);
	m_switchDescriptionBtn->SetWindowName("m_switchDescriptionBtn");
	Register(m_switchDescriptionBtn);
	AddCallback						("m_showLocationBtn",BUTTON_CLICKED,boost::bind(&CUITaskRootItem::OnShowLocationClicked,this));
	AddCallback						("m_switchDescriptionBtn",BUTTON_CLICKED,boost::bind(&CUITaskRootItem::OnSwitchDescriptionClicked,this));

	CUIXmlInit xml_init;
	xml_init.InitWindow			(uiXml,"task_root_item",0,this);

	xml_init.InitStatic			(uiXml,"task_root_item:image",0,m_taskImage);
	xml_init.InitStatic			(uiXml,"task_root_item:caption",0,m_captionStatic);
	
	xml_init.Init3tButton		(uiXml,"task_root_item:location_btn",0,m_showLocationBtn);
	xml_init.Init3tButton		(uiXml,"task_root_item:switch_description_btn",0,m_switchDescriptionBtn);

}

void CUITaskRootItem::SetGameTask(CGameTask* gt, int obj_idx)				
{
	inherited::SetGameTask(gt, obj_idx);

	CStringTable		stbl;
	SGameTaskObjective	*obj = &m_GameTask->m_Objectives[m_TaskObjectiveIdx];

	m_taskImage->InitTexture		(*obj->icon_texture_name);
	Frect r							= obj->icon_rect;
	m_taskImage->SetOriginalRect	(r.x1, r.y1, r.x2, r.y2);
	m_taskImage->ClipperOn			();
	m_taskImage->SetStretchTexture	(true);

	m_captionStatic->SetText		(*stbl(m_GameTask->m_Title));
	m_captionStatic->AdjustHeightToText	();

	float h = _max	(m_taskImage->GetWndPos().y+m_taskImage->GetHeight(),m_captionStatic->GetWndPos().y+m_captionStatic->GetHeight());
	SetHeight						(h);
	
	
	m_curr_descr_mode				= m_EventsWnd->GetDescriptionMode();
	if(m_curr_descr_mode)
		m_switchDescriptionBtn->InitTexture	("ui_icons_newPDA_showtext");
	else
		m_switchDescriptionBtn->InitTexture	("ui_icons_newPDA_showmap");

}

void CUITaskRootItem::Update		()
{
	inherited::Update				();

	bool bHasLocation					= m_GameTask->HasMapLocations();
	m_showLocationBtn->Show				(bHasLocation);
	if(bHasLocation){
		bool bShown						= m_GameTask->ShownLocations();
		m_showLocationBtn->SetButtonMode(bShown ? CUIButton::BUTTON_PUSHED : CUIButton::BUTTON_NORMAL);
	}

	if( m_curr_descr_mode	!= m_EventsWnd->GetDescriptionMode() ){
		m_curr_descr_mode				= m_EventsWnd->GetDescriptionMode();
		if(m_curr_descr_mode)
			m_switchDescriptionBtn->InitTexture	("ui_icons_newPDA_showtext");
		else
			m_switchDescriptionBtn->InitTexture	("ui_icons_newPDA_showmap");
	}

	m_switchDescriptionBtn->SetButtonMode(m_EventsWnd->GetDescriptionMode() ? CUIButton::BUTTON_NORMAL : CUIButton::BUTTON_PUSHED);
}

bool CUITaskRootItem::OnDbClick	()
{
	return true;
}

void CUITaskRootItem::OnShowLocationClicked	()
{
	bool bPushed = m_showLocationBtn->GetCheck	();
	m_GameTask->ShowLocations					(bPushed);
}

void CUITaskRootItem::OnSwitchDescriptionClicked	()
{
	bool bPushed = 	m_switchDescriptionBtn->GetCheck	();
	m_EventsWnd->SetDescriptionMode						(!bPushed);
	m_EventsWnd->ShowDescription						(GameTask(), ObjectiveIdx());
}

void CUITaskRootItem::MarkSelected (bool b)
{
}


CUITaskSubItem::CUITaskSubItem		(CUIEventsWnd* w)
:inherited(w)
{
	Init();
}

CUITaskSubItem::~CUITaskSubItem		()
{}

void CUITaskSubItem::Init			()
{
	inherited::Init					();
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, "job_item.xml");
	R_ASSERT3(xml_result, "xml file not found", "job_item.xml");

	m_stateStatic		= xr_new<CUIStatic>();		m_stateStatic->SetAutoDelete(true);		AttachChild(m_stateStatic);
	m_descriptionStatic	= xr_new<CUIStatic>();		m_descriptionStatic->SetAutoDelete(true);	AttachChild(m_descriptionStatic);
	m_showPointerBtn	= xr_new<CUI3tButton>();	m_showPointerBtn->SetAutoDelete(true);		AttachChild(m_showPointerBtn);
	m_showDescriptionBtn= xr_new<CUI3tButton>();	m_showDescriptionBtn->SetAutoDelete(true);		AttachChild(m_showDescriptionBtn);

	m_showPointerBtn->SetWindowName					("m_showPointerBtn");
	m_showDescriptionBtn->SetWindowName				("m_showDescriptionBtn");
	Register										(m_showPointerBtn);
	Register										(m_showDescriptionBtn);

	AddCallback						("m_showPointerBtn",BUTTON_CLICKED,boost::bind(&CUITaskSubItem::OnShowPointerClicked,this));
	AddCallback						("m_showDescriptionBtn",BUTTON_CLICKED,boost::bind(&CUITaskSubItem::OnShowDescriptionClicked,this));


	CUIXmlInit xml_init;
	xml_init.InitWindow				(uiXml,"task_sub_item",0,this);
	xml_init.InitStatic				(uiXml,"task_sub_item:state_image",0,m_stateStatic);
	xml_init.InitStatic				(uiXml,"task_sub_item:description",0,m_descriptionStatic);
	xml_init.Init3tButton			(uiXml,"task_sub_item:show_pointer_btn",0,m_showPointerBtn);
	xml_init.Init3tButton			(uiXml,"task_sub_item:show_descr_btn",0,m_showDescriptionBtn);


	m_active_color					= xml_init.GetARGB(uiXml,"task_sub_item:description:text_colors:active",0);
	m_failed_color					= xml_init.GetARGB(uiXml,"task_sub_item:description:text_colors:failed",0);
	m_accomplished_color			= xml_init.GetARGB(uiXml,"task_sub_item:description:text_colors:accomplished",0);
}

void CUITaskSubItem::SetGameTask	(CGameTask* gt, int obj_idx)				
{
	inherited::SetGameTask			(gt, obj_idx);

	CStringTable		stbl;
	SGameTaskObjective	*obj = &m_GameTask->m_Objectives[m_TaskObjectiveIdx];

	m_descriptionStatic->SetText				(*stbl(obj->description));
	m_descriptionStatic->AdjustHeightToText		();
	float h = _max(	m_showPointerBtn->GetWndPos().y+m_showPointerBtn->GetHeight(),
					m_descriptionStatic->GetWndPos().y+ m_descriptionStatic->GetHeight());
	SetHeight									(h);
	switch (obj->TaskState())
	{
		case eTaskUserDefined:
		case eTaskStateInProgress:
			m_stateStatic->InitTexture				("ui_icons_PDA_subtask_active");
			m_descriptionStatic->SetTextColor		(m_active_color);
			break;
		case eTaskStateFail:
			m_stateStatic->InitTexture				("ui_icons_PDA_subtask_failed");
			m_descriptionStatic->SetTextColor		(m_failed_color);
			break;
		case eTaskStateCompleted:
			m_stateStatic->InitTexture				("ui_icons_PDA_subtask_accomplished");
			m_descriptionStatic->SetTextColor		(m_accomplished_color);
			break;
		default:
			NODEFAULT;
	};
}

void CUITaskSubItem::Update					()
{
	inherited::Update						();
	SGameTaskObjective	*obj				= &m_GameTask->m_Objectives[m_TaskObjectiveIdx];
	CMapLocation* ml						= obj->HasMapLocation();
	bool bHasLocation						= (NULL != ml);
	m_showPointerBtn->Show					(bHasLocation&&ml->SpotEnabled());

	bool bIsMapMode							= m_EventsWnd->GetDescriptionMode(); 
	m_showDescriptionBtn->Show				(!bIsMapMode||(bIsMapMode&&bHasLocation&&ml->SpotEnabled()) );
	if(bHasLocation){
		bool bPointer						= m_GameTask->HighlightedSpotOnMap(m_TaskObjectiveIdx);
		m_showPointerBtn->SetButtonMode		(bPointer ? CUIButton::BUTTON_PUSHED : CUIButton::BUTTON_NORMAL);
	}
}

bool CUITaskSubItem::OnDbClick				()
{
	m_GameTask->HighlightSpotOnMap			(m_TaskObjectiveIdx,true);
	return true;
}

void CUITaskSubItem::OnShowPointerClicked	()
{
	bool bPushed							= m_showPointerBtn->GetCheck();
	m_GameTask->HighlightSpotOnMap			(m_TaskObjectiveIdx,bPushed);
	m_EventsWnd->ShowDescription			(GameTask(), ObjectiveIdx());
}

void CUITaskSubItem::OnShowDescriptionClicked ()
{
	m_EventsWnd->ShowDescription						(GameTask(), ObjectiveIdx());
}

void CUITaskSubItem::MarkSelected (bool b)
{
	m_showDescriptionBtn->SetButtonMode		(b ? CUIButton::BUTTON_PUSHED : CUIButton::BUTTON_NORMAL);
}

CUIUserTaskItem::CUIUserTaskItem(CUIEventsWnd* w)
:inherited(w)
{
	Init();
}

CUIUserTaskItem::~CUIUserTaskItem			()
{
}

void  CUIUserTaskItem::Init					()
{
	inherited::Init					();
	CUIXml uiXml;
	bool xml_result					= uiXml.Init(CONFIG_PATH, UI_PATH, "job_item.xml");
	R_ASSERT3						(xml_result, "xml file not found", "job_item.xml");

	m_image							= xr_new<CUIStatic>();		m_image->SetAutoDelete(true);				AttachChild(m_image);
	m_descriptionStatic				= xr_new<CUIEditBox>();		m_descriptionStatic->SetAutoDelete(true);	AttachChild(m_descriptionStatic);
	m_descriptionStatic->			SetWindowName("m_descriptionStatic");
	Register						(m_descriptionStatic);
	AddCallback						("m_descriptionStatic",EDIT_TEXT_CHANGED,boost::bind(&CUIUserTaskItem::OnDescriptionChanged,this));

	m_showLocationBtn				= xr_new<CUI3tButton>();	m_showLocationBtn->SetAutoDelete(true);		AttachChild(m_showLocationBtn);
	m_showLocationBtn->				SetWindowName("m_showLocationBtn");
	Register						(m_showLocationBtn);
	AddCallback						("m_showLocationBtn",BUTTON_CLICKED,boost::bind(&CUIUserTaskItem::OnShowLocationClicked,this));

	m_showPointerBtn				= xr_new<CUI3tButton>();	m_showPointerBtn->SetAutoDelete(true);		AttachChild(m_showPointerBtn);
	m_showPointerBtn->				SetWindowName("m_showPointerBtn");
	Register						(m_showPointerBtn);
	AddCallback						("m_showLocationBtn",BUTTON_CLICKED,boost::bind(&CUIUserTaskItem::OnShowLocationClicked,this));
	AddCallback						("m_showPointerBtn",BUTTON_CLICKED,boost::bind(&CUIUserTaskItem::OnShowPointerClicked,this));


	CUIXmlInit xml_init;
	xml_init.InitWindow				(uiXml,"task_user_item",0,this);

	xml_init.InitStatic				(uiXml,"task_user_item:image",0,m_image);
	xml_init.InitEditBox			(uiXml,"task_user_item:description",0,m_descriptionStatic);
	xml_init.Init3tButton			(uiXml,"task_user_item:location_btn",0,m_showLocationBtn);
	xml_init.Init3tButton			(uiXml,"task_user_item:show_pointer_btn",0,m_showPointerBtn);

}

void CUIUserTaskItem::Update					()
{
	inherited::Update		();
	SGameTaskObjective	*obj				= &m_GameTask->m_Objectives[m_TaskObjectiveIdx];
	CMapLocation* ml						= obj->HasMapLocation();
	bool bHasLocation						= (NULL != ml);
	m_showPointerBtn->Show					(bHasLocation&&ml->SpotEnabled());
	if(bHasLocation){
		bool bPointer						= m_GameTask->HighlightedSpotOnMap(m_TaskObjectiveIdx);
		m_showPointerBtn->SetButtonMode		(bPointer ? CUIButton::BUTTON_PUSHED : CUIButton::BUTTON_NORMAL);
		bool bShown							= m_GameTask->ShownLocations();
		m_showLocationBtn->SetButtonMode	(bShown ? CUIButton::BUTTON_PUSHED : CUIButton::BUTTON_NORMAL);
	}
}

void CUIUserTaskItem::SetGameTask				(CGameTask* gt, int obj_idx)
{
	inherited::SetGameTask			(gt, obj_idx);
	CStringTable		stbl;
	SGameTaskObjective	*obj = &m_GameTask->m_Objectives[m_TaskObjectiveIdx];

	m_image->InitTexture		(*obj->icon_texture_name);
	Frect r							= obj->icon_rect;
	m_image->SetOriginalRect	(r.x1, r.y1, r.x2, r.y2);
	m_image->ClipperOn			();
	m_image->SetStretchTexture	(true);

	m_descriptionStatic->SetText				(*stbl(obj->description));
	float h = _max(	m_image->GetWndPos().y+m_image->GetHeight(),
					m_descriptionStatic->GetWndPos().y+ m_descriptionStatic->GetHeight());
	SetHeight									(h);

}

void CUIUserTaskItem::OnShowPointerClicked	()
{
	bool bPushed = m_showPointerBtn->GetCheck();
	m_GameTask->HighlightSpotOnMap			(m_TaskObjectiveIdx,bPushed);
}

void CUIUserTaskItem::OnShowLocationClicked	()
{
	bool bPushed = m_showLocationBtn->GetCheck	();
	m_GameTask->ShowLocations					(bPushed);
}

void CUIUserTaskItem::MarkSelected				(bool b)
{
}

void CUIUserTaskItem::OnDescriptionChanged		()
{
	Objective()->description = m_descriptionStatic->GetText();
}
