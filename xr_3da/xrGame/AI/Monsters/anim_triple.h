#pragma once

#include "../../../SkeletonAnimated.h"

enum EStateAnimTriple {
	eStatePrepare,
	eStateExecute,
	eStateFinalize,
	eStateNone
};

class CMonsterEventManager;

class CAnimTriple {
	EStateAnimTriple		m_current_state;
	EStateAnimTriple		m_previous_state;
	
	MotionID				pool[3];

	bool					m_active;
	CMonsterEventManager	*m_event_man;

	bool					m_execute_once;

public:
						CAnimTriple			();
	virtual 			~CAnimTriple		();

	virtual void		reinit_external		(CMonsterEventManager *man, const MotionID &m_def1, const MotionID &m_def2, const MotionID &m_def3, bool b_execute_once = true);
	
	virtual void		activate			();
	virtual void		deactivate			();

	virtual bool		is_active			() {return m_active;}

	// завершилась анимация, необходимо выбрать новую
	virtual bool		prepare_animation	(MotionID &m);

	// завершить фиксированную анимацию (вторую) и перейти к финальной
	virtual void		pointbreak			();

	EStateAnimTriple	get_state			(){return m_current_state;}

};

