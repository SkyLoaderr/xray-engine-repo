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
#include "UIStatsWnd.h"

#include "../Level.h"
#include "../HUDManager.h"
#include "../string_table.h"

//////////////////////////////////////////////////////////////////////////

const char * const	JOBS_XML			= "jobs.xml";
const u32			uTaskIconSize		= 10;
const u32			clTaskSubItemColor	= 0xffe1e1fa;
const u32			clTaskHeaderColor	= 0xffffffff;

//////////////////////////////////////////////////////////////////////////

CUIJobsWnd::CUIJobsWnd()
	:	filter	(eTaskStateDummy)
{
	pHeaderFnt		= HUD().pFontLetterica18Russian;
	pSubTasksFnt	= HUD().pFontLetterica16Russian;
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

	inherited::Init(0,0, UI_BASE_WIDTH, UI_BASE_HEIGHT);

	CUIXmlInit xml_init;

	AttachChild(&UIList);
	xml_init.InitListWnd(uiXml, "list", 0, &UIList);
	UIList.ActivateList(false);
	UIList.EnableScrollBar(true);
	UIList.EnableActiveBackground(false);
}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::AddTask(CGameTask * const task)
{
	// Code guards
	R_ASSERT(task);
	R_ASSERT(task->ObjectivesNum() > 0);
	if (!task || task->ObjectivesNum() < 0)	return;

	// Проверим на фильтре
	if (filter != eTaskStateDummy && task->ObjectiveState(0) != filter) return;

	// Так как AddParsedItem добавляет несколько UIIconedListItem'ов, то мы запоминаем индекс первого
	// для того чтобы только ему присвоить иконку соответсвующую состоянию задания
	int			iconedItemIdx	= 0; 
	const int	subitemsOffset	= 20;

	// Массив с именами текстурок - иконок состояний задания
	static shared_str iconsTexturesArr[3] =
	{
		"ui\\ui_pda_icon_mission_failed",
		"ui\\ui_pda_icon_mission_in_progress",
		"ui\\ui_pda_icon_mission_completed"
	};

	// Первый таск у нас являет собой заголовок задания
	CUIIconedListItem	*pHeader2 = xr_new<CUIIconedListItem>();
	UIList.AddItem<CUIIconedListItem>(pHeader2);
	pHeader2->SetIcon(iconsTexturesArr[task->ObjectiveState(0)], uTaskIconSize);
	pHeader2->SetFont(pHeaderFnt);
	pHeader2->SetTextColor(clTaskHeaderColor);
	pHeader2->SetNewRenderMethod(true);

	CStringTable stbl;
	pHeader2->SetText(*stbl(task->ObjectiveTitle()));
	pHeader2->SetTextX(15);

	// Теперь добавляем инфу о таске
	CUIStatsListItem *pNewItem = xr_new<CUIStatsListItem>();
	UIList.AddItem<CUIListItem>(pNewItem); 

	CUIXml uiXml;
	bool xml_result = uiXml.Init("$game_data$", JOBS_XML);
	R_ASSERT(xml_result);

	pNewItem->XmlInit("job_info", uiXml);

	pNewItem->FieldsVector[0]->SetText(*stbl("Received:"));
	string64 buf;
	::ZeroMemory(buf, 64);
	strconcat(buf, *InventoryUtilities::GetDateAsString(task->m_ReceiveTime, InventoryUtilities::edpDateToDay), " ",
				   *InventoryUtilities::GetTimeAsString(task->m_ReceiveTime, InventoryUtilities::etpTimeToMinutes));
	pNewItem->FieldsVector[1]->SetText(buf);
	pNewItem->FieldsVector[2]->SetText(*stbl("Finished:"));

	::ZeroMemory(buf, 64);
	if (task->ObjectiveState(0) != eTaskStateInProgress)
	{
		strconcat(buf, *InventoryUtilities::GetDateAsString(task->m_FinishTime, InventoryUtilities::edpDateToDay), " ",
					   *InventoryUtilities::GetTimeAsString(task->m_FinishTime, InventoryUtilities::etpTimeToMinutes));
	}
	else
		strcpy(buf, "--");

	pNewItem->FieldsVector[3]->SetText(buf);

	// Description
	CUIString str;
	str.SetText(*stbl(task->ObjectiveDesc(0)));
//	CUIStatic::PreprocessText(str.m_str, UIList.GetItemWidth() - 10, pSubTasksFnt);
	UIList.AddParsedItem<CUIListItem>(str, 0, clTaskSubItemColor, pSubTasksFnt);
    
	// Теперь пробегаемся по остальным таскам и заносим их как задания

	for (u32 i = 1; i < task->ObjectivesNum(); ++i)
	{
		iconedItemIdx = UIList.GetSize();
		UIList.AddParsedItem<CUIIconedListItem>(*stbl(task->ObjectiveDesc(i)), subitemsOffset, clTaskSubItemColor, pSubTasksFnt);

		R_ASSERT(iconedItemIdx < UIList.GetSize());
		CUIIconedListItem * pTask = smart_cast<CUIIconedListItem*>(UIList.GetItem(iconedItemIdx));

		R_ASSERT(pTask);
		if (pTask)
		{
			pTask->SetIcon(iconsTexturesArr[task->ObjectiveState(i)], uTaskIconSize);
			pTask->SetTextX(15);
			pTask->SetNewRenderMethod(true);
		}
	}

	// Добавляем пустую строчку-разделитель
	UIList.AddItem<CUIListItem>("\n");
}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::ReloadJobs()
{
	UIList.RemoveAll();

	CActor *pActor = smart_cast<CActor*>(Level().CurrentEntity());

	if (!pActor || !pActor->game_task_registry.objects_ptr()) return;

	for(GAME_TASK_VECTOR::const_iterator it = pActor->game_task_registry.objects().begin();
		pActor->game_task_registry.objects().end() != it; ++it)
	{
		CGameTask task;
		task.Init(*it);
		AddTask(&task);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::Show(bool status)
{
	if (status)
		ReloadJobs();
	inherited::Show(status);
}

//////////////////////////////////////////////////////////////////////////

void CUIJobsWnd::SetFilter(ETaskState newFilter)
{
	filter = newFilter;

	switch (filter)
	{
	case eTaskStateCompleted:
		SetWindowName("Completed Jobs");
		break;
	case eTaskStateInProgress:
		SetWindowName("Active Jobs");
		break;
	case eTaskStateFail:
		SetWindowName("Failed Jobs");
		break;
	default:
		R_ASSERT(!"Unknown type of task state");
	}
}