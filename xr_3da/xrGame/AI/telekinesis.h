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

			
			// ������������ ������
			void	activate			(CGameObject *obj, float strength, float height, u32 max_time_keep);

			// �������������� ��� �������
			void	deactivate			();
			// �������������� ������
			void	deactivate			(CGameObject *obj);

			// ������� ������ 'obj' � ������� 'target'
			void	fire				(CGameObject *obj, const Fvector &target);
	
			// ������� ��� ������� � ������� 'target'
			void	fire				(const Fvector &target);

			// ������� ���������� ����������
			bool	is_active			() {return active;}

			// ������� ���������� �������		
			bool	is_active_object	(CGameObject *obj);

			// ������� ���������� �������������� ��������
			u32		get_objects_count	() {return objects.size();}
			
			// ������� ������ �� ������� � �������
CTelekineticObject	get_object_by_index (u32 index) {VERIFY(objects.size() > index); return objects[index];}
					
			// �������� ��������� �� shedule_Update			
			void	schedule_update		();

private:

	// ���������� �� ����� ������
	virtual void 	PhDataUpdate		(dReal step);
	virtual void 	PhTune				(dReal step);
};

