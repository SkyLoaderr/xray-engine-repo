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
// MOTION MANAGMENT
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************************

class CAI_Biting;

const float m_cfStandTurnRSpeed	=	PI_DIV_3;

const float m_cfWalkSpeed		 =	1.7f;
const float m_cfWalkTurningSpeed =	1.0f;
const float m_cfWalkRSpeed		=	PI_DIV_4;		// когда SetDirLook
const float m_cfWalkTurnRSpeed	=	PI_DIV_2;		// когда необходим поворот
const float m_cfWalkMinAngle	=   PI_DIV_6;


const float m_cfRunAttackSpeed		=	5.0f;
const float m_cfRunAttackTurnSpeed	=	3.5f;
const float m_cfRunAttackTurnRSpeed	=	5* PI_DIV_6;
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
	void ApplyData(CAI_Biting *pData);
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
	bool NeedToTurn(CAI_Biting *pData);		//!< возвращает true, если поворот необходим, переменна€ bLeftSide - определ€ет сторону поворота

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
	// добавить 
	void Add(AI_Biting::EPostureAnim p, AI_Biting::EActionAnim a, float s, float r_s, float y, TTime t, u32 m);
	// ѕерейти в следующее состо€ние, если такового не имеетс€ - завершить
	void Switch();
	// ¬ыполн€етс€ в каждом фрейме 
	void Cycle(u32 cur_time);
	
	void ApplyData(CAI_Biting *pData);

	void Finish();
	bool Active() {return (Playing || Started);}
};


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
	

protected:
	CAI_Biting		*pData;								


	TTime			m_dwStartTime;						//!< врем€ перехода в это состо€ние
	TTime			m_dwCurrentTime;					//!< текущее врем€
	TTime			m_dwNextThink;						//!< врем€ следующего выполнени€ состо€ни€

public:
						IState			(CAI_Biting *p);

				void	Execute			(bool bNothingChanged = true);			//!< bNothingChanged - true, если не было измений после предыдущего вызова данного метода

	IC	virtual	bool	CheckCompletion	() {return false;}						//!< возвращает true, если задание выполнено
		virtual void	Reset			();										//!< инициализирует все значени€ по-умолчанию 

				void	Activate		() {m_tState = STATE_INIT;}				//!< ѕерейти в данное состо€ние
	IC			bool	Active			() {return (m_tState != STATE_NOT_ACTIVE);}

		virtual void	Init			();									//!< —тади€ инициализации состо€ни€
		virtual void	Run				();									//!< —тади€ выполнени€ состо€ни€
		virtual void	Done			();									//!< —тади€ завершени€ выполнени€ состо€ни€

				void	SetNextThink	(TTime next_think) {m_dwNextThink = next_think + m_dwCurrentTime;}
		
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
	
	TTime			m_dwReplanTime;						//!< врем€ через, которое делать планирование
	TTime			m_dwLastPlanTime;					//!< последнее врем€ планировани€

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

	float			m_fDistMin;						//!< минимально допустимое рассто€ни€ дл€ аттаки
	float			m_fDistMax;						//!< максимально допустимое рассто€ние дл€ аттаки

public:
	CAttack(CAI_Biting *p);

	void Reset();
	bool CheckCompletion();

private:
	void Init();
	void Run();
	void Replanning();
};
