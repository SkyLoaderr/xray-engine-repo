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


#define DO_ONCE_BEGIN(flag)	if (!flag) {flag = true;  
#define DO_ONCE_END()		}

#define DO_IN_TIME_INTERVAL_BEGIN(varLastTime, varTimeInterval)	if (varLastTime + varTimeInterval < m_dwCurrentTime) { varLastTime = m_dwCurrentTime;
#define DO_IN_TIME_INTERVAL_END()								}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingMotion class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingMotion {
public:
	CMotionParams		m_tParams;			//!< ����� ��������� �������� ��� ����������� ���������
	CMotionTurn			m_tTurn;			//!< ��������� �������� � ������, ���� ��������� �������
	CMotionSequence		m_tSeq;				//!< ������������������ �������� ��� �������������� ���������


	void Init(CAI_Biting *pData);			//!< ������������ ��������
	void SetFrameParams(CAI_Biting *pData);	//!< ��������� ���������� �� ������� �����

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

	float			m_fDistMin;						//!< ���������� ���������� ���������� ��� ������
	float			m_fDistMax;						//!< ����������� ���������� ���������� ��� ������

	TTime			m_dwFaceEnemyLastTime;
	TTime			m_dwFaceEnemyLastTimeInterval;

	TTime			m_dwSuperMeleeStarted;

	typedef	IState inherited;
public:
					CBitingAttack	(CAI_Biting *p);

	virtual	void	Reset			();

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
		ACTION_DRAG,
		ACTION_EAT,
	} m_tAction;

	CEntity			*pCorpse;
	float			m_fDistToCorpse;			//!< ��������� �� �����

	TTime			m_dwLastTimeEat;
	TTime			m_dwEatInterval;

	Fvector			SavedPos;					// ���������� ������� �� �����
	float			m_fDistToDrag;				// �� ����� ���������� ������
	bool			bDragging;

public:
					CBitingEat		(CAI_Biting *p);

	virtual	void	Reset			();
	virtual bool	CheckCompletion	();

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

