////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_state.h
//	Created 	: 27.06.2003
//  Modified 	: 27.06.2003
//	Author		: Serge Zhem
//	Description : FSM states
////////////////////////////////////////////////////////////////////////////

#pragma once 

#include "..\\ai_monster_state.h"

class CAI_Biting;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingMotion class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingMotion {
public:
	CMotionParams		m_tParams;			//!< Общие параметры движения для конкретного состояния
	CMotionTurn			m_tTurn;			//!< Параметры движения в случае, если необходим поворот
	CMotionSequence		m_tSeq;				//!< Последовательность действий для специфического состояния


	void Init();							//!< Иницализация движения
	void SetFrameParams(CAI_Biting *pData);	//!< Установка параметров на текущий фрейм

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingRest class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingRest : public IState {
	CAI_Biting	*pMonster;
	
	enum {
			ACTION_WALK,
			ACTION_STAND,
			ACTION_LIE,
			ACTION_TURN,
			ACTION_WALK_GRAPH_END
	} m_tAction;
	
	TTime			m_dwReplanTime;						//!< время через, которое делать планирование
	TTime			m_dwLastPlanTime;					//!< последнее время планирования

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
// CBitingAttack class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingAttack : public IState {
	
	CAI_Biting	*pMonster;

	enum {
		ACTION_RUN,
		ACTION_ATTACK_MELEE,
		ACTION_FIND_ENEMY
	} m_tAction;

	VisionElem		m_tEnemy;

	bool			m_bAttackRat;

	float			m_fDistMin;						//!< минимально допустимое расстояния для аттаки
	float			m_fDistMax;						//!< максимально допустимое расстояние для аттаки

	TTime			m_dwFaceEnemyLastTime;
	TTime			m_dwFaceEnemyLastTimeInterval;

	// регулировка приближением во время атаки
	u32				nStartStop;						//!< количество	старт-стопов

	TTime			m_dwSuperMeleeStarted;

	typedef	IState inherited;
public:
					CBitingAttack	(CAI_Biting *p);

	virtual	void	Reset			();
	virtual bool	CheckCompletion	();

private:
	virtual	void	Init			();
	virtual void	Run				();

			
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingEat class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingEat : public IState {
	
	CAI_Biting	*pMonster;

	enum {
		ACTION_RUN,
		ACTION_EAT,
	} m_tAction;

	CEntity			*pCorpse;
	float			m_fDistToCorpse;			//!< дистанция до трупа

	TTime			m_dwLastTimeEat;
	TTime			m_dwEatInterval;

public:
					CBitingEat		(CAI_Biting *p);

	virtual	void	Reset			();
	virtual bool	CheckCompletion	();

private:
	virtual void	Init			();
	virtual void	Run				();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingHide class	// отход перебежками через укрытия
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBitingHide : public IState {
	CAI_Biting		*pMonster;

	VisionElem		m_tEnemy;

	typedef IState inherited;
public:
					CBitingHide			(CAI_Biting *p);

	virtual	bool	CheckCompletion	();
	virtual void	Reset			();	
	
private:

	virtual void	Init			();
	virtual void	Run				();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingDetour class	// отход перебежками через укрытия
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
// CBitingPanic class	// убегать от противника
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBitingPanic : public IState {
	CAI_Biting		*pMonster;

	VisionElem		m_tEnemy;

	// implementation of 'face the most open area'
	bool			bFacedOpenArea;
	Fvector			cur_pos;			
	Fvector			prev_pos;
	TTime			m_dwStayTime;


	typedef IState inherited;
public:
					CBitingPanic			(CAI_Biting *p);

	virtual void	Reset			();	

private:
	virtual void	Init			();
	virtual void	Run				();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreDNE class	// Explore danger-non-expedient enemy
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBitingExploreDNE : public IState {
	CAI_Biting		*pMonster;
	VisionElem		m_tEnemy;

	// implementation of 'face the most open area'
	bool			bFacedOpenArea;
	Fvector			cur_pos;			
	Fvector			prev_pos;
	TTime			m_dwStayTime;


	typedef IState inherited;
public:
					CBitingExploreDNE	(CAI_Biting *p);

	virtual void	Reset				();	

private:
	virtual void	Init				();
	virtual void	Run					();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingExploreDE class	// Explore danger-expedient enemy //  Посмотреть по сторонам, укрыться	
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
// CBitingExploreNDE class	// Explore non-danger enemy //  Идти в сторону звука	
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBitingExploreNDE : public IState {
	CAI_Biting		*pMonster;
	VisionElem		m_tEnemy;

	typedef IState inherited;
public:
					CBitingExploreNDE	(CAI_Biting *p);

	virtual void	Reset				();	

private:
	virtual void	Init				();
	virtual void	Run					();
};
