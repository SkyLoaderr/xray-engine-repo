////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_chimera_state.h
//	Created 	: 27.06.2003
//  Modified 	: 27.06.2003
//	Author		: Serge Zhem
//	Description : FSM states
////////////////////////////////////////////////////////////////////////////

#pragma once 

#include "..\\ai_monster_state.h"

class CAI_Chimera;

const float m_cfChimeraStandTurnRSpeed	=	PI_DIV_3;

const float m_cfChimeraWalkSpeed		=	1.7f;
const float m_cfChimeraWalkTurningSpeed =	1.0f;
const float m_cfChimeraWalkRSpeed		=	PI_DIV_4;		// когда SetDirLook
const float m_cfChimeraWalkTurnRSpeed	=	PI_DIV_2;		// когда необходим поворот
const float m_cfChimeraWalkMinAngle		=   PI_DIV_6;


const float m_cfChimeraRunAttackSpeed		=	7.0f;
const float m_cfChimeraRunAttackTurnSpeed	=	3.5f;
const float m_cfChimeraRunAttackTurnRSpeed	=	5* PI_DIV_6;
const float m_cfChimeraRunRSpeed			=	PI_DIV_2;
const float m_cfChimeraRunAttackMinAngle	=   PI_DIV_6;
const float m_cfChimeraAttackFastRSpeed		=	3*PI_DIV_4;

const float m_cfChimeraScaredRSpeed			=	3*PI_DIV_4;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraMotion class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CChimeraMotion {
public:
	CMotionParams		m_tParams;			//!< Общие параметры движения для конкретного состояния
	CMotionTurn			m_tTurn;			//!< Параметры движения в случае, если необходим поворот
	CMotionSequence		m_tSeq;				//!< Последовательность действий для специфического состояния


	void Init();							//!< Иницализация движения
	void SetFrameParams(CAI_Chimera *pData);	//!< Установка параметров на текущий фрейм

};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraRest class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CChimeraRest : public IState {
	CAI_Chimera	*pMonster;
	
	enum {
			ACTION_WALK,
			ACTION_STAND,
			ACTION_LIE,
			ACTION_TURN,
	} m_tAction;
	
	TTime			m_dwReplanTime;						//!< время через, которое делать планирование
	TTime			m_dwLastPlanTime;					//!< последнее время планирования

	typedef IState inherited;

public:
	CChimeraRest(CAI_Chimera *p);

	virtual void Reset();

	virtual TTime UnlockState(TTime cur_time);
private:
	virtual void Run();
	void Replanning();
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraAttack class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CChimeraAttack : public IState {
	
	CAI_Chimera	*pMonster;

	enum {
		ACTION_RUN,
		ACTION_ATTACK_MELEE,
	} m_tAction;

	CEntity			*pEnemy;
	bool			m_bAttackRat;

	float			m_fDistMin;						//!< минимально допустимое расстояния для аттаки
	float			m_fDistMax;						//!< максимально допустимое расстояние для аттаки

	TTime			m_dwFaceEnemyLastTime;
	TTime			m_dwFaceEnemyLastTimeInterval;

	u32				nStartStop;						
	u32				nDoDamage;						//!< Количество нанесённых повреждений

public:
	CChimeraAttack(CAI_Chimera *p);

	void Reset();
	bool CheckCompletion();

private:
	void Init();
	void Run();
	void Replanning();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraEat class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CChimeraEat : public IState {
	
	CAI_Chimera	*pMonster;

	enum {
		ACTION_RUN,
		ACTION_EAT,
	} m_tAction;

	CEntity			*pCorpse;
	float			m_fDistToCorpse;			//!< дистанция до трупа

	TTime			m_dwLastTimeEat;
	TTime			m_dwEatInterval;

public:
	CChimeraEat(CAI_Chimera *p);

	void Reset();
	bool CheckCompletion();

private:
	void Init();
	void Run();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraHide class	// отход перебежками через укрытия
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CChimeraHide : public IState {
	CAI_Chimera		*pMonster;

	VisionElem		m_tEnemy;

	typedef IState inherited;
public:
					CChimeraHide			(CAI_Chimera *p);

	virtual	bool	CheckCompletion	();
	virtual void	Reset			();	
	
private:

	virtual void	Init			();
	virtual void	Run				();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraDetour class	// отход перебежками через укрытия
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CChimeraDetour : public IState {
	CAI_Chimera		*pMonster;

	VisionElem		m_tEnemy;

	typedef IState inherited;
public:
					CChimeraDetour			(CAI_Chimera *p);

	virtual void	Reset			();	
	virtual	bool	CheckCompletion	();

private:
	virtual void	Init			();
	virtual void	Run				();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraPanic class	// убегать от противника
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CChimeraPanic : public IState {
	CAI_Chimera		*pMonster;

	VisionElem		m_tEnemy;

	typedef IState inherited;
public:
					CChimeraPanic			(CAI_Chimera *p);

	virtual void	Reset			();	
	virtual	bool	CheckCompletion	();

private:
	virtual void	Init			();
	virtual void	Run				();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraExploreDNE class	// Explore danger-non-expedient enemy
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CChimeraExploreDNE : public IState {
	CAI_Chimera		*pMonster;
	VisionElem		m_tEnemy;

	typedef IState inherited;
public:
					CChimeraExploreDNE	(CAI_Chimera *p);

	virtual void	Reset				();	

private:
	virtual void	Init				();
	virtual void	Run					();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraExploreDE class	// Explore danger-expedient enemy //  Посмотреть по сторонам, укрыться	
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CChimeraExploreDE : public IState {
	CAI_Chimera		*pMonster;
	VisionElem		m_tEnemy;

	enum {
		ACTION_LOOK_AROUND,
		ACTION_HIDE,
	} m_tAction;


	typedef IState inherited;
public:
					CChimeraExploreDE	(CAI_Chimera *p);

	virtual void	Reset				();	

private:
	virtual void	Init				();
	virtual void	Run					();
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CChimeraExploreNDE class	// Explore non-danger enemy //  Идти в сторону звука	
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CChimeraExploreNDE : public IState {
	CAI_Chimera		*pMonster;
	VisionElem		m_tEnemy;

	typedef IState inherited;
public:
					CChimeraExploreNDE	(CAI_Chimera *p);

	virtual void	Reset				();	

private:
	virtual void	Init				();
	virtual void	Run					();
};
