////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monster_state.h
//	Created 	: 06.07.2003
//  Modified 	: 01.10.2003
//	Author		: Serge Zhem
//	Description : interface for monster states
////////////////////////////////////////////////////////////////////////////

#pragma once 

#include "ai_monster_mem.h"

#define DO_ONCE_BEGIN(flag)	if (!flag) {flag = true;  
#define DO_ONCE_END()		}

#define DO_IN_TIME_INTERVAL_BEGIN(varLastTime, varTimeInterval)	if (varLastTime + varTimeInterval < m_dwCurrentTime) { varLastTime = m_dwCurrentTime;
#define DO_IN_TIME_INTERVAL_END()								}

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


	/* определение приоритета состо€ни€ (реализаци€ инерции) */ 
	IC			void	SetLowPriority	() {m_tPriority = PRIORITY_LOW;}
	IC			void	SetNormalPriority() {m_tPriority = PRIORITY_NORMAL;}
	IC			void	SetHighPriority	() {m_tPriority = PRIORITY_HIGH;}
	IC			void	SetInertia		(TTime inertia) {m_dwInertia = inertia + m_dwCurrentTime;}
	IC			bool 	IsInertia		() {return ((m_tPriority != PRIORITY_NONE) && (m_dwInertia > m_dwCurrentTime));}
	IC		EPriority	GetPriority		() {return m_tPriority;}
};

