#pragma once
#include "ai_monster_defs.h"

class CCustomMonster;

enum EJumpStateType {
	JT_GLIDE,				
	JT_CUSTOM,
};

struct SJumpState{
	CMotionDef		*motion;
	EJumpStateType	type;
	bool			change;

	struct {
		float	linear;
		float	angular;
	} speed;
};


class CJumping {
	
	DEFINE_VECTOR(SJumpState, JUMP_STATE_VECTOR, JUMP_STATE_VECTOR_IT);

	CCustomMonster *pMonster;

	JUMP_STATE_VECTOR		bank;
	JUMP_STATE_VECTOR_IT	ptr_cur;
	CMotionDef				*cur_motion;

	bool			active;					// ��������� ������ �������?
	Fvector			from_pos;				// ��������� �������
	Fvector			target_pos;				// ������� �������
	float			target_yaw;				// ���� ������� �����������
	TTime			time_started;			// ����� ������ ������
	TTime			time_next_allowed;		// ����� ���������� ������������ ������
	float			ph_time;				// ���c������ ����� ������

	CObject			*entity;				// ��������� �� ������-���� (0 - ���)

	// LTX section params
	float			m_fJumpFactor;			// �����������-�������� ������� ������
	float			m_fJumpMinDist;			// ���. ���������, ��������� ��� ������
	float			m_fJumpMaxDist;			// ����. ��������� ��������� ��� ������
	float			m_fJumpMaxAngle;		// ����. ���� ��������� ��� ������ ����� �������� � �����
	TTime			m_dwDelayAfterJump;		// �������� ����� ������
	float			m_fTraceDist;			// 

public:
					CJumping				();
				
	// ������������� ���������� ������. ���������� �������� � Monster::Init
			void	Init					(CCustomMonster *pM); 
	// �������� ���������� �� ltx. ���������� �������� � Monster::Load
			void	Load					(LPCSTR section);
	
	// ���������� ���������� ������
			void	AddState				(CMotionDef *motion, EJumpStateType type, bool change, float linear, float angular);			
			
	// ����� �������� �� ������
			bool	Check					(Fvector from_pos, Fvector to_pos, CObject *pO);

	// ���������� ������� �������� ������
			bool	PrepareAnimation		(CMotionDef **m);
	
			void	OnAnimationEnd			();
	// ��������� ��������� ������ � ������ ������ (���������� �� UpdateCL)
			void	Update					();
			bool	IsActive				() {return active;}
	// ��������� �������� �� ����������� ������ � ������� ������ �������
	virtual bool	CanJump					() {return true;}

			bool	IsGlide					() {return ((active) ? JT_GLIDE == ptr_cur->type : false);}
			CObject	*GetEnemy				() {return entity;}

	virtual void	OnJumpStart				() {};
	virtual void	OnJumpStop				() {};

private:
			void	Reset					();
			// ������ ������
			void	Start					();
			void	Execute					();
			// ���������� ������			
			void	Stop					();

			void	ApplyParams				();
			// ����������� ��������� ��������� ������				
			void	NextState				();

};


