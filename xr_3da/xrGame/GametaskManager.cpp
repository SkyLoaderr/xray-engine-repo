#include "stdafx.h"
#include "GameTaskManager.h"
#include "alife_registry_wrappers.h"
#include "ui/xrXMLParser.h"
#include "GameTask.h"
#include "Level.h"
#include "map_manager.h"
#include "map_location.h"
#include "HUDManager.h"
#include "ui/UIMainInGameWnd.h"
#include "ui/UIMainInGameWnd.h"
#include "UIGameSP.h"
#include "ui/UIPDAWnd.h"
//#include "ui/UIDiaryWnd.h"
#include "ui/UIEventsWnd.h"

struct FindTaskByID{
	TASK_ID	id;
	FindTaskByID(const TASK_ID& s):id(s){}
	bool operator () (const SGameTaskKey& key){
		return (id==key.task_id);
	}
};

CGameTaskManager::CGameTaskManager()
{
	m_gametasks					= xr_new<CGameTaskWrapper>();
	m_flags.zero				();
	m_flags.set					(eChanged, TRUE);
}

CGameTaskManager::~CGameTaskManager()
{
	delete_data							(m_gametasks);
}

void CGameTaskManager::initialize(u16 id)
{
	m_gametasks->registry().init(id);// actor's id
}

GameTasks&	CGameTaskManager::GameTasks	() 
{
	return m_gametasks->registry().objects();
}

CGameTask* CGameTaskManager::HasGameTask(const TASK_ID& id)
{
	FindTaskByID key(id);
	GameTasks_it it = std::find_if(GameTasks().begin(),GameTasks().end(),key);
	if( it!=GameTasks().end() )
		return (*it).game_task;
	
	return 0;
}

CGameTask*	CGameTaskManager::GiveGameTaskToActor				(const TASK_ID& id, bool bCheckExisting)
{
	if(bCheckExisting && HasGameTask(id)) return NULL;
	m_flags.set					(eChanged, TRUE);

	GameTasks().push_back			(SGameTaskKey(id) );

	CGameTask* t					= xr_new<CGameTask>(id);
	GameTasks().back().game_task	= t;
	t->m_ReceiveTime				= Level().GetGameTime();

	SGameTaskObjective	*obj = NULL;
	for (u32 i = 0; i < t->m_Objectives.size(); ++i){
		obj = &t->m_Objectives[i];
		if(obj->object_id!=u16(-1) && obj->map_location.size() && obj->def_location_enabled){
			CMapLocation* ml =	Level().MapManager().AddMapLocation(obj->map_location, obj->object_id);
			if(obj->map_hint.size())	ml->SetHint(obj->map_hint);
			ml->DisablePointer			();
			ml->SetSerializable			(true);
		}
	}
	


	//установить флажок необходимости прочтения тасков в PDA
	if(HUD().GetUI()){
		HUD().GetUI()->UIMainIngameWnd->SetFlashIconState(CUIMainIngameWnd::efiPdaTask, true);

		CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(pGameSP) 
			pGameSP->PdaMenu->UIEventsWnd->Reload();
	}
	
	ChangeStateCallback(t->m_ID,0,eTaskStateInProgress);
	return t;
}

void CGameTaskManager::SetTaskState(CGameTask* t, int objective_num, ETaskState state)
{
	m_flags.set					(eChanged, TRUE);
	bool isRoot =				(objective_num==0);
	if ((u32)objective_num >= t->m_Objectives.size()) {Msg("wrong objective num for [%s]", *(t->m_ID)); return;}

	SGameTaskObjective& o			= t->Objective(objective_num);
	CMapLocation* ml				= o.HasMapLocation();
	bool bHighlighted				= ml&&ml->PointerEnabled();
	if(((state==eTaskStateFail)||(state==eTaskStateCompleted))&&ml ){
		Level().MapManager().RemoveMapLocation(o.map_location, o.object_id);
		o.map_location	= NULL;
		o.object_id		= u16(-1);
	}

	o.SetTaskState			(state);
	
	//highlight next objective if needed
	if(!isRoot && bHighlighted && objective_num != (int)(t->m_Objectives.size()-1) ){//not last
		SGameTaskObjective& next_o			= t->Objective(objective_num+1);
		if(next_o.HasMapLocation())
			t->HighlightSpotOnMap	(objective_num+1, true);
	}


	if(isRoot){//setState for task and all sub-tasks
		
		for(u32 i=0; i<t->m_Objectives.size();++i)
			if( t->Objective(i).TaskState()==eTaskStateInProgress )
				SetTaskState(t,i,state);
	}
	
	if(0 == objective_num && eTaskStateCompleted == state || eTaskStateFail == state)
		t->m_FinishTime = Level().GetGameTime();


	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(pGameSP) 
		pGameSP->PdaMenu->UIEventsWnd->Reload();
}

void CGameTaskManager::SetTaskState(const TASK_ID& id, int objective_num, ETaskState state)
{
	CGameTask* t				= HasGameTask(id);
	if (NULL==t)				{Msg("actor does not has task [%s]", *id);	return;}
	SetTaskState				(t,objective_num, state);
}

void CGameTaskManager::UpdateTasks						()
{
	GameTasks_it it		= GameTasks().begin();
	GameTasks_it it_e	= GameTasks().end();
	for( ;it!=it_e; ++it ){
		CGameTask* t		= (*it).game_task;
		for(u32 i=0; i<t->m_Objectives.size() ;++i){
			SGameTaskObjective& obj = t->Objective(i);
			if(obj.TaskState()!=eTaskStateInProgress && i==0) break;
			if(obj.TaskState()!=eTaskStateInProgress) continue;

			ETaskState state = obj.UpdateState();

			if( (state==eTaskStateFail || state==eTaskStateCompleted))
				SetTaskState(t, i, state);
		}
	}

	if(	m_flags.test(eChanged) )
		UpdateActiveTask	();
}


void CGameTaskManager::UpdateActiveTask				()
{
	GameTasks_it it			= GameTasks().begin();
	GameTasks_it it_e		= GameTasks().end();
	bool bHasSpotPointer	= false;
	for( ;it!=it_e; ++it ){
		CGameTask* t		= (*it).game_task;
		if(t->Objective(0).TaskState()!=eTaskStateInProgress) continue;
		for(u32 i=0; i<t->m_Objectives.size() ;++i){
			SGameTaskObjective& obj = t->Objective(i);
			//1-st enable hidden locations
			if(	(false==obj.def_location_enabled) && t->Objective(i-1).TaskState()==eTaskStateCompleted ){
				if(obj.object_id!=u16(-1) && obj.map_location.size()){
					CMapLocation* ml =	Level().MapManager().AddMapLocation(obj.map_location, obj.object_id);
					if(obj.map_hint.size())		ml->SetHint(obj.map_hint);
					ml->DisablePointer			();
					ml->SetSerializable			(true);

				}
			}
			bHasSpotPointer = bHasSpotPointer || t->HighlightedSpotOnMap(i);
		}
	}
	// highlight new spot pointer
	if(false==bHasSpotPointer){
		it			= GameTasks().begin();
		bool bDone=false;
		for( ;(it!=it_e)&&(!bDone); ++it ){
			CGameTask* t		= (*it).game_task;
			if(t->Objective(0).TaskState()!=eTaskStateInProgress) continue;
			for(u32 i=0; (i<t->m_Objectives.size())&&(!bDone) ;++i){
				
				if(t->Objective(i).TaskState() != eTaskStateInProgress) continue;
				
				if(t->Objective(i).HasMapLocation()){
					t->HighlightSpotOnMap(i,true);
					bDone = true;
				}else
				if(i!=0)
					bDone = true;
			}
		}
	}

	m_flags.set					(eChanged, FALSE);
}

void CGameTaskManager::RemoveUserTask					(CMapLocation* ml)
{
	GameTasks_it it			= GameTasks().begin();
	GameTasks_it it_e		= GameTasks().end();

	for( ;it!=it_e; ++it ){
		CGameTask* t		= (*it).game_task;
		SGameTaskObjective& obj = t->Objective(0);
		if(obj.TaskState()!=eTaskUserDefined) continue;
		if(obj.object_id == ml->ObjectID()){
			GameTasks().erase		(it);
			m_flags.set				(eChanged, TRUE);
			CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
			if(pGameSP) 
				pGameSP->PdaMenu->UIEventsWnd->Reload();
			return;
		}

	}

}


void SGameTaskKey::save(IWriter &stream)
{
	save_data(task_id,					stream);
	save_data(game_task->m_ReceiveTime,	stream);
	save_data(game_task->m_FinishTime,	stream);
	save_data(game_task->m_Title,		stream);

	for(u32 i=0; i<game_task->m_Objectives.size(); ++i){
		save_data(game_task->m_Objectives[i].description,	stream);
		save_data(game_task->m_Objectives[i].map_location,	stream);
		save_data(game_task->m_Objectives[i].object_id,		stream);
		save_data(game_task->m_Objectives[i].task_state,	stream);
		save_data(game_task->m_Objectives[i].def_location_enabled,stream);
		save_data(game_task->m_Objectives[i].map_hint,		stream);
	}
}

void SGameTaskKey::load(IReader &stream)
{
	load_data(task_id,						stream);
	game_task = xr_new<CGameTask>			(task_id);
	load_data(game_task->m_ReceiveTime,		stream);
	load_data(game_task->m_FinishTime,		stream);
	load_data(game_task->m_Title,			stream);

	for(u32 i=0; i<game_task->m_Objectives.size(); ++i){
		load_data(game_task->m_Objectives[i].description,	stream);
		load_data(game_task->m_Objectives[i].map_location,	stream);
		load_data(game_task->m_Objectives[i].object_id,		stream);
		load_data(game_task->m_Objectives[i].task_state,	stream);
		load_data(game_task->m_Objectives[i].def_location_enabled,stream);
		load_data(game_task->m_Objectives[i].map_hint,stream);

		
	}
}

void SGameTaskKey::destroy()
{
	delete_data(game_task);
}

