#ifndef PH_GEOMETRY_OWNER_H
#define PH_GEOMETRY_OWNER_H
#include "Geometry.h"
DEFINE_VECTOR(CODEGeom*,GEOM_STORAGE,GEOM_I)
class CPHGeometryOwner
{
protected:
		bool					b_builded;
		GEOM_STORAGE			m_geoms;					//e					//bl
		dSpaceID				m_group;					//e					//bl
		Fvector					m_mass_center;				//e ??				//bl
		CGameObject*			m_phys_ref_object;			//->to shell ??		//bl
		float					m_volume;					//e ??				//bl
		u16						ul_material;				//e ??				//bl
		ContactCallbackFun*			contact_callback;		//->to shell ??		//bt
		ObjectContactCallbackFun*	object_contact_callback;//->to shell ??		//st
public:
	///
		void					add_Sphere								(const Fsphere&		V);															//aux
		void					add_Box									(const Fobb&		V);															//aux
		void					add_Cylinder							(const Fcylinder&	V);															//aux
		void					add_Shape								(const SBoneShape& shape);														//aux
		void					add_Shape								(const SBoneShape& shape,const Fmatrix& offset);								//aux
		CODEGeom*				last_geom								(){if(m_geoms.empty())return NULL; return m_geoms.back();}						//aux
		bool					has_geoms								(){return !m_geoms.empty();}
		void					set_ContactCallback						(ContactCallbackFun* callback);													//aux (may not be)
		void					set_ObjectContactCallback				(ObjectContactCallbackFun* callback);											//called anywhere ph state influent
		void					set_PhysicsRefObject					(CGameObject* ref_object);												//aux
		CGameObject*			PhysicsRefObject						(){return m_phys_ref_object;}													//aux
		void					SetPhObjectInGeomData					(CPHObject* O);		

		void					SetMaterial								(u16 m)		  ;
		void					SetMaterial								(LPCSTR m){SetMaterial(GMLib.GetMaterialIdx(m));}								//aux
	IC	CODEGeom*				Geom									(u16 num)		{R_ASSERT2 (num<m_geoms.size(),"out of range"); return m_geoms[num]; }
		u16						numberOfGeoms							();																				//aux
		dGeomID					dSpacedGeometry							();																				//aux
		Fvector					get_mc_data								();																				//aux
		Fvector					get_mc_geoms							();																				//aux
		void					calc_volume_data						();																				//aux
const	Fvector&				local_mass_Center						()		{return m_mass_center;}													//aux
		float					get_volume								()		{calc_volume_data();return m_volume;};									//aux
		void					get_Extensions							(const Fvector& axis,float center_prg,float& lo_ext, float& hi_ext);			//aux
		float					getRadius								();	
		void					setStaticForm							(const Fmatrix& form);
		void					setPosition								(const Fvector& pos);
protected:
		void					get_mc_vs_transform						(Fvector& mc,const Fmatrix& m);
		void					build									();
		void					CreateSimulBase							();
		void					destroy									();
		void					build_Geom								(CODEGeom&	V);																	//aux
		void					build_Geom								(u16 i);							
		void					set_body								(dBodyID body);

								CPHGeometryOwner						();
virtual							~CPHGeometryOwner						();
private:
};

#endif