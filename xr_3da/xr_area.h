#ifndef __XR_AREA_H__
#define __XR_AREA_H__

#include "xr_qlist.h"
#include "xr_collide_form.h"
#include "xr_collide_defs.h"

// refs
class ENGINE_API				ICollisionForm;
class ENGINE_API				CObject;

//-----------------------------------------------------------------------------------------------------------
//Space Area
//-----------------------------------------------------------------------------------------------------------
class	ENGINE_API						CObjectSpace
{
private:
	// Debug
	ref_shader							sh_debug;
	CDB::MODEL							Static;
	Fvector								Static_Shift;
public:
	xr_vector<CObject*>					q_nearest;
	clQueryCollision					q_result;

	clQueryCollision					q_debug;
	xr_vector<std::pair<Fsphere,u32> >	dbg_S;
public:
										CObjectSpace		( );
										~CObjectSpace		( );

	void								Load				( );

	// Occluded/No
	BOOL								RayTest				( const Fvector &start, const Fvector &dir, float range, BOOL bDynamic=TRUE, Collide::ray_cache* cache=NULL);

	// Game raypick (nearest) - returns object and addititional params
	BOOL								RayPick				( const Fvector &start, const Fvector &dir, float range, Collide::ray_query& R);
	BOOL								RayPickW			( const Fvector &start, const Fvector &dir, float range, float width, Collide::ray_query& R);

	// General collision query
	void								BoxQuery			( const Fbox& B, const Fmatrix& M, u32 flags=clGET_TRIS|clGET_BOXES|clQUERY_STATIC|clQUERY_DYNAMIC);

	BOOL								EllipsoidCollide	( ICollisionForm *object, const Fmatrix& T, const Fvector& center_pos, const Fbox& bb);

	int									GetNearest			( ICollisionForm *obj, float range );
	int									GetNearest			( const Fvector &point, float range );

	CDB::TRI*							GetStaticTris		() { return Static.get_tris();  }
	CDB::MODEL*							GetStaticModel		() { return &Static; }

	// Device dependance and debugging
	void								dbgRender			();
	ref_shader							dbgGetShader		()	{ return sh_debug;	}
};

#endif //__XR_AREA_H__
