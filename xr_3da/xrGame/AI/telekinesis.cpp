#include "stdafx.h"
#include "telekinesis.h"
#include "../entity_alive.h"


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
		
	// добавить объект
	objects.push_back(tele_object);

	if (!objects.empty()) CPHUpdateObject::Activate();
}

void CTelekinesis::deactivate()
{
	active			= false;

	// отпустить все объекты
	for (u32 i = 0; i < objects.size(); i++) objects[i].release();
	objects.clear	();

	CPHUpdateObject::Deactivate();
}


void CTelekinesis::deactivate(CGameObject *obj)
{
	// найти объект
	TELE_OBJECTS_IT it = find(objects.begin(), objects.end(), obj);
	if (it == objects.end()) return;

	// отпустить объект
	it->release();
	
	// удалить
	objects.erase(it);

	// проверить на полную деактивацию
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
	// найти объект
	TELE_OBJECTS_IT it = find(objects.begin(), objects.end(), obj);
	if (it == objects.end()) return;

	// бросить объект
	it->fire(target);
}

// бросить объект 'obj' в позицию 'target' с учетом коэф силы 
void CTelekinesis::fire(CGameObject *obj, const Fvector &target, float power)
{
	// найти объект
	TELE_OBJECTS_IT it = find(objects.begin(), objects.end(), obj);
	if (it == objects.end()) return;

	// бросить объект
	it->fire(target);
}


bool CTelekinesis::is_active_object(CGameObject *obj)
{
	// найти объект
	TELE_OBJECTS_IT it = find(objects.begin(), objects.end(), obj);
	if (it == objects.end()) return false;

	return true;
}

void CTelekinesis::schedule_update()
{
	if (!active) return;

	// обновить состояние объектов
	for (u32 i = 0; i < objects.size(); i++) {

		CTelekineticObject *cur_obj = &objects[i];

		switch (cur_obj->get_state()) {
		case TS_Raise: 
			if (cur_obj->check_height()) cur_obj->prepare_keep();// начать удержание предмета
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
	return (!tele_object.get_object() || tele_object.get_object()->getDestroy());
}

void  CTelekinesis::PhTune(dReal step)
{
	if (!active) return;

	//убрать все объеты с getDestroy() == true
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



