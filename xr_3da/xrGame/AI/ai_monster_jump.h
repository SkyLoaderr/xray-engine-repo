#pragma once

#include "ai_monster_defs.h"

class CCustomMonster;

// Определение параметров прыжка
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

	bool			active;					// состояние прыжка активно?
	EJumpState		state;					// состояние прыжка
	Fvector			from_pos;				// стартовая позиция
	Fvector			target_pos;				// целевая позиция
	float			target_yaw;				// угол вектора направления
	TTime			time_started;			// время начала прыжка
	TTime			time_next_allowed;		// время следующего разрешенного прыжка
	float			ph_time;				// расcчетное время прыжка
	u8				states_used;

	CObject			*entity;				// указатель на объект-цель (0 - нет)

	// LTX section
	float			m_fJumpFactor;			// коэффициент-делитель времени прыжка
	float			m_fJumpMinDist;			// мин. дистанция, возможная для прыжка
	float			m_fJumpMaxDist;			// макс. дистанция возможная для прыжка
	float			m_fJumpMaxAngle;		// макс. угол возможный для прыжка между монстром и целью
	TTime			m_dwDelayAfterJump;		// задержка после прыжка

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

	// выполнить проверку на возможность прыжка в текущий момент времени
	virtual bool	CanJump				() {return true;}
	// monster can define its own functions	
	virtual	void	PrepareJump			();
	virtual	void	ExecuteJump			();
	virtual	void	FinishJump			();
};

