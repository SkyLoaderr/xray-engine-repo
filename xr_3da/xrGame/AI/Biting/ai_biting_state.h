////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_state.h
//	Created 	: 27.06.2003
//  Modified 	: 27.06.2003
//	Author		: Serge Zhem
//	Description : FSM states
////////////////////////////////////////////////////////////////////////////

#pragma once 

#include "../ai_monster_mem.h"
#include "../ai_monster_state.h"

class CAI_Biting;

#define IS_NEED_REBUILD() pMonster->NeedRebuildPath(2,0.5f)


#include "ai_biting_state_attack.h"
#include "ai_biting_state_exploreNDE.h"
#include "ai_biting_state_panic.h"
#include "ai_biting_state_misc.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingRest class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingRest : public IState {
	CAI_Biting	*pMonster;
	
	enum {
			ACTION_WALK,
			ACTION_SATIETY_GOOD,
			ACTION_SLEEP,
			ACTION_WALK_CIRCUMSPECTION,
			ACTION_WALK_PATH_END
	} m_tAction;
	
	TTime			m_dwLastPlanTime;					//!< ��������� ����� ������������

	bool			m_bFollowPath;

	typedef IState inherited;

public:
					CBitingRest		(CAI_Biting *p);

	virtual TTime	UnlockState		(TTime cur_time);
private:
	virtual void	Run();
	virtual	void	Init();
			void	Replanning();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingEat class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingEat : public IState {
	typedef IState inherited;
	CAI_Biting		*pMonster;
	bool			bCanDrag;

	enum {
		ACTION_CORPSE_APPROACH_RUN,
		ACTION_CORPSE_APPROACH_WALK,
		ACTION_PREPARE_DRAG,
		ACTION_DRAG,
		ACTION_WALK_LITTLE_AWAY,
		ACTION_LOOK_AROUND,
		ACTION_WALK,
		ACTION_EAT,
		ACTION_GET_HIDE,
		ACTION_LITTLE_REST,
	} m_tAction;

	CEntity			*pCorpse;
	float			m_fDistToCorpse;			//!< ��������� �� �����

	TTime			m_dwLastTimeEat;
	TTime			m_dwEatInterval;

	Fvector			SavedPos;					// ���������� ������� �� �����
	float			m_fDistToDrag;				// �� ����� ���������� ������
	bool			bDragging;
	bool			bEatRat;

	bool			flag_once_1;
	TTime			m_dwStandStart;
	TTime			m_dwPrepareDrag;
	bool			bEating;

	bool			bRestAfterLunch;
	bool			bHideAfterLunch;
	TTime			m_dwTimeStartRest;
	TTime			rebuild_path;

	bool			Captured;
	TTime			m_dwLastImpulse;

public:
					CBitingEat		(CAI_Biting *p, bool pmt_can_drag);

	virtual	void	Reset			();

private:
	virtual void	Init			();
	virtual void	Run				();
	virtual void	Done			();
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingHide class	// ����� ����������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBitingHide : public IState {
	CAI_Biting		*pMonster;

	VisionElem		m_tEnemy;

	typedef IState inherited;
public:
					CBitingHide			(CAI_Biting *p);

	virtual void	Reset			();	
	
private:

	virtual void	Init			();
	virtual void	Run				();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingDetour class	// ����� ����������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBitingDetour : public IState {
	CAI_Biting		*pMonster;

	VisionElem		m_tEnemy;

	typedef IState inherited;
public:
					CBitingDetour			(CAI_Biting *p);

	virtual void	Reset			();	

private:
	virtual void	Init			();
	virtual void	Run				();
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreDNE class	// Explore danger-non-expedient enemy
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBitingExploreDNE : public IState {
	typedef	IState inherited;
	CAI_Biting	*pMonster;

	enum {
		ACTION_RUN_AWAY,
		ACTION_LOOK_BACK_POSITION,
		ACTION_LOOK_AROUND
	} m_tAction;

	SoundElem		m_tSound;
	bool			flag_once_1;

	Fvector 		SavedPosition;
	Fvector 		StartPosition;
	Fvector 		LastPosition;

	float			m_fRunAwayDist;

	TTime			m_dwLastPosSavedTime;
	TTime			m_dwStayLastTimeCheck;

	bool			m_bInvisibility;

public:
					CBitingExploreDNE	(CAI_Biting *p, bool bVisibility);
private:
	virtual	void	Init				();
	virtual	void	Run					();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreDE class	// Explore danger-expedient enemy //  ���������� �� ��������, ��������	
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBitingExploreDE : public IState {
	CAI_Biting		*pMonster;
	VisionElem		m_tEnemy;

	TTime			m_dwTimeToTurn;
	TTime			m_dwSoundTime;

	enum {
		ACTION_LOOK_AROUND,
		ACTION_HIDE,
	} m_tAction;


	typedef IState inherited;
public:
					CBitingExploreDE	(CAI_Biting *p);

//	virtual void	Reset				();
	virtual bool	CheckCompletion		();

private:
	virtual void	Init				();
	virtual void	Run					();
};


//////////////////////////////////////////////////////////////////////////
// �������� ��������� CBitingNull
//////////////////////////////////////////////////////////////////////////
class CBitingNull : public IState {
public:
	void Run() {}
};

//////////////////////////////////////////////////////////////////////////
// �������� ��������� CBitingTest
//////////////////////////////////////////////////////////////////////////

class CBitingTest : public IState {
	typedef IState inherited;
	CAI_Biting		*pMonster;

	u32		vertex_id;
	TTime	last_time;
	u32		start_vertex_id;	

public:
	CBitingTest(CAI_Biting *p);
	
	void Init();
	void Run();
};

//////////////////////////////////////////////////////////////////////////
// ���������� ����� ���������� ���������
//////////////////////////////////////////////////////////////////////////
class CBitingSquadTask : public IState {
	typedef IState inherited;
	CAI_Biting		*pMonster;

	enum {
		ACTION_DANGER_1,
		ACTION_DANGER_2,
		ACTION_DANGER_3,
		ACTION_DANGER_4
	} m_tAction;

	bool			flag_once_1;
	Fvector			saved_pos;
	TTime			time_last_switch;

public:
	CBitingSquadTask (CAI_Biting *p);
private:
	virtual void	Init				();
	virtual void	Run					();
};


class CBitingSearchEnemy : public IState {
	typedef IState inherited;
	CAI_Biting		*pMonster;

	enum {
		ACTION_SEARCH_ENEMY_INIT,
		ACTION_SEARCH_ENEMY
	} m_tAction;
	
	VisionElem		m_tEnemy;
	u32				search_vertex_id;

	TTime			RebuildPathInterval;

public:
	CBitingSearchEnemy(CAI_Biting *p);
private:
	virtual void	Init				();
	virtual void	Run					();
	virtual void	Done				();
};

