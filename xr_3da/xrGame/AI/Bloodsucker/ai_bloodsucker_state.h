#pragma once

#include "..\\ai_monster_state.h"

class CAI_Bloodsucker;

//////////////////////////////////////////////////////////////////////////
// State REST
//////////////////////////////////////////////////////////////////////////
class CBloodsuckerRest : public IState {
	CAI_Bloodsucker	*pMonster;
	typedef	IState inherited;

	enum {
		ACTION_SIT,
		ACTION_WALK,
		ACTION_SLEEP,
		ACTION_WALK_CIRCUMSPECTION,
	} m_tAction;

	TTime			m_dwReplanTime;						//!< ����� �����, ������� ������ ������������
	TTime			m_dwLastPlanTime;					//!< ��������� ����� ������������

public:
					CBloodsuckerRest(CAI_Bloodsucker *p);
	virtual void	Reset			();
	virtual TTime	UnlockState		(TTime cur_time);
private:
	virtual	void	Init();
	virtual	void	Run();
			void	Replanning();
};

//////////////////////////////////////////////////////////////////////////
// State EAT
//////////////////////////////////////////////////////////////////////////
class CBloodsuckerEat : public IState {
	typedef	IState inherited;
	CAI_Bloodsucker	*pMonster;
	
	enum {
		ACTION_CORPSE_APPROACH_RUN,
		ACTION_CORPSE_APPROACH_WALK,
		ACTION_EAT
	} m_tAction;

	CEntity			*pCorpse;
	float			m_fDistToCorpse;			//!< ��������� �� �����
	float			m_fWalkDistToCorpse;		//!< ���������� �� �����, �� ������� ����� ���������� �� ������
	
	bool			bTastyCorpse;

	TTime			m_dwLastTimeEat;
	TTime			m_dwEatInterval;

public:
					CBloodsuckerEat	(CAI_Bloodsucker *p);
	virtual void	Reset			();
private:
	virtual	void	Init			();
	virtual	void	Run				();
	virtual void	Done			();
};


//////////////////////////////////////////////////////////////////////////
// State Attack
//////////////////////////////////////////////////////////////////////////
class CBloodsuckerAttack : public IState {
	typedef	IState inherited;
	CAI_Bloodsucker	*pMonster;
	
	enum {
		ACTION_RUN,
		ACTION_ATTACK_MELEE,
	} m_tAction;

	SEnemy			m_tEnemy;

	float			m_fDistMin;						//!< ���������� ���������� ���������� ��� ������
	float			m_fDistMax;						//!< ����������� ���������� ���������� ��� ������

	TTime			m_dwFaceEnemyLastTime;
	TTime			m_dwFaceEnemyLastTimeInterval;

public:
					CBloodsuckerAttack	(CAI_Bloodsucker *p);
	virtual void	Reset			();
private:
	virtual	void	Init			();
	virtual	void	Run				();
};

//////////////////////////////////////////////////////////////////////////
// State Hear DNE Sound
//////////////////////////////////////////////////////////////////////////
class CBloodsuckerHearDNE : public IState {
	typedef	IState inherited;
	CAI_Bloodsucker	*pMonster;

	enum {
		ACTION_LOOK_DESTINATION,
		ACTION_GOTO_SOUND_SOURCE,
	} m_tAction;

	SoundElem		m_tSound;
	bool			flag_once_1, flag_once_2;

	

public:
					CBloodsuckerHearDNE	(CAI_Bloodsucker *p);
	virtual void	Reset			();
private:
	virtual	void	Init			();
	virtual	void	Run				();

			void	Restart			();
};

//////////////////////////////////////////////////////////////////////////
// State Hear NDE Sound
//////////////////////////////////////////////////////////////////////////
class CBloodsuckerHearNDE : public IState {
	typedef	IState inherited;
	CAI_Bloodsucker	*pMonster;

	enum {
		ACTION_DO_NOTHING,
	} m_tAction;

public:
					CBloodsuckerHearNDE	(CAI_Bloodsucker *p);
	virtual void	Reset			();
private:
	virtual	void	Init			();
	virtual	void	Run				();
};

