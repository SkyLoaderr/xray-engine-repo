#ifndef __XR_AREA_H__
#define __XR_AREA_H__

#include "xr_qlist.h"
#include "xr_collide_form.h"
#include "xr_collide_defs.h"

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
class	ENGINE_API			CObjectSpace : public pureDeviceCreate, pureDeviceDestroy
{
private:
	friend class			CCFModel;
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
	// Debug
	Shader*							sh_debug;
	
	u32							dwQueryID;
	BOOL							nl_append		( int x, int z, const Fvector2& O, const Fvector2& D );

	struct SLOT
	{
		u32				dwQueryID;
		vector<CObject*>	lst;
	};

	CQList<SLOT>					Dynamic;
	CDB::MODEL						Static;
	Fvector							Static_Shift;

	void							GetRect			( const Fvector &center, Irect &rect, float range );
	void							GetRect			( const CCFModel *obj, Irect &rect );

	BOOL							TestRaySlot		(int x, int z, const Fvector2& start, const Fvector2& dir);
	void							CaptureSlots	(const Fvector& start, const Fvector& dir, float range);

	IC int 							TransA			( float d ) { return iFloor(d*CL_INV_SLOT_SIZE); }
	IC int 							TransX			( float d ) { return TransA(d+Static_Shift.x); }
	IC int 							TransZ			( float d ) { return TransA(d+Static_Shift.z); }
	IC void							InvTrans		( Fvector2& v, int x, int z)	{v.set(CL_SLOT_SIZE*(x+.5f)-Static_Shift.x,CL_SLOT_SIZE*(z+.5f)-Static_Shift.z);}
	IC void							InvTrans		( Fvector& v, int x, int z)		{v.set(CL_SLOT_SIZE*(x+.5f)-Static_Shift.x,0.f,CL_SLOT_SIZE*(z+.5f)-Static_Shift.z);}

	// safe translate auxilary routines
	vector<Collide::tri>			clContactedT;
	vector<Collide::elipsoid>		clContactedE;
	void							clCheckCollision(SCollisionData& cl);
	void							clResolveStuck	(SCollisionData& cl, Fvector& position);
	Fvector							CollideWithWorld(SCollisionData& cl, Fvector position, Fvector velocity,WORD cnt=0);
public:
	typedef svector<CObject*,256>	NL_TYPE;
	typedef CObject**				NL_IT;

public:
	NL_TYPE							q_nearest;
	clQueryCollision				q_result;

	clQueryCollision				q_debug;
	vector<Fmatrix>					dbg_E;
	vector<pair<Fsphere,u32> >	dbg_S;
	vector<Fvector2>				dbg_Slot;

public:
									CObjectSpace		( );
									~CObjectSpace		( );

	void							Load				( CStream  * );

	void							Object_Register		( CObject *O );
	void							Object_Move			( CObject *O );
	void							Object_Unregister	( CObject *O );

	// Occluded/No
	BOOL							RayTest				( const Fvector &start, const Fvector &dir, float range=MAX_TEST_RANGE, BOOL bDynamic=TRUE, Collide::ray_cache* cache=NULL);

	// Game raypick (nearest) - returns object and addititional params
	BOOL							RayPick				( const Fvector &start, const Fvector &dir, float range, Collide::ray_query& R);
	BOOL							RayPickW			( const Fvector &start, const Fvector &dir, float range, float width, Collide::ray_query& R);

	// General collision query
	void							BoxQuery			( const Fbox& B, const Fmatrix& M, u32 flags=clGET_TRIS|clGET_BOXES|clQUERY_STATIC|clQUERY_DYNAMIC);

	void							cl_Move				( const CCFModel *object, const Fmatrix& T, const Fvector& velocity, const Fbox& bb, const Fbox& bb_foots, Fvector& final_pos, Fvector& final_vel, float sq_vel, BOOL bDynamic=false);
	BOOL							EllipsoidCollide	( CCFModel *object, const Fmatrix& T, const Fvector& center_pos, const Fbox& bb);

	int								GetNearest			( CCFModel *obj, float range );
	int								GetNearest			( const Fvector &point, float range );

	CDB::TRI*						GetStaticTris		() { return Static.get_tris();  }
	CDB::MODEL*						GetStaticModel		() { return &Static; }

	// Device dependance and debugging
	virtual void					OnDeviceCreate		();
	virtual void					OnDeviceDestroy		();
	void							dbgRender			();
	Shader*							dbgGetShader		()	{ return sh_debug;	}
};


#endif //__XR_AREA_H__
