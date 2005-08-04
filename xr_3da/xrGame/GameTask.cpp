#include "stdafx.h"
#include "GameTask.h"
#include "ui/xrXMLParser.h"
#include "encyclopedia_article.h"
#include "map_location.h"
#include "map_manager.h"

#include "level.h"
#include "actor.h"
#include "script_space.h"
#include "script_engine.h"
#include "script_engine.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "ai_space.h"
#include "alife_object_registry.h"
#include "alife_simulator.h"
#include "alife_story_registry.h"
#include "game_object_space.h"
#include "object_broker.h"


ALife::_STORY_ID	story_id		(LPCSTR story_id);
u16					storyId2GameId	(ALife::_STORY_ID);

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
	XML_NODE* task_node				= g_gameTaskXml.NavigateToNodeWithAttribute("game_task","id",*id);

	THROW3							(task_node, "game task id=", *id);
	g_gameTaskXml.SetLocalRoot		(task_node);
	m_Title							= g_gameTaskXml.Read(g_gameTaskXml.GetLocalRoot(), "title", 0, NULL);
	int tag_num						= g_gameTaskXml.GetNodesNum(g_gameTaskXml.GetLocalRoot(),"objective");

	m_Objectives.clear		();
	for(int i=0; i<tag_num; i++)
	{
		XML_NODE*	l_root = NULL;
		l_root							= g_gameTaskXml.NavigateToNode("objective",i); 
		g_gameTaskXml.SetLocalRoot		(l_root);
		m_Objectives.push_back			(SGameTaskObjective(this,i));
		SGameTaskObjective&				objective = m_Objectives.back();


		LPCSTR tag_text					= g_gameTaskXml.Read(l_root, "text", 0, NULL);
		objective.description			= tag_text;
		tag_text						= g_gameTaskXml.Read(l_root, "article", 0, NULL);
		if(tag_text)
			objective.article_id		= tag_text;

		objective.icon_texture_name		= g_gameTaskXml.Read(g_gameTaskXml.GetLocalRoot(), "icon", 0, NULL);
		if(objective.icon_texture_name.size()){
			objective.icon_rect.x1			= g_gameTaskXml.ReadAttribFlt(l_root, "icon", 0, "x");
			objective.icon_rect.y1			= g_gameTaskXml.ReadAttribFlt(l_root, "icon", 0, "y");
			objective.icon_rect.x2			= g_gameTaskXml.ReadAttribFlt(l_root, "icon", 0, "width");
			objective.icon_rect.y2			= g_gameTaskXml.ReadAttribFlt(l_root, "icon", 0, "height");
		}

		objective.map_location			= g_gameTaskXml.Read(l_root, "map_location_type", 0, NULL);
		LPCSTR object_story_id			= g_gameTaskXml.Read(l_root, "object_story_id", 0, NULL);
		LPCSTR ddd;
		ddd								= g_gameTaskXml.Read(l_root, "map_location_hidden", 0, NULL);
		if(ddd)
			objective.def_location_enabled = false;

		bool b1,b2;
		b1								= (0==objective.map_location.size());
		b2								= (NULL==object_story_id);
		VERIFY3							(b1==b2,"incorrect task objective definition for: ",*objective.description);
		objective.object_id				= u16(-1);
		if(object_story_id){
			ALife::_STORY_ID _sid		= story_id(object_story_id);
			objective.object_id			= storyId2GameId(_sid);
		}


//------infoportion_complete
		int info_num					= g_gameTaskXml.GetNodesNum(l_root,"infoportion_complete");
		objective.m_completeInfos.resize(info_num);
		int j;
		for(j=0; j<info_num; ++j)
			objective.m_completeInfos[j]= g_gameTaskXml.Read(l_root, "infoportion_complete", j, NULL);

//------infoportion_fail
		info_num						= g_gameTaskXml.GetNodesNum(l_root,"infoportion_fail");
		objective.m_failInfos.resize	(info_num);

		for(j=0; j<info_num; ++j)
			objective.m_failInfos[j]	= g_gameTaskXml.Read(l_root, "infoportion_fail", j, NULL);

//------infoportion_set_complete
		info_num						= g_gameTaskXml.GetNodesNum(l_root,"infoportion_set_complete");
		objective.m_infos_on_complete.resize(info_num);
		for(j=0; j<info_num; ++j)
			objective.m_infos_on_complete[j]= g_gameTaskXml.Read(l_root, "infoportion_set_complete", j, NULL);

//------infoportion_set_fail
		info_num						= g_gameTaskXml.GetNodesNum(l_root,"infoportion_set_fail");
		objective.m_infos_on_fail.resize(info_num);
		for(j=0; j<info_num; ++j)
			objective.m_infos_on_fail[j]= g_gameTaskXml.Read(l_root, "infoportion_set_fail", j, NULL);


//------function_complete
		LPCSTR		str;
		bool functor_exists;
		info_num						= g_gameTaskXml.GetNodesNum(l_root,"function_complete");
		objective.m_complete_lua_functions.resize(info_num);
		for(j=0; j<info_num; ++j){
			str							= g_gameTaskXml.Read(l_root, "function_complete", j, NULL);
			functor_exists				= ai().script_engine().functor(str ,objective.m_complete_lua_functions[j]);
			THROW3						(functor_exists, "Cannot find script function described in task objective  ", str);
		}


//------function_fail
		info_num						= g_gameTaskXml.GetNodesNum(l_root,"function_fail");
		objective.m_fail_lua_functions.resize(info_num);
		for(j=0; j<info_num; ++j){
			str							= g_gameTaskXml.Read(l_root, "function_fail", j, NULL);
			functor_exists				= ai().script_engine().functor(str ,objective.m_fail_lua_functions[j]);
			THROW3						(functor_exists, "Cannot find script function described in task objective  ", str);
		}

		g_gameTaskXml.SetLocalRoot		(task_node);
	}
	g_gameTaskXml.SetLocalRoot		(g_gameTaskXml.GetRoot());
/*
//perform consistency check
	SGameTaskObjective& root_obj		= Objective(0);
	bool b_root_compl					= root_obj.m_completeInfos.size()	|| root_obj.m_complete_lua_functions.size();
	bool b_root_fail					= root_obj.m_failInfos.size()		|| root_obj.m_fail_lua_functions.size();
	VERIFY3								(b_root_compl==b_root_fail,*m_ID,"incorrect task definition");
	
	for(u32 i = 0; i<m_Objectives.size(); ++i)
	{
		SGameTaskObjective& sub_obj			= Objective(i);
		bool b_compl						= sub_obj.m_completeInfos.size()	|| sub_obj.m_complete_lua_functions.size();
		bool b_fail							= sub_obj.m_failInfos.size()		|| sub_obj.m_fail_lua_functions.size();
		VERIFY3								(b_compl==b_fail,*m_ID,"incorrect task objective definition");
		VERIFY3								((b_compl==b_root_compl)&&(b_root_fail==b_fail),*m_ID,"incorrect task objective definition");
	}
*/
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
		if(m_Objectives[i].HasMapLocation()!=NULL)return true;

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

SGameTaskObjective::SGameTaskObjective		(CGameTask* parent, int _idx)
:description		(NULL),
article_id			(NULL),
map_location		(NULL),
object_id			(u16(-1)),
task_state			(eTaskStateInProgress),
def_location_enabled(true),
parent				(parent),
idx					(_idx)
{

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

		if( task_state==eTaskStateFail )
				SendInfo				(m_infos_on_fail);
		else
		if( task_state==eTaskStateCompleted )
				SendInfo				(m_infos_on_complete);

		//callback for scripters
		ChangeStateCallback(parent->m_ID, idx, new_state);
	}
}

ETaskState SGameTaskObjective::UpdateState	()
{
//check fail infos
	if( CheckInfo(m_failInfos) )
		return		eTaskStateFail;

//check fail functor
	if( CheckFunctions(m_fail_lua_functions) )
		return		eTaskStateFail;
	
//check complete infos
	if( CheckInfo(m_completeInfos) )
		return		eTaskStateCompleted;


//check complete functor
	if( CheckFunctions(m_complete_lua_functions) )
		return		eTaskStateCompleted;

	
	return TaskState();
}

void SGameTaskObjective::SendInfo		(xr_vector<shared_str>& v)
{
	CActor* actor						= smart_cast<CActor*>(Level().CurrentEntity()); VERIFY(actor);
	xr_vector<shared_str>::iterator it	= v.begin();
	for(;it!=v.end();++it)
		actor->TransferInfo					((*it),true);

}

bool SGameTaskObjective::CheckInfo		(xr_vector<shared_str>& v)
{
	bool res = false;
	CActor* actor						= smart_cast<CActor*>(Level().CurrentEntity()); VERIFY(actor);
	xr_vector<shared_str>::iterator it	= v.begin();
	for(;it!=v.end();++it){
		res = actor->HasInfo					(*it);
		if(!res) break;
	}
	return res;
}

bool SGameTaskObjective::CheckFunctions	(xr_vector<luabind::functor<bool> >& v)
{
	bool res = false;
	xr_vector<luabind::functor<bool> >::iterator it	= v.begin();
	for(;it!=v.end();++it){
		if( (*it).is_valid() ) res = (*it)(*(parent->m_ID), idx);
		if(!res) break;
	}
	return res;

}

void ChangeStateCallback(shared_str& task_id, int obj_id, ETaskState state){

	CActor* actor			= smart_cast<CActor*>(Level().CurrentEntity()); VERIFY(actor);
	actor->callback(GameObject::eTaskStateChange)(*task_id, obj_id, state);
}