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

enum EJumpType {
	JT_ATTACK_JUMP,
	JT_JUMP_OVER
};


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
				
	// call this function from Monster::Init
			void	Init				(CCustomMonster *pM); 
	// call this function from Monster::Load
			void	Load				(LPCSTR section);
	// common check
			bool	CheckJump			(Fvector from_pos, Fvector to_pos, CObject *pO);
	// switch to new jump state	
			void	SwitchJumpState		();

	// ��������� �������� �� ����������� ������ � ������� ������ �������
	virtual bool	CanJump				() {return true;}
	// monster can define its own functions	
	virtual	void	PrepareJump			();
	virtual	void	ExecuteJump			();
	virtual	void	FinishJump			();
};

