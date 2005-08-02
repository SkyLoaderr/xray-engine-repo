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
}

CGameTaskManager::~CGameTaskManager()
{
	delete_data		(m_gametasks);
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

	GameTasks().push_back			(SGameTaskKey(id) );

	CGameTask* t					= xr_new<CGameTask>(id);
	GameTasks().back().game_task	= t;
	t->m_ReceiveTime				= Level().GetGameTime();

	SGameTaskObjective	*obj = NULL;
	for (u32 i = 0; i < t->m_Objectives.size(); ++i){
		obj = &t->m_Objectives[i];
		if(obj->object_id!=u16(-1) && obj->map_location.size() && obj->def_location_enabled){
			CMapLocation* ml =	Level().MapManager().AddMapLocation(obj->map_location, obj->object_id);
			ml->DisablePointer			();
		}
	}
	


	//установить флажок необходимости прочтения тасков в PDA
	if(HUD().GetUI()){
		HUD().GetUI()->UIMainIngameWnd->SetFlashIconState(CUIMainIngameWnd::efiPdaTask, true);

		CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(pGameSP) 
			pGameSP->PdaMenu->UIEventsWnd->Reload();
	}
	return t;
}

void CGameTaskManager::SetTaskState(const TASK_ID& id, int objective_num, ETaskState state)
{
	CGameTask* t= HasGameTask(id);
	if(NULL==t){
		Msg		("actor does not has task [%s]", *id);
		return;
	}

	if ((std::size_t)objective_num >= t->m_Objectives.size()) {
		Msg		("wrong objective num for [%s]", *id);
		return;
	}

	if(t->m_Objectives[objective_num].m_bTaskDependent && state == eTaskStateFail) objective_num = 0;//for all

	bool bHasPolinter = t->HighlightedSpotOnMap(objective_num);

	t->m_Objectives[objective_num].SetTaskState	(state);

	if(0 == objective_num){//setState for task and all sub-tasks
		
		for(u32 i=0; i<t->m_Objectives.size();++i)
			if( t->m_Objectives[i].TaskState()==eTaskStateInProgress )
				t->m_Objectives[i].SetTaskState(state);
	}
	
	if(0!=objective_num && objective_num != (int)t->m_Objectives.size()-1){
		SGameTaskObjective&	obj = t->m_Objectives[objective_num+1];
		if( (false == obj.def_location_enabled)						&& 
			(obj.TaskState()==eTaskStateInProgress)	&&
			(obj.map_location.size())		){
				CMapLocation* ml =	Level().MapManager().AddMapLocation(obj.map_location, obj.object_id);
				ml->DisablePointer			();
			}
		if(bHasPolinter && (obj.TaskState()==eTaskStateInProgress) && (obj.HasMapLocation()) )
			t->HighlightSpotOnMap(objective_num+1, true);
	}

	//если мы устанавливаем финальное состояние для основного задания, то
	//запомнить время выполнения
	if(0 == objective_num && eTaskStateCompleted == state || eTaskStateFail == state)
	{
		t->m_FinishTime = Level().GetGameTime();
	}
	CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
	if(pGameSP) 
		pGameSP->PdaMenu->UIEventsWnd->Reload();

}

void CGameTaskManager::UpdateTasks						()
{
	return;
	GameTasks_it it		= GameTasks().begin();
	GameTasks_it it_e	= GameTasks().end();
	for( ;it!=it_e; ++it ){
		CGameTask* t		= (*it).game_task;
		if(t->m_Objectives[0].TaskState()!=eTaskStateInProgress) continue;

		for(u32 i=0; i<t->m_Objectives.size() ;++i){
			SGameTaskObjective& obj = t->m_Objectives[i];
			if(obj.TaskState()!=eTaskStateInProgress) continue;

			ETaskState state = obj.UpdateState();

			if( (state==eTaskStateFail || state==eTaskStateCompleted)){
				SetTaskState(t->m_ID, i, state);
				return;
			}
		}
	}
}


void CGameTaskManager::UpdateActiveTask				()
{

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
	}
}

void SGameTaskKey::destroy()
{
	delete_data(game_task);
}

