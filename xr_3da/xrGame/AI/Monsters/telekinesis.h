#pragma once

#include "telekinetic_object.h"
#include "../../PHObject.h"




class CTelekinesis : public CPHUpdateObject {

protected:
	DEFINE_VECTOR(CTelekineticObject*,TELE_OBJECTS,TELE_OBJECTS_IT);
	TELE_OBJECTS			objects;
	bool					active;

public:
					CTelekinesis		();
	virtual			~CTelekinesis		();

			// allocates relevant TelekineticObject


			// ������������ ������
virtual		CTelekineticObject*	activate(CPhysicsShellHolder *obj, float strength, float height, u32 max_time_keep, bool rot = true);

			// �������������� ��� �������
			void	deactivate			();

			//clear objects (does not call release, but call switch to TS_None)
			void	clear_deactivate	();
			// clear 
virtual		void	clear				();
virtual		void	clear_notrelevant   ();
			// �������������� ������
			void	deactivate			(CPhysicsShellHolder *obj);
			void	remove_object		(TELE_OBJECTS_IT it);
			void	remove_object		(CPhysicsShellHolder *obj);
			// ������� ��� ������� � ������� 'target'
			void	fire_all			(const Fvector &target);
			
			// ������� ������ 'obj' � ������� 'target' � ������ ���� ���� 
			void	fire				(CPhysicsShellHolder *obj, const Fvector &target, float power);

			// ������� ������ 'obj' � ������� 'target' � ������ ���� ���� 
			void	fire_t				(CPhysicsShellHolder *obj, const Fvector &target, float time);


			// ������� ���������� ����������
			bool	is_active			() {return active;}

			// ������� ���������� �������		
			bool	is_active_object	(CPhysicsShellHolder *obj);
			
			// ������� ���������� �������������� ��������
			u32		get_objects_count	();
			
			// ������� ������ �� ������� � �������
			// a	copy of the object!
CTelekineticObject	get_object_by_index (u32 index) {VERIFY(objects.size() > index); return *objects[index];}
					
			// �������� ��������� �� shedule_Update			
			void	schedule_update		();

			// ������ ��� ������ - ������� ��� ����� �� ������
			void	remove_links		(CObject *O);

protected:
	virtual CTelekineticObject*	alloc_tele_object(){return xr_new<CTelekineticObject>();}
private:

	// ���������� �� ����� ������
	virtual void 	PhDataUpdate		(dReal step);
	virtual void 	PhTune				(dReal step);
	

};

