#ifndef PH_STATIC_GEOM_SHELL_H
#define PH_STATIC_GEOM_SHELL_H
#include "PHGeometryOwner.h"
#include "PHObject.h"

class CPHStaticGeomShell: 
	public CPHGeometryOwner,
	public CPHObject
{
			void			get_spatial_params	();	
virtual		void			activate			(){}
virtual		dGeomID			dSpacedGeom			(){return dSpacedGeometry();}
virtual		void			PhDataUpdate		(dReal step){}
virtual		void			PhTune				(dReal step){}
virtual		void			InitContact			(dContact* c,bool& do_collide){}
public:
			void			Activate			(const Fmatrix& form);
			void			Deactivate			();
							CPHStaticGeomShell	();
};
#endif