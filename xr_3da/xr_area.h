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
	xrCriticalSection					Lock;
	CDB::MODEL							Static;
	Fvector								Static_Shift;
	Fbox								m_BoundingVolume;
	xrXRC								xrc;				// MT: dangerous
	collide::rq_results					r_temp;				// MT: dangerous
	xr_vector<ISpatial*>				r_spatial;			// MT: dangerous
//	xr_vector<CObject*>					q_nearest;			// MT: dangerous
//	clQueryCollision					q_result;			// MT: dangerous
//	collide::rq_results					r_results;			// MT: dangerous
public:

#ifdef DEBUG
	ref_shader							sh_debug;
	clQueryCollision					q_debug;			// MT: dangerous
	xr_vector<std::pair<Fsphere,u32> >	dbg_S;				// MT: dangerous
#endif

private:
	BOOL								_RayTest			( const Fvector &start, const Fvector &dir, float range, collide::rq_target tgt, collide::ray_cache* cache=NULL);
	BOOL								_RayPick			( const Fvector &start, const Fvector &dir, float range, collide::rq_target tgt, collide::rq_result& R );
	BOOL								_RayQuery			( collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb);
public:
										CObjectSpace		( );
										~CObjectSpace		( );

	void								Load				( );

	// Occluded/No
	BOOL								RayTest				( const Fvector &start, const Fvector &dir, float range, collide::rq_target tgt, collide::ray_cache* cache=NULL);

	// Game raypick (nearest) - returns object and addititional params
	BOOL								RayPick				( const Fvector &start, const Fvector &dir, float range, collide::rq_target tgt, collide::rq_result& R );

	// General collision query
	BOOL								RayQuery			( collide::rq_results& dest, const collide::ray_defs& rq, collide::rq_callback* cb, LPVOID user_data, collide::test_callback* tb=0);
	BOOL								RayQuery			( collide::rq_results& dest, ICollisionForm* target, const collide::ray_defs& rq);
	// void								BoxQuery			( collide::rq_results& dest, const Fbox& B, const Fmatrix& M, u32 flags=clGET_TRIS|clGET_BOXES|clQUERY_STATIC|clQUERY_DYNAMIC);

	int									GetNearest			( xr_vector<CObject*>&	q_nearest, ICollisionForm *obj, float range );
	int									GetNearest			( xr_vector<CObject*>&	q_nearest, const Fvector &point, float range );

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
