#ifndef __XR_AREA_H__
#define __XR_AREA_H__

#include "xr_collide_form.h"
#include "xr_collide_defs.h"

// refs
class ENGINE_API	ISpatial;
class ENGINE_API	ICollisionForm;
class ENGINE_API	CObject;

//-----------------------------------------------------------------------------------------------------------
//Space Area
//-----------------------------------------------------------------------------------------------------------
class	ENGINE_API						CObjectSpace
{
private:
	// Debug
	CDB::MODEL							Static;
	Fvector								Static_Shift;
	Fbox								m_BoundingVolume;
	xrXRC								xrc;
	Collide::rq_results					r_temp;
public:
	xr_vector<ISpatial*>				r_spatial;
	xr_vector<CObject*>					q_nearest;
	clQueryCollision					q_result;

#ifdef DEBUG
	ref_shader							sh_debug;
	clQueryCollision					q_debug;
	xr_vector<std::pair<Fsphere,u32> >	dbg_S;
#endif

	Collide::rq_results					r_results;
public:
										CObjectSpace		( );
										~CObjectSpace		( );

	void								Load				( );

	// Occluded/No
	BOOL								RayTest				( const Fvector &start, const Fvector &dir, float range, Collide::rq_target tgt, Collide::ray_cache* cache=NULL);

	// Game raypick (nearest) - returns object and addititional params
	BOOL								RayPick				( const Fvector &start, const Fvector &dir, float range, Collide::rq_target tgt, Collide::rq_result& R );

	// General collision query
	BOOL								RayQuery			( const Collide::ray_defs& rq, Collide::rq_callback* cb, LPVOID user_data);
	BOOL								RayQuery			( ICollisionForm* target, const Collide::ray_defs& rq);
	void								BoxQuery			( const Fbox& B, const Fmatrix& M, u32 flags=clGET_TRIS|clGET_BOXES|clQUERY_STATIC|clQUERY_DYNAMIC);

	BOOL								EllipsoidCollide	( ICollisionForm *object, const Fmatrix& T, const Fvector& center_pos, const Fbox& bb);

	int									GetNearest			( ICollisionForm *obj, float range );
	int									GetNearest			( const Fvector &point, float range );

	CDB::TRI*							GetStaticTris		() { return Static.get_tris();	}
	Fvector*							GetStaticVerts		() { return Static.get_verts(); }
	CDB::MODEL*							GetStaticModel		() { return &Static;			}

	const Fbox&							GetBoundingVolume	() { return m_BoundingVolume;}

	// Debugging
#ifdef DEBUG
	void								dbgRender			();
	ref_shader							dbgGetShader		()	{ return sh_debug;	}
#endif
};

#endif //__XR_AREA_H__
