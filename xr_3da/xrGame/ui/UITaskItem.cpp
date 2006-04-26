#include "stdafx.h"
#include "UITaskItem.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "../gametask.h"
#include "../string_table.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "UIEventsWnd.h"
#include "UIEditBoxEx.h"
#include "UIEditBox.h"
#include "UIInventoryUtilities.h"
#include "../map_location.h"
#include "../map_manager.h"
#include "../level.h"

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
	m_captionTime		= xr_new<CUI3tButton>();	m_captionTime->SetAutoDelete(true);			AttachChild(m_captionTime);

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
	xml_init.InitStatic			(uiXml,"task_root_item:caption_time",0,m_captionTime);
	
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

	m_captionStatic->SetText		(*stbl.translate(m_GameTask->m_Title));
	m_captionStatic->AdjustHeightToText	();
	
	xr_string	txt ="";
	txt			+= *(InventoryUtilities::GetDateAsString(gt->m_ReceiveTime, InventoryUtilities::edpDateToDay));
	txt			+= " ";
	txt			+= *(InventoryUtilities::GetTimeAsString(gt->m_ReceiveTime, InventoryUtilities::etpTimeToMinutes));

	m_captionTime->SetText		(txt.c_str());
	m_captionTime->SetWndPos(m_captionTime->GetWndPos().x,m_captionStatic->GetWndPos().y+m_captionStatic->GetHeight()+3.0f);

	float h = _max	(m_taskImage->GetWndPos().y+m_taskImage->GetHeight(),m_captionTime->GetWndPos().y+m_captionTime->GetHeight());
	h	= _max(h,m_switchDescriptionBtn->GetWndPos().y+m_switchDescriptionBtn->GetHeight());
	h	= _max(h,m_showLocationBtn->GetWndPos().y+m_showLocationBtn->GetHeight());
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

	bool bHasLocation				= m_GameTask->HasMapLocations();
//.	m_showLocationBtn->Show			(bHasLocation);
	m_showLocationBtn->Show			(m_EventsWnd->ItemHasDescription(this));

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


	m_active_color					= xml_init.GetColor(uiXml, "task_sub_item:description:text_colors:active", 0, 0x00);
	m_failed_color					= xml_init.GetColor(uiXml, "task_sub_item:description:text_colors:failed", 0 , 0x00);
	m_accomplished_color			= xml_init.GetColor(uiXml, "task_sub_item:description:text_colors:accomplished", 0, 0x00);
}

void CUITaskSubItem::SetGameTask	(CGameTask* gt, int obj_idx)				
{
	inherited::SetGameTask			(gt, obj_idx);

	CStringTable		stbl;
	SGameTaskObjective	*obj = &m_GameTask->m_Objectives[m_TaskObjectiveIdx];

	m_descriptionStatic->SetText				(*stbl.translate(obj->description));
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

	m_showDescriptionBtn->Show				(m_EventsWnd->ItemHasDescription(this));

//.	bool bIsMapMode							= m_EventsWnd->GetDescriptionMode(); 
//.	m_showDescriptionBtn->Show				(!bIsMapMode||(bIsMapMode&&bHasLocation&&ml->SpotEnabled()) );
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
	m_edtWnd = NULL;
	Init();
}

CUIUserTaskItem::~CUIUserTaskItem			()
{
	delete_data	(m_edtWnd);
}

void  CUIUserTaskItem::Init					()
{
	inherited::Init					();
	CUIXml uiXml;
	bool xml_result					= uiXml.Init(CONFIG_PATH, UI_PATH, "job_item.xml");
	R_ASSERT3						(xml_result, "xml file not found", "job_item.xml");

	m_image							= xr_new<CUIStatic>();		m_image->SetAutoDelete(true);				AttachChild(m_image);

	m_descriptionStatic				= xr_new<CUIStatic>();		m_descriptionStatic->SetAutoDelete(true);	AttachChild(m_descriptionStatic);

	m_captionStatic					= xr_new<CUIStatic>();		m_captionStatic->SetAutoDelete(true);	AttachChild(m_captionStatic);
	

	m_showLocationBtn				= xr_new<CUI3tButton>();	m_showLocationBtn->SetAutoDelete(true);		AttachChild(m_showLocationBtn);
	m_showLocationBtn->				SetWindowName("m_showLocationBtn");
	Register						(m_showLocationBtn);
	AddCallback						(m_showLocationBtn->WindowName(),BUTTON_CLICKED,boost::bind(&CUIUserTaskItem::OnShowLocationClicked,this));

	m_showPointerBtn				= xr_new<CUI3tButton>();	m_showPointerBtn->SetAutoDelete(true);		AttachChild(m_showPointerBtn);
	m_showPointerBtn->				SetWindowName("m_showPointerBtn");
	Register						(m_showPointerBtn);
	AddCallback						(m_showPointerBtn->WindowName(),BUTTON_CLICKED,boost::bind(&CUIUserTaskItem::OnShowPointerClicked,this));


	m_editTextBtn					= xr_new<CUI3tButton>();	m_editTextBtn->SetAutoDelete(true);		AttachChild(m_editTextBtn);
	m_editTextBtn->					SetWindowName("m_editTextBtn");
	Register						(m_editTextBtn);
	AddCallback						(m_editTextBtn->WindowName(), BUTTON_CLICKED,boost::bind(&CUIUserTaskItem::OnEditTextClicked,this));

	m_removeBtn						= xr_new<CUI3tButton>();	m_removeBtn->SetAutoDelete(true);		AttachChild(m_removeBtn);
	m_removeBtn->					SetWindowName("m_removeBtn");
	Register						(m_removeBtn);
	AddCallback						(m_removeBtn->WindowName(), BUTTON_CLICKED,boost::bind(&CUIUserTaskItem::OnRemoveClicked,this));


	CUIXmlInit xml_init;
	xml_init.InitWindow				(uiXml,"task_user_item",0,this);

	xml_init.InitStatic				(uiXml,"task_user_item:image",0,m_image);
	xml_init.InitStatic				(uiXml,"task_user_item:description",0,m_descriptionStatic);
	xml_init.InitStatic				(uiXml,"task_user_item:caption",0,m_captionStatic);
	xml_init.Init3tButton			(uiXml,"task_user_item:location_btn",0,m_showLocationBtn);
	xml_init.Init3tButton			(uiXml,"task_user_item:show_pointer_btn",0,m_showPointerBtn);
	xml_init.Init3tButton			(uiXml,"task_user_item:edit_text_btn",0,m_editTextBtn);
	xml_init.Init3tButton			(uiXml,"task_user_item:remove_btn",0,m_removeBtn);

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

	m_captionStatic->SetText					(*stbl.translate(gt->m_Title));
	m_captionStatic->AdjustHeightToText			();

	float h1 = _max( m_image->GetWndPos().y+m_image->GetHeight(),
					m_captionStatic->GetWndPos().y + m_captionStatic->GetHeight() );
	m_descriptionStatic->SetWndPos				(m_descriptionStatic->GetWndPos().x, h1+4.0f);

	m_descriptionStatic->SetText				(*stbl.translate(obj->description));
	m_descriptionStatic->AdjustHeightToText		();

	float h = _max(	m_image->GetWndPos().y+m_image->GetHeight(),
					m_descriptionStatic->GetWndPos().y+ m_descriptionStatic->GetHeight());

	SetHeight									(h+10.0f);
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

void CUIUserTaskItem::OnEditTextClicked		()
{
	delete_data			(m_edtWnd);
	m_edtWnd			= xr_new<CUIUserTaskEditWnd>();
	m_edtWnd->Start		(this);
}

void CUIUserTaskItem::OnRemoveClicked		()
{
	Level().MapManager().RemoveMapLocation(Objective()->HasMapLocation());
}


CUIUserTaskEditWnd::CUIUserTaskEditWnd		()
{
	m_userTask = NULL;
	Init();
}

void CUIUserTaskEditWnd::SendMessage		(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

void CUIUserTaskEditWnd::Start				(CUIUserTaskItem* itm)
{
	VERIFY(!m_userTask);

	CStringTable stbl;

	m_userTask = itm;
	m_editCaption->SetText			(*stbl.translate(m_userTask->GameTask()->m_Title));
	m_editDescription->SetText		(*stbl.translate(m_userTask->Objective()->description));
	HUD().GetUI()->StartStopMenu	(this,true);
}

void CUIUserTaskEditWnd::OnOk			()
{
	m_userTask->GameTask()->m_Title			= m_editCaption->GetText();
	m_userTask->Objective()->description	= m_editDescription->GetText();
	m_userTask->Objective()->HasMapLocation	()->SetHint(m_editDescription->GetText());
	m_userTask->m_EventsWnd->Reload			();

	GetHolder()->StartStopMenu				(this, false);
	m_userTask = NULL;
}

void CUIUserTaskEditWnd::OnCancel				()
{
	GetHolder()->StartStopMenu(this, false);
	m_userTask = NULL;
}

void CUIUserTaskEditWnd::Init					()
{
	Hide					();
	CUIXml uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, "job_item.xml");
	R_ASSERT3(xml_result, "xml file not found", "job_item.xml");

	m_background		= xr_new<CUIFrameWindow>();		m_background->SetAutoDelete(true);
	AttachChild			(m_background);
	
	m_btnOk				= xr_new<CUI3tButton>();	m_btnOk->SetAutoDelete(true);		m_background->AttachChild(m_btnOk);
	m_btnOk->SetWindowName("m_btnOk");
	Register			(m_btnOk);
	AddCallback			(m_btnOk->WindowName(),BUTTON_CLICKED,boost::bind(&CUIUserTaskEditWnd::OnOk,this));

	m_btnCancel			= xr_new<CUI3tButton>();	m_btnCancel->SetAutoDelete(true);	m_background->AttachChild(m_btnCancel);
	m_btnCancel->SetWindowName("m_btnCancel");
	Register			(m_btnCancel);
	AddCallback			(m_btnCancel->WindowName(),BUTTON_CLICKED,boost::bind(&CUIUserTaskEditWnd::OnCancel,this));

	m_editCaption		= xr_new<CUIEditBox>();			m_editCaption->SetAutoDelete(true);		m_background->AttachChild(m_editCaption);
	m_editDescription	= xr_new<CUIEditBoxEx>();		m_editDescription->SetAutoDelete(true); m_background->AttachChild(m_editDescription);


	CUIXmlInit xml_init;
	xml_init.InitWindow				(uiXml,"edit_user_item",0,								this);
	xml_init.InitFrameWindow		(uiXml,"edit_user_item:background",0,					m_background);
	xml_init.Init3tButton			(uiXml,"edit_user_item:background:ok_btn",0,			m_btnOk);
	xml_init.Init3tButton			(uiXml,"edit_user_item:background:cancel_btn",0,		m_btnCancel);
	xml_init.InitEditBox			(uiXml,"edit_user_item:background:edit_caption",0,		m_editCaption);
	xml_init.InitEditBoxEx			(uiXml,"edit_user_item:background:edit_description",0,	m_editDescription);

}
