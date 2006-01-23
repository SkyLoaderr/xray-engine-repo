#pragma once

#include "encyclopedia_article_defs.h"
#include "GameTaskDefs.h"
#include "script_export_space.h"
#include "script_space.h"

class CGameTaskManager;
class CMapLocation;
class CGameTask;

class SScriptObjectiveHelper: public IPureSerializeObject<IReader,IWriter>
{
public:
	xr_vector<shared_str>	m_s_complete_lua_functions;
	xr_vector<shared_str>	m_s_fail_lua_functions;

	xr_vector<shared_str>	m_s_lua_functions_on_complete;
	xr_vector<shared_str>	m_s_lua_functions_on_fail;
public:
	bool			not_empty		() {return m_s_complete_lua_functions.size()	||
												m_s_fail_lua_functions.size()		||
												m_s_lua_functions_on_complete.size()||
												m_s_lua_functions_on_fail.size		() ;}

	virtual void			save			(IWriter &stream);
	virtual void			load			(IReader &stream);
			
			void			init_functors	(xr_vector<shared_str>& v_src, xr_vector<luabind::functor<bool> >& v_dest);
};

class SGameTaskObjective : public IPureSerializeObject<IReader,IWriter>
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
	virtual void			save			(IWriter &stream);
	virtual void			load			(IReader &stream);
	
	SScriptObjectiveHelper	m_pScriptHelper;
	SGameTaskObjective		(CGameTask* parent, int idx);
	SGameTaskObjective		();
	shared_str				description;
	ARTICLE_ID				article_id;
	shared_str				map_hint;
	shared_str				map_location;
	u16						object_id;
	shared_str				article_key;
	CMapLocation*			HasMapLocation		();
	ETaskState				TaskState			()	{return task_state;};
	ETaskState				UpdateState			();

	shared_str							icon_texture_name;
	Frect								icon_rect;
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

// for scripting access
	void					SetDescription_script	(LPCSTR _descr);
	void					SetArticleID_script		(LPCSTR _id);
	void					SetMapHint_script		(LPCSTR _str);
	void					SetMapLocation_script	(LPCSTR _str);
	void					SetObjectID_script		(u16 id);
	void					SetArticleKey_script	(LPCSTR _str);

	void					AddCompleteInfo_script	(LPCSTR _str);
	void					AddFailInfo_script		(LPCSTR _str);
	void					AddOnCompleteInfo_script(LPCSTR _str);
	void					AddOnFailInfo_script	(LPCSTR _str);

	void					AddCompleteFunc_script	(LPCSTR _str);
	void					AddFailFunc_script		(LPCSTR _str);
	void					AddOnCompleteFunc_script(LPCSTR _str);
	void					AddOnFailFunc_script	(LPCSTR _str);
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
							CGameTask				();

	bool					HighlightedSpotOnMap	(int objective_id);					
	void					HighlightSpotOnMap		(int objective_id, bool bHighlight);
	
	bool					HasMapLocations			();
	void					ShowLocations			(bool bShow);
	bool					ShownLocations			();
	
	SGameTaskObjective&		Objective				(int objectice_id)	{return m_Objectives[objectice_id];};

	TASK_ID					m_ID;
	shared_str				m_Title;
	OBJECTIVE_VECTOR		m_Objectives;
	ALife::_TIME_ID			m_ReceiveTime;
	ALife::_TIME_ID			m_FinishTime;
	bool					m_is_task_general;

	enum		{eForceNewTaskActivation =(1<<0),};
	static		Flags32		m_game_task_flags;

// for scripting access
	void					Load_script				(LPCSTR _id);
	void					SetTitle_script			(LPCSTR _title);
	void					AddObjective_script		(SGameTaskObjective* O);
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CGameTask)
#undef script_type_list
#define script_type_list save_type_list(CGameTask)

void ChangeStateCallback	(shared_str& task_id, int obj_id, ETaskState state);