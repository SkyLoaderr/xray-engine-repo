////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monster_state.h
//	Created 	: 06.07.2003
//  Modified 	: 06.07.2003
//	Author		: Serge Zhem
//	Description : FSM states and motion
////////////////////////////////////////////////////////////////////////////

#pragma once 

#include "ai_monster_mem.h"

//*********************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// MOTION MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************

class CCustomMonster;

#define		DEFAULT_ANIM	eMotionStandIdle

#define		MASK_ANIM		(1 << 0)
#define		MASK_SPEED		(1 << 1)
#define		MASK_R_SPEED	(1 << 2)
#define		MASK_YAW		(1 << 3)
#define		MASK_TIME		(1 << 4)


enum EMotionAnim {
	eMotionStandIdle = 0,
	eMotionLieIdle,
	eMotionStandTurnLeft,
	eMotionWalkFwd,
	eMotionWalkBkwd,
	eMotionWalkTurnLeft,
	eMotionWalkTurnRight,
	eMotionRun,
	eMotionRunTurnLeft,
	eMotionRunTurnRight,
	eMotionAttack,
	eMotionAttackRat,
	eMotionFastTurnLeft,
	eMotionEat,
	eMotionStandDamaged,
	eMotionScared,
	eMotionDie,
	eMotionLieDown,
	eMotionStandUp,
	eMotionCheckCorpse,
	eMotionLieDownEat,
	eMotionAttackJump
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMotionParams class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CMotionParams{
public:
	EMotionAnim					anim;

	float						speed;
	float						r_speed;
	float						yaw;
	TTime						time;

	u32							mask;
public:	
	void SetParams(EMotionAnim a, float s, float r_s, float y, TTime t, u32 m);
	void ApplyData(CCustomMonster *pData);
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMotionTurn class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CMotionTurn {
	CMotionParams TurnLeft;
	CMotionParams TurnRight;

	float fMinAngle;
	bool  bLeftSide;
public:
	void Set(EMotionAnim a_left, EMotionAnim a_right, float s, float r_s, float min_angle);

	bool CheckTurning(CCustomMonster *pData);
	void Clear() {
		fMinAngle = 0;						//!< если fMinAngle == 0, то поворот не нужен
	}
private:	
	bool NeedToTurn(CCustomMonster *pData);		//!< возвращает true, если поворот необходим, переменна€ bLeftSide - определ€ет сторону поворота

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMotionSequence class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CMotionSequence {

	xr_vector<CMotionParams>::iterator it;		// указатель на текущий элемент
	xr_vector<CMotionParams> States;

public:	

	bool Playing;
	bool Started;								// true, если новое состо€ние готово к выполнению
	bool Finished;

public:

	// »нициализаци€ всех полей
	void Init();
	// добавить состо€ние
	void Add(EMotionAnim a, float s, float r_s, float y, TTime t, u32 m);
	// ѕерейти в следующее состо€ние, если такового не имеетс€ - завершить
	void Switch();
	// ¬ыполн€етс€ в каждом фрейме 
	void Cycle(u32 cur_time);

	void ApplyData(CCustomMonster *pData);

	void Finish();
	bool Active() {return (Playing || Started);}
};



//*********************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATE MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************



///////////////////////////////////////////////////////////////////////////////////////////////////////////
// IState class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class IState {

	enum {
		STATE_NOT_ACTIVE,
		STATE_INIT,
		STATE_RUN,
		STATE_DONE
	} m_tState;											

	enum EPriority {
		PRIORITY_NONE,
		PRIORITY_LOW,
		PRIORITY_NORMAL,
		PRIORITY_HIGH
	} m_tPriority;										//!< »нерционный приоритет


protected:
	TTime			m_dwCurrentTime;					//!< текущее врем€
	TTime			m_dwStateStartedTime;				//!< врем€ перехода в это состо€ние
	TTime			m_dwNextThink;						//!< врем€ следующего выполнени€ состо€ни€
	TTime			m_dwTimeLocked;						//!< врем€ заблокировки состо€ни€

	bool			m_bLocked;							//!< —осто€ние заблокировано

	TTime			m_dwInertia;						//!< »нерци€

public:
						IState			();

				void	Execute			(TTime cur_time);						//!< bNothingChanged - true, если не было измений после предыдущего вызова данного метода

	IC	virtual	bool	CheckCompletion	() {return false;}						//!< возвращает true, если задание выполнено
		virtual void	Reset			();										//!< инициализирует все значени€ по-умолчанию 

	IC			void	Activate		() {m_tState = STATE_INIT;}				//!< ѕерейти в данное состо€ние
	IC			bool	Active			() {return (m_tState != STATE_NOT_ACTIVE);}

		virtual void	Init			();									//!< —тади€ инициализации состо€ни€
		virtual void	Run				();									//!< —тади€ выполнени€ состо€ни€
		virtual void	Done			();									//!< —тади€ завершени€ выполнени€ состо€ни€

	IC			void	SetNextThink	(TTime next_think) {m_dwNextThink = next_think + m_dwCurrentTime;}


	/* сохранение и восстановление времени при активации критических состо€ний (kinda StandUp) */
				void	LockState		();				
		virtual	TTime	UnlockState		(TTime cur_time);					//!< возвращает dt=(текущее врем€ - сохранЄнное)
	IC			bool	IsLocked		() {return m_bLocked;}

	/* определение приоритета состо€ни€ (реализаци€ инерции) */ 
	IC			void	SetLowPriority	() {m_tPriority = PRIORITY_LOW;}
	IC			void	SetNormalPriority() {m_tPriority = PRIORITY_NORMAL;}
	IC			void	SetHighPriority	() {m_tPriority = PRIORITY_HIGH;}
	IC			void	SetInertia		(TTime inertia) {m_dwInertia = inertia + m_dwCurrentTime;}
	IC			bool 	IsInertia		() {return ((m_tPriority != PRIORITY_NONE) && (m_dwInertia > m_dwCurrentTime));}
	IC		EPriority	GetPriority		() {return m_tPriority;}
};

