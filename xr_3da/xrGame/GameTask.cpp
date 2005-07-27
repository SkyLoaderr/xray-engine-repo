///////////////////////////////////////////////////////////////
// GameTask.cpp
// Класс игрового задания задания
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameTask.h"
#include "gameobject.h"
#include "script_engine.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_story_registry.h"
#include "script_game_object.h"
#include "script_space.h"
#include "object_broker.h"
#include "ui/xrXMLParser.h"
#include "encyclopedia_article.h"
#include "map_location.h"
#include "map_manager.h"
ALife::_STORY_ID	story_id		(LPCSTR story_id);
u16					storyId2GameId	(ALife::_STORY_ID);


#include "level.h"
#include "script_space.h"
#include "ai_space.h"
#include "alife_object_registry.h"
#include "alife_story_registry.h"
#include "script_engine.h"
using namespace luabind;

ALife::_STORY_ID	story_id	(LPCSTR story_id)
{
	int res=
							(
		object_cast<int>(
			luabind::object(
				luabind::get_globals(
					ai().script_engine().lua()
				)
				["story_ids"]
			)
			[story_id]
		)
	);
	return ALife::_STORY_ID(res);
}

u16 storyId2GameId	(ALife::_STORY_ID id)
{
	if(ai().get_alife()){ 
		return ai().alife().story_objects().object(id, false)->ID;
	}else{
		u32 cnt = Level().Objects.o_count();
		for(u32 it=0;it<cnt;++it){
			CObject* O = Level().Objects.o_get_by_iterator(it);
			CGameObject* GO = smart_cast<CGameObject*>(O);
			if(GO->story_id()==id)
				return GO->ID();
		}
	return u16(-1);
	}
}

CUIXml	g_gameTaskXml;
bool	g_gameTaskXmlInited = false;

CGameTask::CGameTask(const TASK_ID& id)
{
	m_ReceiveTime	= 0;
	m_FinishTime	= 0;
	m_Title			= NULL;
	m_ID			= id;
	Load			(id);
}

void CGameTask::Load(const TASK_ID& id)
{
	if(!g_gameTaskXmlInited)
		g_gameTaskXml.Init			(CONFIG_PATH, "gameplay", "game_tasks.xml");
	XML_NODE* task_node = g_gameTaskXml.NavigateToNodeWithAttribute("game_task","id",*id);

	THROW3(task_node, "game task id=", *id);

	int tag_num = g_gameTaskXml.GetNodesNum(task_node, "objective");

	m_Objectives.clear		();
	m_Objectives.reserve	(tag_num);

	m_Title = g_gameTaskXml.Read(task_node, "title", 0, NULL);

	for(int i=0; i<tag_num; i++)
	{
		SGameTaskObjective				objective;
//		objective.task_state			= eTaskStateInProgress; //todo: move to ctor. moved
		LPCSTR tag_text					= g_gameTaskXml.Read(task_node, "objective:text", i, NULL);
		objective.description			= tag_text;
		tag_text						= g_gameTaskXml.Read(task_node, "objective:article", i, NULL);
		if(tag_text)
			objective.article_id		= tag_text;

		objective.icon_texture_name		= g_gameTaskXml.Read(task_node, "objective:icon", i, NULL);
		if(objective.icon_texture_name.size()){
			objective.icon_rect.x1			= g_gameTaskXml.ReadAttribFlt(task_node, "objective:icon", i, "x");
			objective.icon_rect.y1			= g_gameTaskXml.ReadAttribFlt(task_node, "objective:icon", i, "y");
			objective.icon_rect.x2			= g_gameTaskXml.ReadAttribFlt(task_node, "objective:icon", i, "width");
			objective.icon_rect.y2			= g_gameTaskXml.ReadAttribFlt(task_node, "objective:icon", i, "height");
		}

		objective.map_location			= g_gameTaskXml.Read(task_node, "objective:map_location_type", i, NULL);
		LPCSTR object_story_id			= g_gameTaskXml.Read(task_node, "objective:object_story_id", i, NULL);
		objective.def_location_enabled	= (NULL == g_gameTaskXml.Read(task_node, "objective:map_location_hidden", i, NULL));
		bool b1,b2;
		b1 = (0==objective.map_location.size());
		b2 = (NULL==object_story_id);
		VERIFY3(b1==b2,"incorrect task objective definition for: ",*objective.description);
		objective.object_id = u16(-1);
		if(object_story_id){
			ALife::_STORY_ID _sid = story_id(object_story_id);
			objective.object_id = storyId2GameId(_sid);
		}
		m_Objectives.push_back(objective);
	}
}

bool CGameTask::HighlightedSpotOnMap(int objective_id)
{
	CMapLocation* ml = m_Objectives[objective_id].HasMapLocation();
	return (ml && ml->PointerEnabled	());
}

void CGameTask::HighlightSpotOnMap(int objective_id, bool bHighlight)
{
	Level().MapManager().DisableAllPointers();
	CMapLocation* ml = m_Objectives[objective_id].HasMapLocation();
	
	if(NULL==ml) return;

	if(bHighlight)
		ml->EnablePointer();
	else
		ml->DisablePointer();
}

bool CGameTask::HasMapLocations			()
{
	for(u32 i=0; i<m_Objectives.size(); ++i)
		if(m_Objectives[i].HasMapLocation())return true;

	return false;
}

void CGameTask::ShowLocations			(bool bShow)
{
	for(u32 i=0; i<m_Objectives.size(); ++i){
		SGameTaskObjective& obj = 	m_Objectives[i];
		CMapLocation* ml		= obj.HasMapLocation		();
		if(NULL!=ml){
			if(bShow)
				ml->EnableSpot();
			else
				ml->DisableSpot();
		}
	}
}

bool CGameTask::ShownLocations			()
{
	for(u32 i=0; i<m_Objectives.size(); ++i){
		SGameTaskObjective& obj						= m_Objectives[i];
		CMapLocation* ml  = obj.HasMapLocation		();
		if(ml) return ml->SpotEnabled				();
	}
	return false;
}


CMapLocation* SGameTaskObjective::HasMapLocation		()
{
	if( map_location.size()==0) return NULL;
	return Level().MapManager().GetMapLocation(map_location, object_id);
}

void SGameTaskObjective::SetTaskState		(ETaskState new_state)
{
	task_state = new_state;
	if( (new_state==eTaskStateFail) || (new_state==eTaskStateCompleted) ){
		CMapLocation* ml = HasMapLocation();
		if(ml)
			Level().MapManager().RemoveMapLocation(map_location, object_id);
	}
}
