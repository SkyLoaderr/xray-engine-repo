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
#include "UIJobItem.h"
#include "../GameTask.h"
#include "UIStatsWnd.h"

#include "../Level.h"
#include "../HUDManager.h"
#include "../string_table.h"

//////////////////////////////////////////////////////////////////////////

const char * const	JOBS_XML						= "jobs.xml";
const u32			uTaskIconSize					= 10;
const u32			clTaskSubItemColor				= 0xffe1e1fa;
const u32			clTaskHeaderColor				= 0xffffffff;
const float			secondaryObjectiveScaleFactor	= 0.8f;
const int			subitemsOffset					= 30;

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
	UIList.ActivateList(true);
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

	CStringTable		stbl;
	SGameTaskObjective	*obj = NULL;
	Irect				r;
	CUIJobItem			*pJobItem = NULL;
	u32					color = 0xffffffff;

	// Устанавливаем цвет иконок
	switch (task->ObjectiveState(0))
	{
	case eTaskStateCompleted:
		color = 0xff00ff00;
		break;
	case eTaskStateFail:
		color = 0xffff0000;
		break;
	case eTaskStateInProgress:
		color = 0xffffffff;
		break;
	default:
		NODEFAULT;
	}

	// Пробегаемся по таскам и заносим их как задания
	int tmp = 0;

	CActor* pActor = smart_cast<CActor*>(Level().CurrentEntity());

	for (u32 i = 0; i < task->ObjectivesNum(); ++i)
	{
		if (i > 0) tmp = subitemsOffset;

		pJobItem = xr_new<CUIJobItem>(tmp);
		obj = &task->data()->m_Objectives[i];
		UIList.AddItem<CUIListItem>(pJobItem);
		r.set(obj->icon_x, obj->icon_y, obj->icon_width, obj->icon_height);
//		r.set(500, 800, 50, 50);
		if (0 != obj->icon_texture_name.size())
			pJobItem->SetPicture			(*obj->icon_texture_name, r, color);
//		pJobItem->SetPicture				("ui\\ui_icon_equipment", r);
		pJobItem->SetCaption				(*stbl(task->ObjectiveTitle()));
		pJobItem->SetDescription			(*stbl(task->ObjectiveDesc(i)));
		pJobItem->SetAdditionalMaterialID	(task->ObjectiveArticle(i));
		if (i > 0) pJobItem->ScalePicture	(secondaryObjectiveScaleFactor);

		if(pActor && pActor->encyclopedia_registry.objects_ptr())
		{
			for(ARTICLE_VECTOR::iterator it = pActor->encyclopedia_registry.objects().begin();
				it != pActor->encyclopedia_registry.objects().end(); it++)
			{
				if (task->ObjectiveArticle(i) == it->index)
				{
					if (ARTICLE_DATA::eEncyclopediaArticle == it->article_type)
						pJobItem->SetCallbackMessage(PDA_OPEN_ENCYCLOPEDIA_ARTICLE);
					else if (ARTICLE_DATA::eDiaryArticle == it->article_type)
						pJobItem->SetCallbackMessage(PDA_OPEN_DIARY_ARTICLE);
					break;
				}
			}
		}
	}
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