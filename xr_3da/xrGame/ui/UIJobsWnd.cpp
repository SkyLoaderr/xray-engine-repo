//=============================================================================
//  Filename:   UIJobsWnd.cpp
//	---------------------------------------------------------------------------
//  Jobs dialog subwindow in Diary window
//=============================================================================

#include "StdAfx.h"
#include "UIJobsWnd.h"
#include "xrXMLParser.h"
#include "UIXmlInit.h"
#include "UIPdaWnd.h"
#include "UIIconedListItem.h"
#include "../GameTask.h"

#include "../Level.h"
#include "../HUDManager.h"

const char * const	JOBS_XML			= "jobs.xml";
const u32			uTaskIconSize		= 10;
const u32			clTaskSubItemColor	= 0xffffffff;
const u32			clTaskHeaderColor	= 0xffe1e1fa;

//////////////////////////////////////////////////////////////////////////

CUIJobsWnd::CUIJobsWnd()
	:	filter	(eTaskStateMax)
{
	pHeaderFnt		= HUD().pFontLetterica25;
	pSubTasksFnt	= HUD().pFontLetterica18Russian;
	pHeaderFnt->SetColor(clTaskHeaderColor);
}

//////////////////////////////////////////////////////////////////////////

CUIJobsWnd::~CUIJobsWnd()
{

}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::Init()
{
	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", JOBS_XML);
	R_ASSERT3(xml_result, "xml file not found", JOBS_XML);

	CUIXmlInit xml_init;

	AttachChild(&UIList);
	xml_init.InitListWnd(uiXml, "list", 0, &UIList);
	UIList.EnableScrollBar(true);
}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::AddTask(CGameTask * const task)
{
	// Code guards
	R_ASSERT(task);
	R_ASSERT(task->ObjectivesNum() > 0);
	if (!task || task->ObjectivesNum() < 0)	return;

	// ѕроверим на фильтре
	if (filter != eTaskStateMax && task->ObjectiveState(0) != filter) return;

	// “ак как AddParsedItem добавл€ет несколько UIIconedListItem'ов, то мы запоминаем индекс первого
	// дл€ того чтобы только ему присвоить иконку соответсвующую состо€нию задани€
	int			iconedItemIdx	= 0; 
	const int	subitemsOffset	= 20;

	// ћассив с именами текстурок - иконок состо€ний задани€
	static ref_str iconsTexturesArr[3] =
	{
		"ui\\ui_pda_icon_mission_failed",
		"ui\\ui_pda_icon_mission_in_progress",
		"ui\\ui_pda_icon_mission_completed"
	};

	// ѕервый таск у нас €вл€ет собой заголовок задани€
	CUIListItem			*pHeader1 = xr_new<CUIListItem>();
	CUIIconedListItem	*pHeader2 = xr_new<CUIIconedListItem>();
	CUIListItem			*pHeader3 = xr_new<CUIListItem>();
	UIList.AddItem<CUIListItem>(pHeader1);
	UIList.AddItem<CUIIconedListItem>(pHeader2);
	pHeader2->SetIcon(iconsTexturesArr[task->ObjectiveState(0)], uTaskIconSize);
	pHeader2->SetFont(pHeaderFnt);
	pHeader2->SetTextColor(clTaskHeaderColor);
	pHeader2->SetText(*task->ObjectiveDesc(0));
	pHeader2->SetTextX(15);
	UIList.AddItem<CUIListItem>(pHeader3);

	// “еперь пробегаемс€ по остальным таскам и заносим их как задани€

	for (u32 i = 1; i < task->ObjectivesNum(); ++i)
	{
		iconedItemIdx = UIList.GetSize();
		UIList.AddParsedItem<CUIIconedListItem>(*task->ObjectiveDesc(i), subitemsOffset, clTaskSubItemColor, HUD().pFontLetterica18Russian);

		R_ASSERT(iconedItemIdx < UIList.GetSize());
		CUIIconedListItem * pTask = dynamic_cast<CUIIconedListItem*>(UIList.GetItem(iconedItemIdx));

		R_ASSERT(pTask);
		if (pTask)
		{
			pTask->SetIcon(iconsTexturesArr[task->ObjectiveState(i)], uTaskIconSize);
			pTask->SetTextX(15);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::ReloadJobs()
{
	UIList.RemoveAll();

	CActor *pActor = dynamic_cast<CActor*>(Level().CurrentEntity());

	if (!pActor) return;

	for(KNOWN_INFO_VECTOR::const_iterator it = pActor->known_info_registry.objects().begin();
		pActor->known_info_registry.objects().end() != it; ++it)
	{
		//подгрузить кусочек информации с которым мы работаем
		CInfoPortion info_portion;
		info_portion.Load(it->id);

		// ƒобавл€ем таск если есть
		CGameTask *pTask = info_portion.GetTask();

		if (pTask)
		{
			pTask->CalcState(pActor);
			AddTask(pTask);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::Show()
{
	ReloadJobs();
	inherited::Show();
}

//////////////////////////////////////////////////////////////////////////

ref_str CUIJobsWnd::DialogName()
{
	switch (filter)
	{
	case eTaskStateCompleted:
		return "Completed Jobs";
		break;
	case eTaskStateInProgress:
		return "Active Jobs";
		break;
	case eTaskStateFail:
		return "Failed Jobs";
		break;
	default:
		R_ASSERT(!"Unknown type of task state");
	}

	return "";
}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::SetFilter(ETaskState newFilter)
{
	filter = newFilter;
}
