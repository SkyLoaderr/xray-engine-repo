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

#define		DEFAULT_ANIM	eAnimStandIdle

#define		MASK_ANIM		(1 << 0)
#define		MASK_SPEED		(1 << 1)
#define		MASK_R_SPEED	(1 << 2)
#define		MASK_YAW		(1 << 3)

#define		STOP_ANIM_END	(1 << 0)
#define		STOP_TIME_OUT	(1 << 1)
#define		STOP_AT_TURNED	(1 << 2)


enum EMotionAnim {
	eAnimStandIdle = 0,
	eAnimStandTurnLeft,
	eAnimStandTurnRight,
	eAnimStandDamaged,
	
	eAnimSitIdle,
	eAnimLieIdle,

	eAnimSitToSleep,
	eAnimStandSitDown,
	eAnimStandLieDown,
	eAnimLieStandUp,
	eAnimSitStandUp,
	eAnimStandLieDownEat,

	eAnimWalkFwd,
	eAnimWalkBkwd,
	eAnimWalkTurnLeft,
	eAnimWalkTurnRight,
	
	eAnimRun,
	eAnimRunTurnLeft,
	eAnimRunTurnRight,
	eAnimFastTurn,					

	eAnimAttack,
	eAnimAttackRat,
	
	eAnimEat,
	eAnimSleep,
	eAnimDie,
	
	eAnimCheckCorpse,
	eAnimScared,
	eAnimAttackJump,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMotionSequence class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CMotionParams {

public:
	u32 stop_mask;
	u32 time;

	void SetParams(EMotionAnim a, float s, float r_s, float y, TTime t, u32 m, u32 s_m = 0) {
	}
	void ApplyData(CCustomMonster *p) {}
};
class CMotionTurn {
public:
	void Set(EMotionAnim l,EMotionAnim r, float f1, float f2, float f3) {};
	void Clear() {}
	void SetMoveBkwd(bool b) {};
};


class CMotionSequence {

	xr_vector<CMotionParams>::iterator it;		// индекс текущий элемент
	xr_vector<CMotionParams> States;

	CCustomMonster	*pMonster;

public:	

	bool Playing;

public:
	IC	void	Setup		(CCustomMonster *pM) {pMonster = pM;}

		// »нициализаци€ всех полей
		void	Init		();
		// добавить состо€ние
		void	Add			(EMotionAnim a, float s, float r_s, float y, TTime t, u32 m, u32 s_m = 0);
		// ѕерейти в следующее состо€ние, если такового не имеетс€ - завершить
		void	Switch		();
		// ¬ыполн€етс€ в каждом фрейме, выполн€ютс€ проверки на завершение тек. элемента последовательности 
		void	Cycle		(u32 cur_time);
		// ¬ыполн€етс€ по завершении анимации
		void	OnAnimEnd	();
		// ”становить параметры, определЄнные в текущем элементе последовательности
		void	ApplyData	();
		// «авершение последовательности
		void	Finish		();

	IC	bool	isActive	() {return Playing;}
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

