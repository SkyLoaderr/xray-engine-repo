#pragma once

#include "xrPool.h"
#include "xr_collide_defs.h"

#pragma pack(push,4)

/*
Requirements:
0. Generic
	* O(1) insertion
		- radius completely determines	"level"
		- position completely detemines "node"
	* O(1) removal
	* 
1. Rendering
	* Should live inside spatial DB
	* Should have at least "bounding-sphere" or "bounding-box"
	* Should have pointer to "sector" it lives in
	* Approximate traversal order relative to point ("camera")
2. Spatial queries
	* Should live inside spatial DB
	* Should have at least "bounding-sphere" or "bounding-box"
*/

const float						c_spatial_min	= 8.f;

class ENGINE_API IRender_Sector;
class ENGINE_API ISpatial;
class ENGINE_API ISpatial_NODE;
class ENGINE_API ISpatial_DB;

//////////////////////////////////////////////////////////////////////////
class ENGINE_API ISpatial
{
public:
	u32							spatial_type;			
	Fvector						spatial_center;			// OWN:
	float						spatial_radius;			// OWN:
	Fvector						spatial_node_center;	// Cached node center for TBV optimization
	float						spatial_node_radius;	// Cached node bounds for TBV optimization
	ISpatial_NODE*				spatial_node_ptr;		// Cached parent node for "empty-members" optimization
	IRender_Sector*				spatial_sector;
public:
	BOOL						spatial_inside	();
public:
	virtual		void			spatial_move	();

	ISpatial					();
	virtual ~ISpatial			();
};

class ENGINE_API ISpatial_NODE
{
public:
	typedef	_W64 unsigned		ptrt;
public:
	ISpatial_NODE*				parent;					// parent node for "empty-members" optimization
	ISpatial_NODE*				children		[8];	// children nodes
	xr_vector<ISpatial*>		items;					// own items
public:
	void						_init			(ISpatial_NODE* _parent);
	void						_remove			(ISpatial*		_S);
	void						_insert			(ISpatial*		_S);
	BOOL						_empty			()						
	{
		return items.empty() && 
			0!=(
				ptrt(children[0])|ptrt(children[1])|
				ptrt(children[2])|ptrt(children[3])|
				ptrt(children[4])|ptrt(children[5])|
				ptrt(children[6])|ptrt(children[7])
			);	
	}
};

class ENGINE_API ISpatial_DB
{
private:
	poolSS<ISpatial_NODE,128>		allocator;
	xr_vector<ISpatial_NODE*>		allocator_pool;
	Fvector							v_center;
	float							f_bounds;
	ISpatial*						rt_insert_object;

public:
	ISpatial_NODE*					m_root;
	xr_vector<ISpatial*>			q_result;
	u32								stat_nodes;
	CStatTimer						stat_insert;
	CStatTimer						stat_remove;
private:
	IC u32							_octant			(u32 x, u32 y, u32 z)	{	return z*4 + y*2 + x;	}

	ISpatial_NODE*					_node_create	();
	void 							_node_destroy	(ISpatial_NODE* &P);

	void							_insert			(ISpatial_NODE* N, Fvector& n_center, float n_radius);
	void							_remove			(ISpatial_NODE* N, ISpatial_NODE* N_sub);
public:
	// managing
	void							initialize		(Fbox& BB);
	void							destroy			();
	void							insert			(ISpatial* S);
	void							remove			(ISpatial* S);
	void							update			(u32 nodes=8);

public:
	enum
	{
		O_ONLYFIRST		= (1<<0),
		O_ONLYNEAREST	= (1<<1),
		O_ORDERED		= (1<<2),
		O_force_u32		= u32(-1)
	};

	// query
	void							q_ray			(u32 _o, u32 _mask, const Fvector&	_start,  const Fvector&	_dir, float _range);
	void							q_box			(u32 _o, u32 _mask, const Fvector&	_center, const Fvector& _size);
	void							q_sphere		(u32 _o, u32 _mask, const Fvector&	_center, const Fvector& _radius);
	void							q_frustum		(u32 _o, u32 _mask, const CFrustum&	_frustum);
};

extern ISpatial_DB*					SpatialSpace;

/*
class ENGINE_API ISpatial_SPACE
{
public:
	typedef xr_vector<ISpatial*>	NL_TYPE;
	typedef CObject**				NL_IT;
public:
	ISpatial_NODE*					m_dynamic;
	CDB::MODEL*						m_static;

	NL_TYPE							q_nearest;
	clQueryCollision				q_result;
private:
	poolSS<ISpatial_NODE,128>		allocator;
	xr_vector<ISpatial_NODE*>		allocator_pool;
	Fvector							v_center;
	float							f_bounds;
	ISpatial*						rt_insert_object;

	IC u32							_octant			(u32 x, u32 y, u32 z)	{	return z*4 + y*2 + x;	}

	ISpatial_NODE*					_node_create	();
	void 							_node_destroy	(ISpatial_NODE* &P);

	void							_insert			(ISpatial_NODE* N, Fvector& n_center, float n_radius);
	void							_remove			(ISpatial_NODE* N, ISpatial_NODE* N_sub);
public:
	void							insert			(ISpatial* S);
	void							remove			(ISpatial* S);

	// Occluded/No
	BOOL							RayTest				( const Fvector &start, const Fvector &dir, float range=MAX_TEST_RANGE, BOOL bDynamic=TRUE, Collide::ray_cache* cache=NULL);

	// Game raypick (nearest) - returns object and addititional params
	BOOL							RayPick				( const Fvector &start, const Fvector &dir, float range, Collide::ray_query& R);

	// General collision query
	void							BoxQuery			( const Fbox& B, const Fmatrix& M, u32 flags=clGET_TRIS|clGET_BOXES|clQUERY_STATIC|clQUERY_DYNAMIC);
	int								GetNearest			( const Fvector &point, float range );
	CDB::TRI*						GetStaticTris		() { return Static.get_tris();  }
	CDB::MODEL*						GetStaticModel		() { return &Static; }
};
*/

#pragma pack(pop)
