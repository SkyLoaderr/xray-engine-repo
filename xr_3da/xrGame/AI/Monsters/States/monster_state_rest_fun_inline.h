#pragma once

#include "../../../PhysicsShell.h"
#include "../../../PHInterpolation.h"
#include "../../../PHElement.h"

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateMonsterRestFunAbstract CStateMonsterRestFun<_Object>

#define IMPULSE_TO_CORPSE	20.f
#define MIN_DELAY			100
#define TIME_IN_STATE		15000

TEMPLATE_SPECIALIZATION
CStateMonsterRestFunAbstract::CStateMonsterRestFun(_Object *obj) : inherited(obj, ST_Rest)
{
}

TEMPLATE_SPECIALIZATION
void CStateMonsterRestFunAbstract::initialize()
{
	inherited::initialize	();

	time_last_hit			= 0;

#ifdef DEBUG
	if (psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(object,object).remove_item	(u32(0));
		DBG().object_info(object,object).add_item		("Rest :: Fun", D3DCOLOR_XRGB(255,0,0), 0);
	}
#endif

}


TEMPLATE_SPECIALIZATION
void CStateMonsterRestFunAbstract::execute()
{
	Fvector point;
	float	dist;
	
	Fvector dir;
	dir.sub			(object->CorpseMan.get_corpse_position(), object->Position());
	dist			= dir.magnitude();
	dir.normalize	();
	point.mad		(object->CorpseMan.get_corpse_position(), dir, 2.0f);

	object->set_action									(ACT_RUN);
	object->movement().set_target_point			(point);
	object->movement().set_rebuild_time			(100 + u32(50.f * dist));
	object->movement().set_use_covers			(false);
	object->movement().set_distance_to_end		(0.5f);
	object->MotionMan.accel_activate					(eAT_Calm);
	object->MotionMan.accel_set_braking					(false);
	
	object->set_state_sound								(MonsterSpace::eMonsterSoundIdle);
	
	if ((dist < object->get_sd()->m_fDistToCorpse + 0.5f) && (time_last_hit + MIN_DELAY < Device.dwTimeGlobal)) {
		CEntityAlive		*corpse = const_cast<CEntityAlive *>		(object->CorpseMan.get_corpse());
		CPhysicsShellHolder	*target = smart_cast<CPhysicsShellHolder *>	(corpse);

		if  (target && target->m_pPhysicsShell) {
			Fvector			dir;
			dir.add			(Fvector().sub(target->Position(), object->Position()), object->Direction());
			
			float			h,p;
			dir.getHP		(h,p);
			dir.setHP		(h, p + 5 * PI / 180);
			dir.normalize	();
			
			// ��������� ������
			for (u32 i=0; i<target->m_pPhysicsShell->Elements().size();i++) {
				target->m_pPhysicsShell->Elements()[i]->applyImpulse(dir, IMPULSE_TO_CORPSE * target->m_pPhysicsShell->getMass() / target->m_pPhysicsShell->Elements().size());
			}

			time_last_hit	= Device.dwTimeGlobal;
		}
	}
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterRestFunAbstract::check_start_conditions()
{
	return (object->CorpseMan.get_corpse() != 0);
}

TEMPLATE_SPECIALIZATION
bool CStateMonsterRestFunAbstract::check_completion()
{
	if (!object->CorpseMan.get_corpse()) return true;
	if (time_state_started + TIME_IN_STATE < Device.dwTimeGlobal) return true;
	return false;
}
