#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBurerAttackAbstract CStateBurerAttack<_Object>

TEMPLATE_SPECIALIZATION
CStateBurerAttackAbstract::CStateBurerAttack(LPCSTR state_name, SSubStatePtr state_tele, SSubStatePtr state_gravi, SSubStatePtr state_melee) : inherited(state_name)
{
	states[eStateTelekinetic]	= state_tele;
	states[eStateGravi]			= state_gravi;
	states[eStateMelee]			= state_melee;
}

TEMPLATE_SPECIALIZATION
CStateBurerAttackAbstract::~CStateBurerAttack()
{
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackAbstract::Load(LPCSTR section)
{
	add_state				(states[eStateTelekinetic],		eStateTelekinetic,	1);
	add_state				(states[eStateGravi],			eStateGravi,		2);
	add_state				(states[eStateMelee],			eStateMelee,		0);

	add_transition			(eStateTelekinetic,	eStateMelee,1,1);
	add_transition			(eStateGravi,		eStateMelee,1,1);
	add_transition			(eStateTelekinetic,	eStateGravi,1,1);

	inherited::Load			(section);
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackAbstract::reinit(_Object *object)
{
	inherited::reinit		(object);
	set_current_state		(eStateTelekinetic);
	set_dest_state			(eStateTelekinetic);
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackAbstract::initialize()
{
	inherited::initialize();
}

#define GOOD_DISTANCE			20.f
#define MAX_HANDLED_OBJECTS		3
#define CHECK_OBJECTS_RADIUS	10.f
#define MINIMAL_MASS			50.f
#define MAXIMUM_MASS			500.f

TEMPLATE_SPECIALIZATION
void CStateBurerAttackAbstract::execute()
{
	const CEntityAlive *enemy = m_object->EnemyMan.get_enemy();
	float dist = m_object->Position().distance_to(enemy->Position());
	Fvector dir;
	dir.sub(enemy->Position(), m_object->Position());
	dir.normalize();

	if (dist > GOOD_DISTANCE) {
		if (m_object->CTelekinesis::get_objects_count() < MAX_HANDLED_OBJECTS) {
			
			// получить список объектов вокруг монстра
			Level().ObjectSpace.GetNearest	(m_object->Position(), CHECK_OBJECTS_RADIUS); 
			xr_vector<CObject*> &tpNearest	= Level().ObjectSpace.q_nearest; 
			bool b_objects_found = true;

			if (tpNearest.empty()) {
				// получить список объектов на середине пути от монстра до врага
				Fvector pos;
				pos.mad(m_object->Position(), dir, dist / 2.f);
				Level().ObjectSpace.GetNearest(pos, CHECK_OBJECTS_RADIUS); 
				tpNearest	= Level().ObjectSpace.q_nearest; 

				if (tpNearest.empty()) b_objects_found = false;
			} 
			
			if (b_objects_found) {
				// выбрать объект
				for (u32 i=0;i<tpNearest.size();i++) {
					CGameObject *obj = dynamic_cast<CGameObject *>(tpNearest[i]);
					// проверка по объекту и его массе
					if (!obj || !obj->m_pPhysicsShell || (obj->m_pPhysicsShell->getMass() < MINIMAL_MASS) || (obj->m_pPhysicsShell->getMass() > MINIMAL_MASS)) continue;

					// проверить, активен ли уже объект
					if (m_object->CTelekinesis::is_active_object(obj)) continue;

					// применить телекинез на объект
					m_object->CTelekinesis::activate(obj, 3.f, 3.f, 10000);
					break;
				}
			}
		}
	}

	if (m_object->CTelekinesis::get_objects_count() > 0) {
		// обработать объекты
		set_dest_state(eStateTelekinetic);
	} else {
		// бить вплотную
		set_dest_state(eStateMelee);
	}
	
	inherited::execute();
}

TEMPLATE_SPECIALIZATION
void CStateBurerAttackAbstract::finalize()
{
	inherited::finalize();
}

