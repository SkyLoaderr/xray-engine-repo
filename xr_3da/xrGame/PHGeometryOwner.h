#ifndef PH_GEOMETRY_OWNER_H
#define PH_GEOMETRY_OWNER_H
#include "Geometry.h"
DEFINE_VECTOR(CODEGeom*,GEOM_STORAGE,GEOM_I)
class CPHGeometryOwner
{
public:
	///
		//void					add_Sphere						(const Fsphere&		V);															//aux
		//void					add_Box							(const Fobb&		V);															//aux
		//void					add_Cylinder					(const Fcylinder&	V);															//aux
		// void					add_Shape						(const SBoneShape& shape);														//aux
		// void					add_Shape						(const SBoneShape& shape,const Fmatrix& offset);								//aux
		// CODEGeom*				last_geom						(){if(m_geoms.empty())return NULL; return m_geoms.back();}						//aux
		// bool					has_geoms						(){return !m_geoms.empty();}
		// void					set_ContactCallback				(ContactCallbackFun* callback);													//aux (may not be)
		// float					getRadius						();	
protected:
		GEOM_STORAGE			m_geoms;					//e					//bl
		dSpaceID				m_group;					//e					//bl
		Fvector					m_mass_center;				//e ??				//bl
		CPhysicsRefObject*		m_phys_ref_object;			//->to shell ??		//bl
		float					m_volume;					//e ??				//bl
		u16						ul_material;				//e ??				//bl
		ContactCallbackFun*			contact_callback;		//->to shell ??		//bt
		ObjectContactCallbackFun*	object_contact_callback;//->to shell ??		//st

		void					build_Geom						(CODEGeom&	V);																	//aux
		void					build_Geom						(u16 i);							
		void					set_body						(dBodyID body);
private:
};

#endif