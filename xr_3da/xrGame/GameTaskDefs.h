///////////////////////////////////////////////////////////////
// GameTaksDefs.h
// Класс игрового задания задания
///////////////////////////////////////////////////////////////

#pragma once

enum ETaskState {
	eTaskStateFail			= 0,
	eTaskStateInProgress,
	eTaskStateCompleted,
	eTaskUserDefined,
	eTaskStateDummy			= u32(-1)
};

//for scripting access
struct STaskObjective
{
	shared_str					m_name;
	ETaskState					m_state;
	LPCSTR						GetName()						{return *m_name;}
};

struct STask{
	shared_str					m_name;
	ETaskState					m_state;
	xr_vector<STaskObjective>	m_objectives;

	LPCSTR						GetName()						{return *m_name;}
	int							GetObjectivesNum()				{return m_objectives.size();}
	STaskObjective				GetObjective(int idx)			{return m_objectives[idx];}

};
struct STasks{
	xr_vector<STask>	m_all_tasks;
	int					Size() {return m_all_tasks.size();}
	STask				GetAt(int idx) {return m_all_tasks[idx];}
};





typedef shared_str		TASK_ID;
DEFINE_VECTOR		(TASK_ID, TASK_ID_VECTOR, TASK_ID_IT);

//состояние цели и задания

#include "alife_abstract_registry.h"

class CGameTask;

struct SGameTaskKey : public IPureSerializeObject<IReader,IWriter>,public IPureDestroyableObject {
	TASK_ID			task_id;
	CGameTask*		game_task;
	SGameTaskKey	(TASK_ID t_id):task_id(t_id),game_task(NULL){};
	SGameTaskKey	():task_id(NULL),game_task(NULL){};


	virtual void save								(IWriter &stream);
	virtual void load								(IReader &stream);
	virtual void destroy							();
};

DEFINE_VECTOR (SGameTaskKey, GameTasks, GameTasks_it);

struct CGameTaskRegistry : public CALifeAbstractRegistry<u16, GameTasks> {
	virtual void save(IWriter &stream){
		CALifeAbstractRegistry<u16, GameTasks>::save(stream);
	};
};
