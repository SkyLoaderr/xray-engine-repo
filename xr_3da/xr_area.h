#ifndef __XR_AREA_H__
#define __XR_AREA_H__

#include "xr_qlist.h"
#include "xr_collide_form.h"
#include "xr_collide_defs.h"
#include "fixedvector.h"

const float CL_SLOT_SIZE		= 16.f;
const float CL_INV_SLOT_SIZE	= 1.f/CL_SLOT_SIZE;

ENGINE_API extern int			psCollideActDepth;
ENGINE_API extern int			psCollideActStuckDepth;

// refs
class ENGINE_API CCFModel;
class ENGINE_API CObject;

//-----------------------------------------------------------------------------------------------------------
//Area slot
//-----------------------------------------------------------------------------------------------------------
#define MAX_TEST_RANGE			100

//-----------------------------------------------------------------------------------------------------------
//Space Area
//-----------------------------------------------------------------------------------------------------------
class	ENGINE_API			CObjectSpace
{
private:
	friend class			CCFModel;

	IC void minmax(float &mn, float &mx) { if (mn > mx) swap(mn,mx); }
public:
	struct SCollisionData{
		// data about player movement
		Fvector				vVelocity;
		Fvector				vSourcePoint;
		
		// for error handling  
		Fvector				vLastSafePosition;
		BOOL				bStuck; 
		
		// data for collision response 
		BOOL				bFoundCollision;
		float				fNearestDistance;					// nearest distance to hit
		Fvector				vNearestIntersectionPoint;			// on sphere
		Fvector				vNearestPolygonIntersectionPoint;	// on polygon
		
		Fvector				vRadius;
	};
private:
	DWORD					dwQueryID;
	BOOL					nl_append		( int x, int z, const Fvector2& O, const Fvector2& D );

	struct SLOT
	{
		DWORD			dwQueryID;
		CList<CObject*>	lst;
	};

	CQList<SLOT>			Dynamic;
	RAPID::Model			Static;
	Fvector					Static_Shift;

	IC void					GetRect			( const Fvector &center, Irect &rect, float range );
	IC void					GetRect			( const CCFModel *obj, Irect &rect );
	IC int					GetNearest		( CCFModel *obj, float range );
	IC int					GetNearest		( const Fvector &point, float range );

	BOOL					TestRaySlot		(int x, int z, const Fvector2& start, const Fvector2& dir);
	void					CaptureSlots	(const Fvector& start, const Fvector& dir, float range);

	IC int 					TransA			( float d ) { return iFloor(d*CL_INV_SLOT_SIZE); }
	IC int 					TransX			( float d ) { return TransA(d+Static_Shift.x); }
	IC int 					TransZ			( float d ) { return TransA(d+Static_Shift.z); }
	IC void					InvTrans		( Fvector2& v, int x, int z) {v.set(CL_SLOT_SIZE*(x+.5f)-Static_Shift.x,CL_SLOT_SIZE*(z+.5f)-Static_Shift.z);}
	IC void					InvTrans		( Fvector& v, int x, int z) {v.set(CL_SLOT_SIZE*(x+.5f)-Static_Shift.x,0.f,CL_SLOT_SIZE*(z+.5f)-Static_Shift.z);}

	// safe translate auxilary routines
	CList<Collide::tri>		clContactedT;
	CList<Collide::elipsoid>clContactedE;
	void					clCheckCollision(SCollisionData& cl);
	void					clResolveStuck	(SCollisionData& cl, Fvector& position);
	Fvector					CollideWithWorld(SCollisionData& cl, Fvector position, Fvector velocity,WORD cnt=0);

public:
	typedef svector<CCFModel*,128>	NL_TYPE;
	typedef CCFModel**				NL_IT;

public:
	NL_TYPE					nearest_list;
	clQueryCollision		q_result;

	clQueryCollision			q_debug;
	CList<Fmatrix>				dbg_E;
	CList<pair<Fsphere,DWORD> >	dbg_S;
	CList<Fvector2>				dbg_Slot;
public:
							CObjectSpace		( );
							~CObjectSpace		( );

	void					Load				( CStream  * );

	void					Object_Register		( CObject *O );
	void					Object_Move			( CObject *O );
	void					Object_Unregister	( CObject *O );

	// Occluded/No
	BOOL					RayTest				( const Fvector &start, const Fvector &dir, float range=MAX_TEST_RANGE, BOOL bDynamic=TRUE, Collide::ray_cache* cache=NULL);

	// Game raypick (nearest) - returns object and addititional params
	BOOL					RayPick				( const Fvector &start, const Fvector &dir, float range, Collide::ray_query& R);
	BOOL					RayPickW			( const Fvector &start, const Fvector &dir, float range, float width, Collide::ray_query& R);

	// General collision query
	void					BoxQuery			( const Fbox& B, const Fmatrix& M, DWORD flags=clGET_TRIS|clGET_BOXES|clQUERY_STATIC|clQUERY_DYNAMIC);

	void					cl_Move				( const CCFModel *object, const Fmatrix& T, const Fvector& velocity, const Fbox& bb, const Fbox& bb_foots, Fvector& final_pos, Fvector& final_vel, float sq_vel, BOOL bDynamic=false );
	BOOL					EllipsoidCollide	( CCFModel *object, const Fmatrix& T, const Fvector& center_pos, const Fbox& bb);

	BOOL					TestNearestObject	( CCFModel *object, const Fvector& center, float radius);

	RAPID::tri*				GetStaticTris		() { return Static.GetTris();  }
	RAPID::Model*			GetStaticModel		() { return &Static; }

	void					dbgRender			();
};


#endif //__XR_AREA_H__
