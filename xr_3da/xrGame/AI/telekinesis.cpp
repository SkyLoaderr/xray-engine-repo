#include "stdafx.h"
#include "telekinesis.h"
#include "../entity_alive.h"
#include "../PhysicsShell.h"


CTelekinesis::CTelekinesis()
{
	active = false;
}

CTelekinesis::~CTelekinesis()
{

}

void CTelekinesis::activate(CGameObject *obj, float strength, float height, u32 max_time_keep)
{
	active = true;

	CTelekineticObject tele_object;		
	if (!tele_object.init(obj,strength, height,max_time_keep)) return;
		
	// �������� ������
	objects.push_back(tele_object);

	if (!objects.empty()) CPHUpdateObject::Activate();
}

void CTelekinesis::deactivate()
{
	active			= false;

	// ��������� ��� �������
	for (u32 i = 0; i < objects.size(); i++) objects[i].release();
	objects.clear	();

	CPHUpdateObject::Deactivate();
}


void CTelekinesis::deactivate(CGameObject *obj)
{
	// ����� ������
	TELE_OBJECTS_IT it = find(objects.begin(), objects.end(), obj);
	if (it == objects.end()) return;

	// ��������� ������
	it->release();
	
	// �������
	objects.erase(it);

	// ��������� �� ������ �����������
	if (objects.empty()) {
		CPHUpdateObject::Deactivate();
		active = false;
	}
}


void CTelekinesis::fire(const Fvector &target)
{
	if (!active) return;

	for (u32 i = 0; i < objects.size(); i++) objects[i].fire(target);

	deactivate();
}

void CTelekinesis::fire(CGameObject *obj, const Fvector &target)
{
	// ����� ������
	TELE_OBJECTS_IT it = find(objects.begin(), objects.end(), obj);
	if (it == objects.end()) return;

	// ������� ������
	it->fire(target);
}

// ������� ������ 'obj' � ������� 'target' � ������ ���� ���� 
void CTelekinesis::fire(CGameObject *obj, const Fvector &target, float power)
{
	// ����� ������
	TELE_OBJECTS_IT it = find(objects.begin(), objects.end(), obj);
	if (it == objects.end()) return;

	// ������� ������
	it->fire(target,power);
}


bool CTelekinesis::is_active_object(CGameObject *obj)
{
	// ����� ������
	TELE_OBJECTS_IT it = find(objects.begin(), objects.end(), obj);
	if (it == objects.end()) return false;

	return true;
}

void CTelekinesis::schedule_update()
{
	if (!active) return;

	// �������� ��������� ��������
	for (u32 i = 0; i < objects.size(); i++) {

		CTelekineticObject *cur_obj = &objects[i];

		switch (cur_obj->get_state()) {
		case TS_Raise: 
			if (cur_obj->check_height()) cur_obj->prepare_keep();// ������ ��������� ��������
			break;
		case TS_Keep:
			if (cur_obj->time_keep_elapsed()) deactivate(cur_obj->get_object());
			break;
		case TS_Fire:
			if (cur_obj->time_fire_elapsed()) deactivate(cur_obj->get_object());
			break;
		case TS_None: continue; 
		}
	}
}

void CTelekinesis::PhDataUpdate(dReal step)
{
	if (!active) return;

	for (u32 i = 0; i < objects.size(); i++) {
		switch (objects[i].get_state()) {
		case TS_Raise:	objects[i].raise(step); break;
		case TS_Keep:	objects[i].keep(); break;
		case TS_None:	break;
		}
	}
}

bool RemovePred(CTelekineticObject &tele_object)
{
	return (!tele_object.get_object() || 
		tele_object.get_object()->getDestroy() ||
		!tele_object.get_object()->m_pPhysicsShell ||
		!tele_object.get_object()->m_pPhysicsShell->bActive);
}

void  CTelekinesis::PhTune(dReal step)
{
	if (!active) return;

	//������ ��� ������ �� ������� �����������
	TELE_OBJECTS_IT it = remove_if(objects.begin(),objects.end(), RemovePred);
	objects.erase(it, objects.end());
	
	for (u32 i = 0; i < objects.size(); i++) {
		switch (objects[i].get_state()) {
		case TS_Raise:	
		case TS_Keep:	objects[i].enable();
		case TS_None:	break;
		}
	}
}

u32 CTelekinesis::get_objects_count() 
{
	u32 count = 0;
	for (u32 i=0;i<objects.size(); i++) {
		ETelekineticState state = objects[i].get_state();
		if ((state == TS_Raise) || (state == TS_Keep)) count++;
	}

	return count;
}

