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


const float m_cfBitingStandTurnRSpeed	=	PI_DIV_4;

const float m_cfBitingWalkSpeed			=	3.0f;
const float m_cfBitingWalkTurningSpeed	=	1.0f;
const float m_cfBitingWalkRSpeed		=	PI_DIV_4;		// когда SetDirLook
const float m_cfBitingWalkTurnRSpeed	=	PI_DIV_2;		// когда необходим поворот
const float m_cfBitingWalkMinAngle		=   PI_DIV_6;


const float m_cfBitingRunAttackSpeed		=	7.0f;
const float m_cfBitingRunAttackTurnSpeed	=	3.5f;
const float m_cfBitingRunAttackTurnRSpeed	=	5* PI_DIV_6;
const float m_cfBitingRunRSpeed				=	PI_DIV_2;
const float m_cfBitingRunAttackMinAngle		=   PI_DIV_6;
const float m_cfBitingAttackFastRSpeed		=	3*PI_DIV_4;

const float m_cfBitingAttackFastRSpeed2		=	PI;

const float m_cfBitingScaredRSpeed			=	3*PI_DIV_4;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingMotion class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingMotion {
public:
	CMotionParams		m_tParams;			//!< ќбщие параметры движени€ дл€ конкретного состо€ни€
	CMotionTurn			m_tTurn;			//!< ѕараметры движени€ в случае, если необходим поворот
	CMotionSequence		m_tSeq;				//!< ѕоследовательность действий дл€ специфического состо€ни€


	void Init();							//!< »ницализаци€ движени€
	void SetFrameParams(CAI_Biting *pData);	//!< ”становка параметров на текущий фрейм

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
	
	TTime			m_dwReplanTime;						//!< врем€ через, которое делать планирование
	TTime			m_dwLastPlanTime;					//!< последнее врем€ планировани€

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

	float			m_fDistMin;						//!< минимально допустимое рассто€ни€ дл€ аттаки
	float			m_fDistMax;						//!< максимально допустимое рассто€ние дл€ аттаки

	TTime			m_dwFaceEnemyLastTime;
	TTime			m_dwFaceEnemyLastTimeInterval;

	// регулировка приближением во врем€ атаки
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
	float			m_fDistToCorpse;			//!< дистанци€ до трупа

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
// CBitingHide class	// отход перебежками через укрыти€
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
// CBitingDetour class	// отход перебежками через укрыти€
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
// CBitingExploreDE class	// Explore danger-expedient enemy //  ѕосмотреть по сторонам, укрытьс€	
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
// CBitingExploreNDE class	// Explore non-danger enemy //  »дти в сторону звука	
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CFindEnemy // враг потер€н из вида - искать врага
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CFindEnemy : public IState {
	CAI_Biting		*pMonster;
	
	TTime			m_dwReplanTime;			//!< врем€ через, которое делать планирование
	TTime			m_dwLastPlanTime;		//!< последнее врем€ планировани€
	bool			bLookLeft;

	typedef IState inherited;
public:
					CFindEnemy 			(CAI_Biting *p);

	virtual void	Reset				();	

private:
	virtual void	Init				();
	virtual void	Run					();
};

