#ifndef PH_STATIC_GEOM_SHELL_H
#define PH_STATIC_GEOM_SHELL_H
#include "PHGeometryOwner.h"
#include "PHObject.h"

class CPHStaticGeomShell: 
	public CPHGeometryOwner,
	public CPHObject,
	public CPHUpdateObject
{
			void			get_spatial_params	();	
virtual		void			activate			(){CPHUpdateObject::Activate();}
virtual		dGeomID			dSpacedGeom			(){return dSpacedGeometry();}
virtual		void			PhDataUpdate		(dReal step){Island().Step(step);Island().Unmerge();CPHUpdateObject::Deactivate();}
virtual		void			PhTune				(dReal step){}
virtual		void			InitContact			(dContact* c,bool& do_collide,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/){}
public:
			void			Activate			(const Fmatrix& form);
			void			Deactivate			();
							CPHStaticGeomShell	();
};

CPHStaticGeomShell* P_BuildStaticGeomShell(CGameObject* obj,ObjectContactCallbackFun* object_contact_callback);
#endif