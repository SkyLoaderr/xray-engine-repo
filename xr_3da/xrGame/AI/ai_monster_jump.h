#pragma once

#include "ai_monster_defs.h"

class CCustomMonster;

// ����������� ���������� ������
#define JUMP_PREPARE_USED	(1<<0)
#define JUMP_FINISH_USED	(1<<1)

enum EJumpState {
	JS_PREPARE,
	JS_JUMP,
	JS_FINISH,
	JS_NONE
};

//struct SJump{
//	EMotionAnim	prepare;
//	EMotionAnim	jump;
//	EMotionAnim	finish;
//
//	float		trace_dist;
//	u8			states_used;
//};


class CJumping {
	CCustomMonster *pMonster;

	bool			active;					// ��������� ������ �������?
	EJumpState		state;					// ��������� ������
	Fvector			from_pos;				// ��������� �������
	Fvector			target_pos;				// ������� �������
	float			target_yaw;				// ���� ������� �����������
	TTime			time_started;			// ����� ������ ������
	TTime			time_next_allowed;		// ����� ���������� ������������ ������
	float			ph_time;				// ���c������ ����� ������
	u8				states_used;

	CObject			*entity;				// ��������� �� ������-���� (0 - ���)

	// LTX section
	float			m_fJumpFactor;			// �����������-�������� ������� ������
	float			m_fJumpMinDist;			// ���. ���������, ��������� ��� ������
	float			m_fJumpMaxDist;			// ����. ��������� ��������� ��� ������
	float			m_fJumpMaxAngle;		// ����. ���� ��������� ��� ������ ����� �������� � �����
	TTime			m_dwDelayAfterJump;		// �������� ����� ������

public:
					CJumping			();
				
	// ������������� ���������� ������. ���������� �������� � Monster::Init
			void	Init				(CCustomMonster *pM); 
	// �������� ���������� �� ltx. ���������� �������� � Monster::Load
			void	Load				(LPCSTR section);
			
			void	SetJump				();
			
	// ����� �������� �� ������
			bool	CheckJump			(Fvector from_pos, Fvector to_pos, CObject *pO);
	// ����������� ��������� ��������� ������	
			void	SwitchJumpState		();

	// ��������� �������� �� ����������� ������ � ������� ������ �������
	virtual bool	CanJump				() {return true;}
	
	virtual	void	PrepareJump			();
	virtual	void	ExecuteJump			();
	virtual	void	FinishJump			();

	//	// ��������� ��������� ������ � ������ ������ (���������� �� UpdateCL)
	//	void		JMP_Update				();


};

