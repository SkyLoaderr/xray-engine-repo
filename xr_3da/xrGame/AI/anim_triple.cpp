#include "stdafx.h"
#include "anim_triple.h"

CAnimTriple::CAnimTriple()
{
	cur_state	= eStateNone;
}
CAnimTriple::~CAnimTriple()
{

}

void CAnimTriple::init_external(CMotionDef *m_def1, CMotionDef *m_def2, CMotionDef *m_def3)
{
	pool[0] = m_def1;
	pool[1]	= m_def2;
	pool[2]	= m_def3;
}

void CAnimTriple::activate()
{
	if (is_active()) deactivate();

	cur_state = eStatePrepare;
}

void CAnimTriple::deactivate()
{
	cur_state = eStateNone;
}

bool CAnimTriple::prepare_animation(CMotionDef **m)
{
	if (is_active()) {
		*m = pool[cur_state];
		if (cur_state != eStateExecute) cur_state = EStateAnimTriple(cur_state + 1);
		return true;
	}

	return false;
}


void CAnimTriple::pointbreak()
{
	cur_state = eStateFinalize;
}

