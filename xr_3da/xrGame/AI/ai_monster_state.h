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
	u32							stop_mask;

public:	
	void SetParams(EMotionAnim a, float s, float r_s, float y, TTime t, u32 m, u32 s_m = 0);
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
	bool  bMoveBkwd;
public:
	void Set(EMotionAnim a_left, EMotionAnim a_right, float s, float r_s, float min_angle);
	
	bool CheckTurning(CCustomMonster *pData);
	void Clear() {
		fMinAngle = 0;						//!< ���� fMinAngle == 0, �� ������� �� �����
	}
	void SetMoveBkwd(bool dir_bkwd) {bMoveBkwd = dir_bkwd;}
private:	
	bool NeedToTurn(CCustomMonster *pData);		//!< ���������� true, ���� ������� ���������, ���������� bLeftSide - ���������� ������� ��������

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMotionSequence class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CMotionSequence {

	xr_vector<CMotionParams>::iterator it;		// ������ ������� �������
	xr_vector<CMotionParams> States;

	CCustomMonster	*pMonster;

public:	

	bool Playing;

public:
	IC	void	Setup		(CCustomMonster *pM) {pMonster = pM;}

		// ������������� ���� �����
		void	Init		();
		// �������� ���������
		void	Add			(EMotionAnim a, float s, float r_s, float y, TTime t, u32 m, u32 s_m = 0);
		// ������� � ��������� ���������, ���� �������� �� ������� - ���������
		void	Switch		();
		// ����������� � ������ ������, ����������� �������� �� ���������� ���. �������� ������������������ 
		void	Cycle		(u32 cur_time);
		// ����������� �� ���������� ��������
		void	OnAnimEnd	();
		// ���������� ���������, ����������� � ������� �������� ������������������
		void	ApplyData	();
		// ���������� ������������������
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
	} m_tPriority;										//!< ����������� ���������


protected:
	TTime			m_dwCurrentTime;					//!< ������� �����
	TTime			m_dwStateStartedTime;				//!< ����� �������� � ��� ���������
	TTime			m_dwNextThink;						//!< ����� ���������� ���������� ���������
	TTime			m_dwTimeLocked;						//!< ����� ������������ ���������

	bool			m_bLocked;							//!< ��������� �������������

	TTime			m_dwInertia;						//!< �������
	
public:
						IState			();

				void	Execute			(TTime cur_time);						//!< bNothingChanged - true, ���� �� ���� ������� ����� ����������� ������ ������� ������

	IC	virtual	bool	CheckCompletion	() {return false;}						//!< ���������� true, ���� ������� ���������
		virtual void	Reset			();										//!< �������������� ��� �������� ��-��������� 

	IC			void	Activate		() {m_tState = STATE_INIT;}				//!< ������� � ������ ���������
	IC			bool	Active			() {return (m_tState != STATE_NOT_ACTIVE);}

		virtual void	Init			();									//!< ������ ������������� ���������
		virtual void	Run				();									//!< ������ ���������� ���������
		virtual void	Done			();									//!< ������ ���������� ���������� ���������

	IC			void	SetNextThink	(TTime next_think) {m_dwNextThink = next_think + m_dwCurrentTime;}


	/* ���������� � �������������� ������� ��� ��������� ����������� ��������� (kinda StandUp) */
				void	LockState		();				
		virtual	TTime	UnlockState		(TTime cur_time);					//!< ���������� dt=(������� ����� - ����������)
	IC			bool	IsLocked		() {return m_bLocked;}

	/* ����������� ���������� ��������� (���������� �������) */ 
	IC			void	SetLowPriority	() {m_tPriority = PRIORITY_LOW;}
	IC			void	SetNormalPriority() {m_tPriority = PRIORITY_NORMAL;}
	IC			void	SetHighPriority	() {m_tPriority = PRIORITY_HIGH;}
	IC			void	SetInertia		(TTime inertia) {m_dwInertia = inertia + m_dwCurrentTime;}
	IC			bool 	IsInertia		() {return ((m_tPriority != PRIORITY_NONE) && (m_dwInertia > m_dwCurrentTime));}
	IC		EPriority	GetPriority		() {return m_tPriority;}
};

