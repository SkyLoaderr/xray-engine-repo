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
		fMinAngle = 0;						//!< если fMinAngle == 0, то поворот не нужен
	}
private:	
	bool NeedToTurn(CAI_Biting *pData);		//!< возвращает true, если поворот необходим, переменная bLeftSide - определяет сторону поворота

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMotionSequence class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class CMotionSequence {

	xr_vector<CMotionParams>::iterator it;		// указатель на текущий элемент
	xr_vector<CMotionParams> States;

public:	

	bool Playing;
	bool Started;								// true, если новое состояние готово к выполнению
	bool Finished;

public:

	// Инициализация всех полей
	void Init();
	// добавить 
	void Add(AI_Biting::EPostureAnim p, AI_Biting::EActionAnim a, float s, float r_s, float y, TTime t, u32 m);
	// Перейти в следующее состояние, если такового не имеется - завершить
	void Switch();
	// Выполняется в каждом фрейме
	void Cycle(u32 cur_time);
	
	void SetData(CAI_Biting *pData);

	void Finish();
	bool Active() {return (Playing || Started);}
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CBitingMotion class
///////////////////////////////////////////////////////////////////////////////////////////////////////////

class CBitingMotion {
public:
	CMotionParams		m_tParams;
	CMotionTurn			m_tTurn;
	CMotionSequence		m_tSeq;

	void Init() {
		m_tSeq.Init();
	}

	void SetFrameParams(CAI_Biting *pData) {
		if (!m_tSeq.Active()) {
			m_tParams.SetData(pData);
			m_tTurn.CheckTurning(pData);
		} else {
			m_tSeq.SetData(pData);
		}
	}
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


	TTime			m_dwStartTime;						//!< время перехода в это состояние
	TTime			m_dwCurrentTime;					//!< текущее время

public:
						IState			(CAI_Biting *p);

				void	Execute			(bool bNothingChanged = true);			//!< bNothingChanged - true, если не было измений после предыдущего вызова данного метода

	IC	virtual	bool	CheckCompletion	() {return false;}						//!< возвращает true, если задание выполнено
		virtual void	Reset			();										//!< инициализирует все значения по-умолчанию 

				void	Activate		() {m_tState = STATE_INIT;}				//!< Перейти в данное состояние
	IC			bool	Active			() {return (m_tState != STATE_NOT_ACTIVE);}

		virtual void	Init			();									//!< Стадия инициализации состояния
		virtual void	Run				();									//!< Стадия выполнения состояния
		virtual void	Done			();									//!< Стадия завершения выполнения состояния

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
	
	TTime			m_dwReplanTime;						//!< время через, которое делать планирование
	TTime			m_dwLastPlanTime;					//!< последнее время планирования

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

	float			m_fDistMin;						//!< минимально допустимое расстояния для аттаки
	float			m_fDistMax;						//!< максимально допустимое расстояние для аттаки

public:
	CAttack(CAI_Biting *p);

	void Reset();
	bool CheckCompletion();

private:
	void Init();
	void Run();
	void Replanning();
};
