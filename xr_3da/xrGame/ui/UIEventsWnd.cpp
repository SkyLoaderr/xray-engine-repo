#include "stdafx.h"
#include "UIEventsWnd.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIXmlInit.h"
#include "UIAnimatedStatic.h"
#include "UIMapWnd.h"
#include "UIListWnd.h"
#include "UITabControl.h"
#include "UITaskDescrWnd.h"
#include "../MainUI.h"
#include "../HUDManager.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "../level.h"
#include "../actor.h"
#include "../gametaskmanager.h"
#include "../gametask.h"
#include "../map_manager.h"
#include "../map_location.h"
#include "../string_table.h"
#include "UITaskItem.h"
#include "../alife_registry_wrappers.h"

CUIEventsWnd::CUIEventsWnd			()
{
	m_flags.zero			();
}

CUIEventsWnd::~CUIEventsWnd			()
{
	delete_data			(m_UIMapWnd);
	delete_data			(m_UITaskInfoWnd);
}

void CUIEventsWnd::Init				()
{
	CUIXml uiXml;
	bool xml_result					= uiXml.Init(CONFIG_PATH, UI_PATH, "pda_events.xml");
	R_ASSERT3						(xml_result, "xml file not found", "pda_events.xml");

	// load map background
	CUIXmlInit xml_init;
	xml_init.InitWindow				(uiXml, "main_wnd", 0, this);


	m_UILeftFrame					= xr_new<CUIFrameWindow>(); m_UILeftFrame->SetAutoDelete(true);
	AttachChild						(m_UILeftFrame);
	xml_init.InitFrameWindow		(uiXml, "main_wnd:left_frame", 0, m_UILeftFrame);


	m_UILeftHeader					= xr_new<CUIFrameLineWnd>(); m_UILeftHeader->SetAutoDelete(true);
	m_UILeftFrame->AttachChild		(m_UILeftHeader);
	xml_init.InitFrameLine			(uiXml, "main_wnd:left_frame:left_frame_header", 0, m_UILeftHeader);

	m_UIAnimation					= xr_new<CUIAnimatedStatic>(); m_UIAnimation->SetAutoDelete(true);
	xml_init.InitAnimatedStatic		(uiXml, "main_wnd:left_frame:left_frame_header:anim_static", 0, m_UIAnimation);
	m_UILeftHeader->AttachChild		(m_UIAnimation);

	m_UIRightWnd					= xr_new<CUIWindow>(); m_UIRightWnd->SetAutoDelete(true);
	AttachChild						(m_UIRightWnd);
	xml_init.InitWindow				(uiXml, "main_wnd:right_frame", 0, m_UIRightWnd);

	m_UIMapWnd						= xr_new<CUIMapWnd>(); m_UIMapWnd->SetAutoDelete(false);
//	m_UIRightWnd->AttachChild		(m_UIMapWnd);
	m_UIMapWnd->Init				("pda_events.xml","main_wnd:right_frame:map_wnd");

	m_UITaskInfoWnd					= xr_new<CUITaskDescrWnd>(); m_UITaskInfoWnd->SetAutoDelete(false);
	m_UITaskInfoWnd->Init			(&uiXml,"main_wnd:right_frame:task_descr_view");
	

	m_ListWnd						= xr_new<CUIListWnd>(); m_ListWnd->SetAutoDelete(true);
	m_UILeftFrame->AttachChild		(m_ListWnd);
	xml_init.InitListWnd			(uiXml, "main_wnd:left_frame:list", 0, m_ListWnd);
	m_ListWnd->ActivateList			(true);
	m_ListWnd->EnableScrollBar		(true);
	m_ListWnd->EnableActiveBackground(false);
	m_ListWnd->SetWindowName		("list_wnd");
	Register						(m_ListWnd);

	m_TaskFilter					= xr_new<CUITabControl>(); m_TaskFilter->SetAutoDelete(true);
	m_UILeftHeader->AttachChild		(m_TaskFilter);
	xml_init.InitTabControl			(uiXml, "main_wnd:left_frame:left_frame_header:filter_tab", 0, m_TaskFilter);
	m_TaskFilter->SetWindowName		("filter_tab");
	Register						(m_TaskFilter);
   AddCallback						("filter_tab",TAB_CHANGED,boost::bind(&CUIEventsWnd::OnFilterChanged,this,_1,_2));

   AddCallback						("list_wnd",LIST_ITEM_CLICKED,boost::bind(&CUIEventsWnd::OnListItemClicked,this,_1,_2));

   m_currFilter						= eActiveTask;
   SetDescriptionMode				(true);
}

void CUIEventsWnd::Update			()
{
	if(m_flags.test(flNeedReload) ){
		ReloadList(false);
		m_flags.set(flNeedReload,FALSE );
	}
	inherited::Update		();
}

void CUIEventsWnd::Draw				()
{
	inherited::Draw			();
}

void	CUIEventsWnd::SendMessage			(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

void CUIEventsWnd::OnFilterChanged			(CUIWindow* w, void*)
{
	m_currFilter			=(ETaskFilters)m_TaskFilter->GetActiveIndex();
	ReloadList				(false);
}

void CUIEventsWnd::Reload					()
{
		m_flags.set(flNeedReload,TRUE );
}

void CUIEventsWnd::ReloadList				(bool bClearOnly)
{
	m_ListWnd->RemoveAll	();
	if(bClearOnly)		return;

	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor) return;

	GameTasks& tasks = pActor->GameTaskManager().GameTasks();
	GameTasks::iterator it =  tasks.begin();
	CGameTask* task = NULL;
	for(;it!=tasks.end();++it){
		task = (*it).game_task;
		R_ASSERT(task);
		R_ASSERT(task->m_Objectives.size() > 0);

		if( !Filter(task) ) continue;

		CStringTable		stbl;
		CUITaskItem			*pTaskItem = NULL;

		for (u32 i = 0; i < task->m_Objectives.size(); ++i)
		{
			if(i==0)
				pTaskItem = xr_new<CUITaskRootItem>(this);
			else
				pTaskItem = xr_new<CUITaskSubItem>(this);

			pTaskItem->SetGameTask			(task, i);
			m_ListWnd->AddItem<CUIListItem>	(pTaskItem);
		}

	}

}

void CUIEventsWnd::Show					(bool status)
{
	inherited::Show			(status);
	m_UIMapWnd->Show		(status);
	m_UITaskInfoWnd->Show	(status);

	ReloadList				(status == false);
}

bool CUIEventsWnd::Filter(CGameTask* t)
{
	ETaskState task_state = t->m_Objectives[0].task_state;
	
	return (m_currFilter==eActiveTask			&& task_state==eTaskStateInProgress )||
			(m_currFilter==eAccomplishedTask	&& task_state==eTaskStateCompleted )||
			(m_currFilter==eFailedTask			&& task_state==eTaskStateFail )||
			(m_currFilter==eOwnTask				&& task_state==eTaskUserDefined );
}

void CUIEventsWnd::OnListItemClicked		(CUIWindow* w, void* d)
{
	//w -listwnd
	CUIListItem*	pSelItem	= (CUIListItem*)d;
	CUITaskItem*	pTaskSelItem	= smart_cast<CUITaskItem*>(pSelItem);

	if(pTaskSelItem->ObjectiveIdx() != 0 && pTaskSelItem->Objective()->HasMapLocation()){
		CMapLocation* ml = Level().MapManager().GetMapLocation(	pTaskSelItem->Objective()->map_location,
																pTaskSelItem->Objective()->object_id);
		m_UIMapWnd->SetActiveMap(ml->LevelName(), ml->Position());
	}

	int sz = m_ListWnd->GetSize		();
	for(int i=0; i<sz;++i){
		CUIListItem* itm = m_ListWnd->GetItem(i);
		if(pSelItem==itm)	
			itm->MarkSelected	(true);
		else
			itm->MarkSelected	(false);
	}
}

void CUIEventsWnd::SetDescriptionMode		(bool bMap)
{
	if(bMap){
		m_UIRightWnd->DetachChild		(m_UITaskInfoWnd);
		m_UIRightWnd->AttachChild		(m_UIMapWnd);
	}else{
		m_UIRightWnd->DetachChild		(m_UIMapWnd);
		m_UIRightWnd->AttachChild		(m_UITaskInfoWnd);
	}
	m_flags.set(flMapMode, bMap);
}

bool CUIEventsWnd::GetDescriptionMode		()
{
	return !!m_flags.test(flMapMode);
}

void CUIEventsWnd::ShowDescription			(CGameTask* t, int idx)
{
	if(GetDescriptionMode()){//map
		if(idx != 0 && t->m_Objectives[idx].HasMapLocation()){
			CMapLocation* ml = Level().MapManager().GetMapLocation(	t->m_Objectives[idx].map_location,
																	t->m_Objectives[idx].object_id);
			m_UIMapWnd->SetActiveMap(ml->LevelName(), ml->Position());
		}
	}else{//articles
		m_UITaskInfoWnd->ClearAll	();
		m_UITaskInfoWnd->AddArticle	("zone_anomalies_meat");
		m_UITaskInfoWnd->AddArticle	("zone_anomalies_zharka");

	}
}
