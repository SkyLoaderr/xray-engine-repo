#pragma once

#include "telekinetic_object.h"
#include "../PHObject.h"

class CTelekinesis : public CPHUpdateObject /* make this object update physics correctly */ {
	
	DEFINE_VECTOR(CTelekineticObject,TELE_OBJECTS,TELE_OBJECTS_IT);
	
	TELE_OBJECTS	objects;
	bool			active;

public:
					CTelekinesis		();
	virtual			~CTelekinesis		();

			
			// активировать объект
			void	activate			(CGameObject *obj, float strength, float height, u32 max_time_keep);

			// деактивировать все объекты
			void	deactivate			();
			// деактивировать объект
			void	deactivate			(CGameObject *obj);

			// бросить объект 'obj' в позицию 'target'
			void	fire				(CGameObject *obj, const Fvector &target);
	
			// бросить все объекты в позицию 'target'
			void	fire				(const Fvector &target);

			// вернуть активность телекинеза
			bool	is_active			() {return active;}

			// вернуть активность объекта		
			bool	is_active_object	(CGameObject *obj);

			// вернуть количество кониролируемых объектов
			u32		get_objects_count	() {return objects.size();}
			
			// вернуть объект по индексу в массиве
CTelekineticObject	get_object_by_index (u32 index) {VERIFY(objects.size() > index); return objects[index];}
					
			// обновить состон€ие на shedule_Update			
			void	schedule_update		();

private:

	// обновление на шагах физики
	virtual void 	PhDataUpdate		(dReal step);
	virtual void 	PhTune				(dReal step);
};

