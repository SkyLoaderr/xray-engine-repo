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

			// вернуть активность объекта		
			bool	is_active_object	(CGameObject *obj);
			
			// активировать объект
			void	activate			(CGameObject *obj, float strength, float height, u32 max_time_keep);

			// деактивировать все объекты
			void	deactivate			();
			// деактивировать объект
			void	deactivate			(CGameObject *obj);

			// вернуть активность телекинеза
			bool	is_active			() {return active;}
			// вернуть количество кониролируемых объектов
			u32		get_objects_count	() {return objects.size();}

			// бросить объект 'obj' в позицию 'target'
			void	fire				(CGameObject *obj, const Fvector &target);
	
			// бросить все объекты в позицию 'target'
			void	fire				(const Fvector &target);

			// обновить состон€ие на shedule_Update			
			void	schedule_update		();
private:
	// обновление на шагах физики
	virtual void 	PhDataUpdate		(dReal step);
	virtual void 	PhTune				(dReal step);
};

