#pragma once

#define TIME_TO_RAISE	2000
#define TIME_TO_KEEP	2000
#define STRENGTH		10.f

template <typename CMonster>
CTelekinesis<CMonster>::CTelekinesis()
{
	active = false;
}

template <typename CMonster>
CTelekinesis<CMonster>::~CTelekinesis()
{

}

template <typename CMonster>
void CTelekinesis<CMonster>::Activate()
{
	if (active)		return;
	VERIFY(objects.empty());

	active = true;

	// set object list
	Level().ObjectSpace.GetNearest		(monster->Position(),10.f); 
	xr_vector<CObject*> &tpNearest		= Level().ObjectSpace.q_nearest; 

	for (u32 i = 0; i < tpNearest.size(); i++) {
		CEntityAlive *pE = dynamic_cast<CEntityAlive *>(tpNearest[i]);
		if (!pE || !pE->m_pPhysicsShell) continue;

		objects.push_back(CTelekineticObject().set(pE,TS_Raise));
		pE->m_pPhysicsShell->set_ApplyByGravity(FALSE);
	}

}

template <typename CMonster>
void CTelekinesis<CMonster>::Deactivate()
{
	active			= false;

	for (u32 i = 0; i < objects.size(); i++) {
		Release(objects[i].pE);
	}

	objects.clear	();
}

template <typename CMonster>
void CTelekinesis<CMonster>::Update(float dt)
{
	if (!active) return;

	u32 cur_time = Level().timeServer();

	for (u32 i = 0; i < objects.size(); i++) {
		CTelekineticObject *cur_obj = &objects[i]; 

		if (cur_obj->state == TS_None) continue;

		if (cur_obj->state == TS_Raise)	{
			if (cur_obj->time_state_started + TIME_TO_RAISE > cur_time) Raise(cur_obj->pE);
			else {
				cur_obj->state = TS_Keep;
				cur_obj->time_state_started = cur_time;
			}
		}

		if (cur_obj->state == TS_Keep) {
			if (cur_obj->time_state_started + TIME_TO_KEEP > cur_time) Keep(cur_obj->pE);
			else {
				cur_obj->state = TS_None;
				cur_obj->time_state_started = cur_time;
				Release(cur_obj->pE);
			}
		}
	}
}

template <typename CMonster>
void CTelekinesis<CMonster>::Raise(CEntityAlive *pE) 
{
	Fvector dir;
	dir.set(0.f,-1.0f,0.f);
	pE->m_pPhysicsShell->applyImpulse(dir, 1.5f * pE->m_pPhysicsShell->getMass());
}

template <typename CMonster>
void CTelekinesis<CMonster>::Keep(CEntityAlive *pE)
{
	Fvector dir;
	dir.set(0.f,Random.randF(-0.1f,0.1f),0.f);
	pE->m_pPhysicsShell->applyImpulse(dir, 0.5f * pE->m_pPhysicsShell->getMass());	
}

template <typename CMonster>
void CTelekinesis<CMonster>::Release(CEntityAlive *pE)
{
	Fvector dir_inv;
	dir_inv.set(0.f,-1.0f,0.f);

	pE->m_pPhysicsShell->set_ApplyByGravity(TRUE);
	pE->m_pPhysicsShell->applyImpulse(dir_inv, 1.5f * pE->m_pPhysicsShell->getMass());
}

template <typename CMonster>
void CTelekinesis<CMonster>::Throw(CEntityAlive *pE, const Fvector &target)
{
	// calc dir
	Fvector dir;
	dir.sub(target,pE->Position());

	// just throw here
	pE->m_pPhysicsShell->set_ApplyByGravity(TRUE);
	pE->m_pPhysicsShell->applyImpulse(dir, STRENGTH * pE->m_pPhysicsShell->getMass());
}

template <typename CMonster>
void CTelekinesis<CMonster>::Throw(const Fvector &target)
{
	if (!active) return;
	
	for (u32 i = 0; i < objects.size(); i++) {
		Throw(objects[i].pE, target);
	}
}


