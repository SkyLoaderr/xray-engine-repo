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
				
	// Инициализация параметров прыжка. Необходимо вызывать в Monster::Init
			void	Init				(CCustomMonster *pM); 
	// Загрузка параметров из ltx. Необходимо вызывать в Monster::Load
			void	Load				(LPCSTR section);
			
			void	SetJump				();
			
	// Общая проверка на прыжок
			bool	CheckJump			(Fvector from_pos, Fvector to_pos, CObject *pO);
	// Переключить следующее состояние прыжка	
			void	SwitchJumpState		();

	// Выполнить проверку на возможность прыжка в текущий момент времени
	virtual bool	CanJump				() {return true;}
	
	virtual	void	PrepareJump			();
	virtual	void	ExecuteJump			();
	virtual	void	FinishJump			();

	//	// Обновляет состояние прыжка в каждом фрейме (вызывается из UpdateCL)
	//	void		JMP_Update				();


};

