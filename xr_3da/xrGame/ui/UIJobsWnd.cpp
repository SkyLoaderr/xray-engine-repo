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
	R_ASSERT2(xml_result, "xml file not found");
	string256 buf;

	CUIXmlInit xml_init;

	AttachChild(&UIMainJobsFrame);
	xml_init.InitFrameWindow(uiXml, "msglog_frame_window", 0, &UIMainJobsFrame);

	UIMainJobsFrame.AttachChild(&UIMJFHeader);
	xml_init.InitFrameLine(uiXml, "cntd_frame_line", 0, &UIMJFHeader);
	strconcat(buf, ALL_PDA_HEADER_PREFIX, " / Task List");
	UIMJFHeader.UITitleText.SetText(buf);

	UIMainJobsFrame.AttachChild(&UIList);
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

	// Первый таск у нас являет собой заголовок задания
	CUIListItem *pHeader1 = xr_new<CUIListItem>();
	CUIListItem *pHeader2 = xr_new<CUIListItem>();
	CUIListItem *pHeader3 = xr_new<CUIListItem>();
	UIList.AddItem<CUIListItem>(pHeader1);
	UIList.AddItem<CUIListItem>(pHeader2);
	pHeader2->SetFont(pHeaderFnt);
	pHeader2->SetTextColor(clTaskHeaderColor);
	pHeader2->SetText(*task->ObjectiveDesc(0));
	UIList.AddItem<CUIListItem>(pHeader3);

	// Теперь пробегаемся по остальным таскам и заносим их как задания

	// Так как AddParsedItem добавляет несколько UIIconedListItem'ов, то мы запоминаем индекс первого
	// для того чтобы только ему присвоить иконку соответсвующую состоянию задания
	int iconedItemIdx = 0; 

	// Массив с именами текстурок - иконок состояний задания
	static ref_str iconsTexturesArr[3] =
	{
		"ui\\ui_pda_icon_mission_failed",
		"ui\\ui_pda_icon_mission_in_progress",
		"ui\\ui_pda_icon_mission_completed"
	};

	for (u32 i = 1; i < task->ObjectivesNum(); ++i)
	{
		iconedItemIdx = UIList.GetSize();
		UIList.AddParsedItem<CUIIconedListItem>(*task->ObjectiveDesc(i), 2, clTaskSubItemColor, HUD().pFontLetterica18Russian);

		R_ASSERT(iconedItemIdx < UIList.GetSize());
		CUIIconedListItem * pTask = dynamic_cast<CUIIconedListItem*>(UIList.GetItem(iconedItemIdx));

		R_ASSERT(pTask);
		if (pTask)
		{
			pTask->SetIcon(iconsTexturesArr[task->ObjectiveState(i)], uTaskIconSize);
		}
	}
}