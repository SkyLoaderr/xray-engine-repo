#pragma once

#include "encyclopedia_article_defs.h"
#include "GameTaskDefs.h"
#include "script_space.h"

class CGameTaskManager;
class CMapLocation;
class CGameTask;

struct SGameTaskObjective 
{
	friend struct SGameTaskKey;
	friend class CGameTaskManager;
private:
	ETaskState				task_state;
	CGameTask*				parent;
	int						idx;
	void					SendInfo		(xr_vector<shared_str>&);
	void					CallAllFuncs	(xr_vector<luabind::functor<bool> >& v);
	bool					CheckInfo		(xr_vector<shared_str>&);
	bool					CheckFunctions	(xr_vector<luabind::functor<bool> >& v);
	void					SetTaskState	(ETaskState new_state);
public:
	SGameTaskObjective		(CGameTask* parent, int idx);
	shared_str				description;
	ARTICLE_ID				article_id;
	shared_str				map_hint;
	shared_str				map_location;
	u16						object_id;
	shared_str				article_key;
	CMapLocation*			HasMapLocation		();
	ETaskState				TaskState			()	{return task_state;};
	ETaskState				UpdateState			();

	//прикрипленная иконка
	shared_str							icon_texture_name;
	Frect								icon_rect;//x,y,w,h
	bool								def_location_enabled;
//complete/fail stuff
	xr_vector<shared_str>				m_completeInfos;
	xr_vector<shared_str>				m_failInfos;
	xr_vector<shared_str>				m_infos_on_complete;
	xr_vector<shared_str>				m_infos_on_fail;
	xr_vector<luabind::functor<bool> >	m_complete_lua_functions;
	xr_vector<luabind::functor<bool> >	m_fail_lua_functions;

	xr_vector<luabind::functor<bool> >	m_lua_functions_on_complete;
	xr_vector<luabind::functor<bool> >	m_lua_functions_on_fail;

};

DEFINE_VECTOR(SGameTaskObjective, OBJECTIVE_VECTOR, OBJECTIVE_VECTOR_IT);

class CGameTask
{
private:
							CGameTask				(const CGameTask&){}; //disable copy ctor
protected:
	void					Load					(const TASK_ID& id);
public:
							CGameTask				(const TASK_ID& id);

	bool					HighlightedSpotOnMap	(int objective_id);					
	void					HighlightSpotOnMap		(int objective_id, bool bHighlight);
	
	bool					HasMapLocations			();
	void					ShowLocations			(bool bShow);
	bool					ShownLocations			();
	
	SGameTaskObjective&		Objective				(int objectice_id)	{return m_Objectives[objectice_id];};

	shared_str				m_ID;
	shared_str				m_Title;
	OBJECTIVE_VECTOR		m_Objectives;
	ALife::_TIME_ID			m_ReceiveTime;
	ALife::_TIME_ID			m_FinishTime;
	
};

void ChangeStateCallback	(shared_str& task_id, int obj_id, ETaskState state);