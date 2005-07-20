#include "stdafx.h"
#include "UITaskItem.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "../gametask.h"
#include "../string_table.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>

CUITaskItem::CUITaskItem			()
:m_GameTask			(NULL),
m_TaskObjectiveIdx	(-1)
{
}

CUITaskItem::~CUITaskItem			()
{
}

void CUITaskItem::SetGameTask(CGameTask* gt, int obj_idx)				
{ 
	m_GameTask = gt;
	m_TaskObjectiveIdx = obj_idx;
//	if(m_GameTask->m_Objectives[m_TaskObjectiveIdx].article_id.size())
//		UIAdditionalMaterials.Show(true);
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

CUITaskRootItem::CUITaskRootItem		()
{
	Init();
}

CUITaskRootItem::~CUITaskRootItem		()
{}

void CUITaskRootItem::Init			()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, "job_item.xml");
	R_ASSERT3(xml_result, "xml file not found", "job_item.xml");

	m_taskImage			= xr_new<CUIStatic>();		m_taskImage->SetAutoDelete(true);			AttachChild(m_taskImage);
	m_captionStatic		= xr_new<CUIStatic>();		m_captionStatic->SetAutoDelete(true);		AttachChild(m_captionStatic);
	m_descriptionStatic	= xr_new<CUIStatic>();		m_descriptionStatic->SetAutoDelete(true);	AttachChild(m_descriptionStatic);
	m_showLocationBtn	= xr_new<CUI3tButton>();	m_showLocationBtn->SetAutoDelete(true);		AttachChild(m_showLocationBtn);			m_showLocationBtn->SetCheckMode(true);
	m_switchDescriptionBtn= xr_new<CUI3tButton>();	m_switchDescriptionBtn->SetAutoDelete(true); AttachChild(m_switchDescriptionBtn);	m_switchDescriptionBtn->SetCheckMode(true);
	m_showLocationBtn->SetWindowName("m_showLocationBtn");
	Register(m_showLocationBtn);
	m_switchDescriptionBtn->SetWindowName("m_switchDescriptionBtn");
	Register(m_switchDescriptionBtn);
	AddCallback						("m_showLocationBtn",BUTTON_CLICKED,boost::bind(&CUITaskRootItem::OnShowLocationClicked,this));
	AddCallback						("m_switchDescriptionBtn",BUTTON_CLICKED,boost::bind(&CUITaskRootItem::OnSwitchDescriptionClicked,this));

	CUIXmlInit xml_init;
	xml_init.InitStatic			(uiXml,"task_root_item:image",0,m_taskImage);
	xml_init.InitStatic			(uiXml,"task_root_item:caption",0,m_captionStatic);
	xml_init.InitStatic			(uiXml,"task_root_item:description",0,m_descriptionStatic);
	
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
	m_descriptionStatic->SetText	(*stbl(obj->description));
	
}

void CUITaskRootItem::Update		()
{
	inherited::Update				();

	bool bHasLocation						= m_GameTask->HasMapLocations();
	m_showLocationBtn->Enable				(bHasLocation);
	if(bHasLocation){
		bool bShown							= m_GameTask->ShownLocations();
		m_showLocationBtn->SetButtonMode		(bShown ? CUIButton::BUTTON_PUSHED : CUIButton::BUTTON_NORMAL);
	}

}

void CUITaskRootItem::OnShowLocationClicked	()
{
	bool bPushed = m_showLocationBtn->GetCheck	();
	m_GameTask->ShowLocations					(bPushed);
}

void CUITaskRootItem::OnSwitchDescriptionClicked	()
{
	bool bPushed = m_switchDescriptionBtn->GetCheck	();
}


CUITaskSubItem::CUITaskSubItem		()
{
	Init();
}

CUITaskSubItem::~CUITaskSubItem		()
{}

void CUITaskSubItem::Init			()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, "job_item.xml");
	R_ASSERT3(xml_result, "xml file not found", "job_item.xml");

	m_stateStatic		= xr_new<CUIStatic>();		m_stateStatic->SetAutoDelete(true);		AttachChild(m_stateStatic);
	m_descriptionStatic	= xr_new<CUIStatic>();		m_descriptionStatic->SetAutoDelete(true);	AttachChild(m_descriptionStatic);
	m_showPointerBtn	= xr_new<CUI3tButton>();	m_showPointerBtn->SetAutoDelete(true);		AttachChild(m_showPointerBtn); m_showPointerBtn->SetCheckMode(true);
	m_showPointerBtn->SetWindowName	("m_showPointerBtn");
	Register						(m_showPointerBtn);
	AddCallback						("m_showPointerBtn",BUTTON_CLICKED,boost::bind(&CUITaskSubItem::OnShowPointerClicked,this));


	CUIXmlInit xml_init;
	xml_init.InitStatic				(uiXml,"task_sub_item:state_image",0,m_stateStatic);
	xml_init.InitStatic				(uiXml,"task_sub_item:description",0,m_descriptionStatic);
	xml_init.Init3tButton			(uiXml,"task_sub_item:show_pointer_btn",0,m_showPointerBtn);
	m_defTextColor					= m_descriptionStatic->GetTextColor	();
	m_defColor						= m_descriptionStatic->GetColor	();
}

void CUITaskSubItem::SetGameTask	(CGameTask* gt, int obj_idx)				
{
	inherited::SetGameTask			(gt, obj_idx);

	CStringTable		stbl;
	SGameTaskObjective	*obj = &m_GameTask->m_Objectives[m_TaskObjectiveIdx];

	m_descriptionStatic->SetText	(*stbl(obj->description));

	switch (obj->task_state)
	{
		case eTaskUserDefined:
		case eTaskStateInProgress:
			m_stateStatic->InitTexture("ui_radio_e");
			break;
		case eTaskStateFail:
			m_stateStatic->InitTexture("ui_radio_d");
			break;
		case eTaskStateCompleted:
			m_stateStatic->InitTexture("ui_radio_t");
			break;
		default:
			NODEFAULT;
	};
}

void CUITaskSubItem::Update					()
{
	inherited::Update						();
	SGameTaskObjective	*obj				= &m_GameTask->m_Objectives[m_TaskObjectiveIdx];
	bool bHasLocation						= obj->HasMapLocation();
	m_showPointerBtn->Enable				(bHasLocation);
	if(bHasLocation){
		bool bPointer						= m_GameTask->HighlightedSpotOnMap(m_TaskObjectiveIdx);
		m_showPointerBtn->SetButtonMode		(bPointer ? CUIButton::BUTTON_PUSHED : CUIButton::BUTTON_NORMAL);
	}
}

void CUITaskSubItem::OnShowPointerClicked	()
{
	bool bPushed = m_showPointerBtn->GetCheck();
	m_GameTask->HighlightSpotOnMap			(m_TaskObjectiveIdx,bPushed);
}

void CUITaskSubItem::MarkSelected (bool b)
{
	if(b)
		m_descriptionStatic->SetLightAnim	("ui_task_selected");
	else{
		m_descriptionStatic->SetLightAnim	(NULL);
		m_descriptionStatic->SetTextColor	(m_defTextColor);
		m_descriptionStatic->SetColor		(m_defColor);
	}
}
