////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_state.h
//	Created 	: 27.06.2003
//  Modified 	: 27.06.2003
//	Author		: Serge Zhem
//	Description : FSM states
////////////////////////////////////////////////////////////////////////////

#pragma once 

//*********************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATE MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************

class CAI_Biting;

const float m_cfWalkSpeed		=	2.0;
const float m_cfWalkRSpeed		=	PI_DIV_4;
const float m_cfWalkTurnRSpeed	=	PI_DIV_2;
const float m_cfWalkMinAngle	=   PI_DIV_6;
const float m_cfStandTurnRSpeed	=	PI_DIV_2;

const float m_cfRunAttackSpeed		=	4.0;
const float m_cfRunAttackTurnSpeed	=	4.0;
const float m_cfRunAttackTurnRSpeed	=	4.0;
const float m_cfRunRSpeed			=	PI_DIV_2;
const float m_cfRunAttackMinAngle	=   PI_DIV_6;



#define		MASK_ANIM		(1 << 0)
#define		MASK_SPEED		(1 << 1)
#define		MASK_R_SPEED	(1 << 2)
#define		MASK_YAW		(1 << 3)
#define		MASK_TIME		(1 << 4)


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMotionParams class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CMotionParams{
public:
	AI_Biting::EActionAnim		action;
	AI_Biting::EPostureAnim		posture;

	float						speed;
	float						r_speed;
	float						yaw;
	TTime						time;

	u32							mask;
public:	
	void SetParams(AI_Biting::EPostureAnim p, AI_Biting::EActionAnim a, float s, float r_s, float y, TTime t, u32 m);
	void SetData(CAI_Biting *pData);
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
	void Set(AI_Biting::EPostureAnim p_left, AI_Biting::EActionAnim a_left, 
			 AI_Biting::EPostureAnim p_right, AI_Biting::EActionAnim a_right,
		 	 float s, float r_s, float min_angle);

	bool CheckTurning(CAI_Biting *pData);
	void Clear() {
		fMinAngle = 0;						//!< ���� fMinAngle == 0, �� ������� �� �����
	}
private:	
	bool NeedToTurn(CAI_Biting *pData);		//!< ���������� true, ���� ������� ���������, ���������� bLeftSide - ���������� ������� ��������

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingMotion class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingMotion {
public:
	CMotionParams	m_tParams;
	CMotionTurn		m_tTurn;


	void SetFrameParams(CAI_Biting *pData) {
		m_tParams.SetData(pData);
		m_tTurn.CheckTurning(pData);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMotionSequence class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CMotionSequence {

	xr_vector<CMotionParams>::iterator it;		// ��������� �� ������� �������
	xr_vector<CMotionParams> States;

public:	

	bool Playing;
	bool Started;								// true, ���� ����� ��������� ������ � ����������
	bool Finished;

public:
	
	// ������������� ���� �����
	void Init();
	// �������� 
	void Add(AI_Biting::EPostureAnim p, AI_Biting::EActionAnim a, float s, float r_s, float y, TTime t, u32 m);
	// ������� � ��������� ���������, ���� �������� �� ������� - ���������
	void Switch(CAI_Biting *pData);
	// ����������� � ������ ������
	void Cycle(u32 cur_time, CAI_Biting *pData);

	void Finish();
	bool Active() {return (Playing || Started);}
};



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
	

protected:
	CAI_Biting		*pData;								


	TTime			m_dwStartTime;						//!< ����� �������� � ��� ���������
	TTime			m_dwCurrentTime;					//!< ������� �����

public:
						IState			(CAI_Biting *p);

				void	Execute			(bool bNothingChanged = true);			//!< bNothingChanged - true, ���� �� ���� ������� ����� ����������� ������ ������� ������

	IC	virtual	bool	CheckCompletion	() {return false;}						//!< ���������� true, ���� ������� ���������
		virtual void	Reset			();										//!< �������������� ��� �������� ��-��������� 

				void	Activate		() {m_tState = STATE_INIT;}				//!< ������� � ������ ���������
	IC			bool	Active			() {return (m_tState != STATE_NOT_ACTIVE);}

		virtual void	Init			();									//!< ������ ������������� ���������
		virtual void	Run				();									//!< ������ ���������� ���������
		virtual void	Done			();									//!< ������ ���������� ���������� ���������

};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CRest class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CRest : public IState {

	enum {
			ACTION_WALK,
			ACTION_STAND,
			ACTION_LIE,
			ACTION_TURN,
	} m_tAction;
	
	TTime			m_dwReplanTime;						//!< ����� �����, ������� ������ ������������
	TTime			m_dwLastPlanTime;					//!< ��������� ����� ������������

public:
	CRest(CAI_Biting *p);

	void Reset();

private:
	void Run();
	void Replanning();
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAttack class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CAttack : public IState {
	enum {
		ACTION_RUN,
		ACTION_ATTACK_MELEE,
	} m_tAction;

	CEntity			*pEnemy;
	bool			m_bAttackRat;

	float			m_fDistMin;						//!< ���������� ���������� ���������� ��� ������
	float			m_fDistMax;						//!< ����������� ���������� ���������� ��� ������

public:
	CAttack(CAI_Biting *p);

	void Reset();
	bool CheckCompletion();

private:
	void Init();
	void Run();
	void Replanning();
};
