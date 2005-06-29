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
/*
TASK_DATA::TASK_DATA():task_id(NULL),receive_time(0),finish_time(0)
{
}

TASK_DATA::TASK_DATA(TASK_ID id, ALife::_TIME_ID time):task_id(id),receive_time(time),finish_time(0)
{
	if(!task_id.size()) return;
	
	CGameTask task;
	task.Load(task_id);
	states.resize(task.ObjectivesNum());
	for(std::size_t i=0; i<states.size();i++){
		states[i].m_state = eTaskStateInProgress;
		states[i].m_flags.zero();
	}
};


void TASK_DATA::load (IReader& stream)
{
	load_data(task_id, stream);
	load_data(receive_time, stream);
	load_data(finish_time, stream);
	load_data(states, stream);
}
void TASK_DATA::save (IWriter& stream)
{
	save_data(task_id, stream);
	save_data(receive_time, stream);
	save_data(finish_time, stream);
	save_data(states, stream);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


SGameTaskData::SGameTaskData ()
{
}
SGameTaskData::~SGameTaskData ()
{}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


CGameTask::CGameTask		()
{
}

CGameTask::~CGameTask		()
{
}

void CGameTask::Load	(TASK_ID  id)
{
	m_TaskId = id;
	inherited_shared::load_shared(m_TaskId, NULL);
}


void CGameTask::load_shared	(LPCSTR)
{
	const id_to_index::ITEM_DATA& item_data = *id_to_index::GetById(m_TaskId);

	CUIXml uiXml;
	string_path xml_file_full;
	strconcat	(xml_file_full, *shared_str(item_data.file_name), ".xml");

	bool xml_result = uiXml.Init(CONFIG_PATH, GAME_PATH, xml_file_full);
	THROW3(xml_result, "xml file not found", xml_file_full);

	//loading from XML
	XML_NODE* task_node = uiXml.NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	THROW3(task_node, "game task id=", *item_data.id);

	int tag_num = uiXml.GetNodesNum(task_node, "objective");

	data()->m_Objectives.clear		();
	data()->m_Objectives.reserve	(tag_num);

	data()->title = uiXml.Read(task_node, "title", 0, NULL);

	for(int i=0; i<tag_num; i++)
	{
		SGameTaskObjective objective;
		LPCSTR tag_text = uiXml.Read(task_node, "objective:text", i, NULL);
		objective.description = tag_text;
		tag_text = uiXml.Read(task_node, "objective:article", i, NULL);
		if(tag_text)
			objective.article_id = tag_text;

		objective.icon_texture_name		= uiXml.Read(task_node, "objective:icon", i, NULL);
		objective.icon_x				= uiXml.ReadAttribFlt(task_node, "objective:icon", i, "x");
		objective.icon_y				= uiXml.ReadAttribFlt(task_node, "objective:icon", i, "y");
		objective.icon_width			= uiXml.ReadAttribFlt(task_node, "objective:icon", i, "width");
		objective.icon_height			= uiXml.ReadAttribFlt(task_node, "objective:icon", i, "height");

		objective.map_location			= uiXml.Read(task_node, "objective:map_location_type", i, NULL);
		LPCSTR object_story_id		= uiXml.Read(task_node, "objective:object_story_id", i, NULL);
		bool b1,b2;
		b1 = (0==objective.map_location.size());
		b2 = (NULL==object_story_id);
		VERIFY3(b1==b2,"incorrect task objective definition for: ",*objective.description);
		objective.object_id = u16(-1);
		if(object_story_id){
			ALife::_STORY_ID _sid = story_id(object_story_id);
			objective.object_id = storyId2GameId(_sid);
		}


		data()->m_Objectives.push_back(objective);
	}
}

void CGameTask::InitXmlIdToIndex()
{
	if(!id_to_index::tag_name)
		id_to_index::tag_name = "game_task";
	if(!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("game_tasks", "files");
}

u32			CGameTask::ObjectivesNum	()
{
	return	data()->m_Objectives.size();
}
shared_str		CGameTask::ObjectiveDesc	(u32 index)
{
	return data()->m_Objectives[index].description;
}

ARTICLE_ID		CGameTask::ObjectiveArticle	(u32 index)
{
	return data()->m_Objectives[index].article_id;
}

ETaskState	CGameTask::ObjectiveState  (u32 index)
{
	VERIFY(index<m_ObjectiveStates.size());
	return m_ObjectiveStates[index].m_state;
}

void 	CGameTask::ObjectiveIcon  (u32 index, shared_str& tex_name, 
								   float& x, float& y, float& width, float& height)
{
	VERIFY(index<m_ObjectiveStates.size());
	tex_name = data()->m_Objectives[index].icon_texture_name;
	x = data()->m_Objectives[index].icon_x;
	y = data()->m_Objectives[index].icon_y;
	width = data()->m_Objectives[index].icon_width;
	height = data()->m_Objectives[index].icon_height;
}


void CGameTask::Init(const TASK_DATA& task_data)
{
	Load				(task_data.task_id);
	m_ObjectiveStates	= task_data.states;

	m_ReceiveTime		= task_data.receive_time;
	m_FinishTime		= task_data.finish_time;
}
*/

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
	Load			(id);
}

void CGameTask::Load(const TASK_ID& id)
{
	if(!g_gameTaskXmlInited)
		g_gameTaskXml.Init			(CONFIG_PATH, "gameplay", "game_tasks.xml");
	//loading from XML
//	XML_NODE* task_node = g_gameTaskXml.NavigateToNode(*id);
	XML_NODE* task_node = g_gameTaskXml.NavigateToNodeWithAttribute("game_task","id",*id);

	THROW3(task_node, "game task id=", *id);

	int tag_num = g_gameTaskXml.GetNodesNum(task_node, "objective");

	m_Objectives.clear		();
	m_Objectives.reserve	(tag_num);

	m_Title = g_gameTaskXml.Read(task_node, "title", 0, NULL);

	for(int i=0; i<tag_num; i++)
	{
		SGameTaskObjective				objective;
		objective.task_state			= eTaskStateInProgress; //todo: move to ctor
		objective.flags.zero			();	
		LPCSTR tag_text					= g_gameTaskXml.Read(task_node, "objective:text", i, NULL);
		objective.description			= tag_text;
		tag_text						= g_gameTaskXml.Read(task_node, "objective:article", i, NULL);
		if(tag_text)
			objective.article_id		= tag_text;

		objective.icon_texture_name		= g_gameTaskXml.Read(task_node, "objective:icon", i, NULL);
		objective.icon_rect.x1			= g_gameTaskXml.ReadAttribFlt(task_node, "objective:icon", i, "x");
		objective.icon_rect.y1			= g_gameTaskXml.ReadAttribFlt(task_node, "objective:icon", i, "y");
		objective.icon_rect.x2			= g_gameTaskXml.ReadAttribFlt(task_node, "objective:icon", i, "width");
		objective.icon_rect.y2			= g_gameTaskXml.ReadAttribFlt(task_node, "objective:icon", i, "height");

		objective.map_location			= g_gameTaskXml.Read(task_node, "objective:map_location_type", i, NULL);
		LPCSTR object_story_id			= g_gameTaskXml.Read(task_node, "objective:object_story_id", i, NULL);
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

bool CGameTask::Highlighted(int objective_id)
{
	return !!m_Objectives[objective_id].flags.test(eTaskFlagMapPointer);
}

void CGameTask::Highlight(int objective_id, bool bHighlight)
{
	m_Objectives[objective_id].flags.set(eTaskFlagMapPointer,bHighlight);
	CMapLocation* ml = Level().MapManager().GetMapLocation(m_Objectives[objective_id].map_location, m_Objectives[objective_id].object_id);
	if(ml){
		if(bHighlight)
			ml->EnablePointer();
		else
			ml->DisablePointer();
	};

	if(objective_id==0){
		int c = 1;
		for(;c<(int)m_Objectives.size();++c){
			Highlight(c,bHighlight);
		}
	}
}

