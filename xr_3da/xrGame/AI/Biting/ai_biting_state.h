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


const float m_cfBitingStandTurnRSpeed	=	PI_DIV_3;

const float m_cfBitingWalkSpeed		 =	1.7f;
const float m_cfBitingWalkTurningSpeed =	1.0f;
const float m_cfBitingWalkRSpeed		=	PI_DIV_4;		// ����� SetDirLook
const float m_cfBitingWalkTurnRSpeed	=	PI_DIV_2;		// ����� ��������� �������
const float m_cfBitingWalkMinAngle	=   PI_DIV_6;


const float m_cfBitingRunAttackSpeed		=	5.0f;
const float m_cfBitingRunAttackTurnSpeed	=	3.5f;
const float m_cfBitingRunAttackTurnRSpeed	=	5* PI_DIV_6;
const float m_cfBitingRunRSpeed			=	PI_DIV_2;
const float m_cfBitingRunAttackMinAngle	=   PI_DIV_6;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingMotion class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingMotion {
public:
	CMotionParams		m_tParams;			//!< ����� ��������� �������� ��� ����������� ���������
	CMotionTurn			m_tTurn;			//!< ��������� �������� � ������, ���� ��������� �������
	CMotionSequence		m_tSeq;				//!< ������������������ �������� ��� �������������� ���������


	void Init();							//!< ������������ ��������
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
	} m_tAction;
	
	TTime			m_dwReplanTime;						//!< ����� �����, ������� ������ ������������
	TTime			m_dwLastPlanTime;					//!< ��������� ����� ������������

	typedef IState inherited;

public:
	CBitingRest(CAI_Biting *p);

	virtual void Reset();

	virtual TTime UnlockState(TTime cur_time);
private:
	virtual void Run();
	void Replanning();
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingAttack class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingAttack : public IState {
	
	CAI_Biting	*pMonster;

	enum {
		ACTION_RUN,
		ACTION_ATTACK_MELEE,
	} m_tAction;

	CEntity			*pEnemy;
	bool			m_bAttackRat;

	float			m_fDistMin;						//!< ���������� ���������� ���������� ��� ������
	float			m_fDistMax;						//!< ����������� ���������� ���������� ��� ������

public:
	CBitingAttack(CAI_Biting *p);

	void Reset();
	bool CheckCompletion();

private:
	void Init();
	void Run();
	void Replanning();
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
	float			m_fDistToCorpse;			//!< ��������� �� �����

	TTime			m_dwLastTimeEat;
	TTime			m_dwEatInterval;

public:
	CBitingEat(CAI_Biting *p);

	void Reset();
	bool CheckCompletion();

private:
	void Init();
	void Run();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingHide class	// ����� ����������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBitingHide : public IState {
	CAI_Biting		*pMonster;

	VisionElem		m_tEnemy;

	TTime			m_dwReplanTime;						//!< ����� �����, ������� ������ ������������
	TTime			m_dwLastPlanTime;					//!< ��������� ����� ������������

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
// CBitingDetour class	// ����� ����������� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CBitingDetour : public IState {
	CAI_Biting		*pMonster;

	VisionElem		m_tEnemy;

	typedef IState inherited;
public:
					CBitingDetour			(CAI_Biting *p);

	virtual void	Reset			();	
	virtual	bool	CheckCompletion	();

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

	typedef IState inherited;
public:
					CBitingPanic			(CAI_Biting *p);

	virtual void	Reset			();	
	virtual	bool	CheckCompletion	();

private:
	virtual void	Init			();
	virtual void	Run				();
};
