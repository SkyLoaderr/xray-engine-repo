////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_state.h
//	Created 	: 27.06.2003
//  Modified 	: 27.06.2003
//	Author		: Serge Zhem
//	Description : FSM states
////////////////////////////////////////////////////////////////////////////

#pragma once 

#include "..\\ai_monster_mem.h"
#include "..\\ai_monster_state.h"

class CAI_Biting;

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
	
	TTime			m_dwReplanTime;						//!< ����� �����, ������� ������ ������������
	TTime			m_dwLastPlanTime;					//!< ��������� ����� ������������

	bool			m_bFollowPath;

	typedef IState inherited;

public:
					CBitingRest		(CAI_Biting *p);

	virtual void	Reset			();

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

public:
					CBitingEat		(CAI_Biting *p, bool pmt_can_drag);

	virtual	void	Reset			();

private:
	virtual void	Init			();
	virtual void	Run				();
	virtual void	Done			();
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingAttack class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingAttack : public IState {
	typedef	IState inherited;
	CAI_Biting	*pMonster;

	enum {
		ACTION_RUN,
		ACTION_ATTACK_MELEE,
		ACTION_FIND_ENEMY,
		ACTION_STEAL,
		ACTION_THREATEN,
		ACTION_WALK_ANGRY_AROUND,
		ACTION_STAND_ANGRY_FACE_ENEMY,
		ACTION_WALK_AWAY,
		ACTION_FACE_ENEMY,
		ACTION_THREATEN2,
	} m_tAction;

	VisionElem		m_tEnemy;

	bool			m_bAttackRat;

	float			m_fDistMin;						//!< ���������� ���������� ���������� ��� ������
	float			m_fDistMax;						//!< ����������� ���������� ���������� ��� ������
	float			dist;							// ������� ���������

	TTime			m_dwFaceEnemyLastTime;
	TTime			m_dwFaceEnemyLastTimeInterval;

	TTime			m_dwSuperMeleeStarted;

	bool			m_bInvisibility;
	
	bool			flag_once_1;

	bool			bEnemyDoesntSeeMe;

	bool			bCanThreaten;

	Fvector			temp_pos;
	TTime			m_dwTimeWalkingPath;

	bool			once_flag_1,once_flag_2;

	TTime			time_start_walk_away;

	TTime			ThreatenTimeStarted;

public:	
					CBitingAttack	(CAI_Biting *p, bool bVisibility);

	virtual	void	Reset			();

	virtual	void	Init			();
	virtual void	Run				();
	virtual void	Done			();

			bool	CheckThreaten	();
			Fvector RandomPos		(Fvector pos, float R);
			
			void	WalkAngrySubState();

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
// CBitingPanic class	// ������� �� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBitingPanic : public IState {
	CAI_Biting		*pMonster;

	VisionElem		m_tEnemy;

	// implementation of 'face the most open area'
	bool			bFacedOpenArea;
	Fvector			cur_pos;			
	Fvector			prev_pos;
	TTime			m_dwStayTime;

	bool			m_bInvisibility;

	typedef IState inherited;
public:
					CBitingPanic			(CAI_Biting *p, bool invisibility);

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

	virtual void	Reset				();
	virtual bool	CheckCompletion		();

private:
	virtual void	Init				();
	virtual void	Run					();
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreNDE class	// Explore non-danger enemy //  ���� � ������� �����	
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBitingExploreNDE : public IState {
	typedef IState inherited;
	CAI_Biting		*pMonster;
	SoundElem		m_tSound;
	
	enum {
		ACTION_LOOK_DESTINATION,
		ACTION_GOTO_SOUND_SOURCE,
		ACTION_LOOK_AROUND
	} m_tAction;

	bool			flag_once_1;
	bool			flag_once_2;


public:
					CBitingExploreNDE	(CAI_Biting *p);
private:
	virtual void	Init				();
	virtual void	Run					();
};

//////////////////////////////////////////////////////////////////////////
// �������� ��������� CBitingNull
//////////////////////////////////////////////////////////////////////////
class CBitingNull : public IState {
public:
	CBitingNull(CAI_Biting * /**p/**/) {}
	void Run() {}
};

//////////////////////////////////////////////////////////////////////////
// ���������� ����� ���������� ���������
//////////////////////////////////////////////////////////////////////////
class CBitingSquadTask : public IState {
	typedef IState inherited;
	CAI_Biting		*pMonster;
public:
	CBitingSquadTask (CAI_Biting *p);
private:
	virtual void	Init				();
	virtual void	Run					();
};


