#include "stdafx.h"
#include "GameTaskManager.h"
#include "alife_registry_wrappers.h"
#include "ui/xrXMLParser.h"
#include "GameTask.h"
#include "Level.h"
#include "map_manager.h"
#include "map_location.h"
#include "HUDManager.h"
#include "actor.h"
#include "UIGameSP.h"
#include "ui/UIPDAWnd.h"
#include "encyclopedia_article.h"
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

CGameTask* CGameTaskManager::GiveGameTaskToActor(const TASK_ID& id, bool bCheckExisting)
{
	if(bCheckExisting && HasGameTask(id)) return NULL;
	CGameTask* t					= xr_new<CGameTask>(id);

	return GiveGameTaskToActor		(t, bCheckExisting);
}

CGameTask*	CGameTaskManager::GiveGameTaskToActor(CGameTask* t, bool bCheckExisting)
{
	if(bCheckExisting && HasGameTask(t->m_ID)) return NULL;
	m_flags.set					(eChanged, TRUE);

	GameTasks().push_back			(SGameTaskKey(t->m_ID) );
	GameTasks().back().game_task	= t;
	t->m_ReceiveTime				= Level().GetGameTime();

	ARTICLE_VECTOR& article_vector = Actor()->encyclopedia_registry->registry().objects();


	SGameTaskObjective	*obj = NULL;
	for (u32 i = 0; i < t->m_Objectives.size(); ++i){
		obj = &t->m_Objectives[i];
		if(obj->article_id.size()){
		FindArticleByIDPred pred(obj->article_id);
		if( std::find_if(article_vector.begin(), article_vector.end(), pred) == article_vector.end() ){
			CEncyclopediaArticle article;
			article.Load(obj->article_id);
			article_vector.push_back(ARTICLE_DATA(obj->article_id, Level().GetGameTime(), article.data()->articleType));
			}
		}

		if(obj->object_id!=u16(-1) && obj->map_location.size() && obj->def_location_enabled){
			CMapLocation* ml =	Level().MapManager().AddMapLocation(obj->map_location, obj->object_id);
			if(obj->map_hint.size())	ml->SetHint(obj->map_hint);
			ml->DisablePointer			();
			ml->SetSerializable			(true);
		}
	}
	if	(t->m_ID != "user_task" && CGameTask::m_game_task_flags.test(CGameTask::eForceNewTaskActivation)){
		t->HighlightSpotOnMap(1, true);
	}


	//установить флажок необходимости прочтения тасков в PDA
	if(HUD().GetUI()){
		CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
		if(pGameSP) 
			pGameSP->PdaMenu->PdaContentsChanged	(pda_section::quests);
	}
	if(t->m_ID!="user_task")
		t->Objective(0).ChangeStateCallback();

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
	if(pGameSP) {
		pGameSP->PdaMenu->PdaContentsChanged	(pda_section::quests);
	}

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
			if(	!obj.def_location_enabled && obj.TaskState()==eTaskStateInProgress && t->Objective(i-1).TaskState()==eTaskStateCompleted )
			{
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
//		it			= GameTasks().begin();
		bool bDone=false;
		GameTasks::reverse_iterator rit = GameTasks().rbegin();
		GameTasks::reverse_iterator rit_e = GameTasks().rend();

		for( ;(rit!=rit_e)&&(!bDone); ++rit ){
			CGameTask* t		= (*rit).game_task;
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


SGameTaskObjective* CGameTaskManager::ActiveTask()
{
	GameTasks_it it			= GameTasks().begin();
	GameTasks_it it_e		= GameTasks().end();
	for( ;it!=it_e; ++it ){
		CGameTask* t		= (*it).game_task;

		if(t->Objective(0).TaskState()==eTaskStateFail ||
			t->Objective(0).TaskState()==eTaskStateCompleted) continue;

		for(u32 i=1; i<t->m_Objectives.size() ;++i){
			SGameTaskObjective& obj = t->Objective(i);

			//1-st enable hidden locations
			if(	((obj.TaskState()==eTaskStateInProgress)||(obj.TaskState()==eTaskUserDefined))	&& 
				obj.object_id!=u16(-1)	)
			{
				CMapLocation* ml			=	obj.HasMapLocation();
				if(ml && ml->PointerEnabled())
					return &obj;
			}
		}
	}
	return NULL;
}
