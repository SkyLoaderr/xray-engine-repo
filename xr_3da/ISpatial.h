#pragma once

#include "xrPool.h"

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
	Fvector						spatial_center;			// OWN:
	float						spatial_radius;			// OWN:
	Fvector						spatial_node_center;	// Cached node center for TBV optimization
	float						spatial_node_radius;	// Cached node bounds for TBV optimization
	ISpatial_NODE*				spatial_node_ptr;		// Cached parent node for "empty-members" optimization
	IRender_Sector*				spatial_sector;
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
public:
	poolSS<ISpatial_NODE,128>	allocator;
	xr_vector<ISpatial_NODE*>	allocator_pool;
	Fvector						v_center;
	float						f_bounds;
	ISpatial_NODE*				root;
private:
	ISpatial*					rt_insert_object;
	IC u32						_octant			(u32 x, u32 y, u32 z)	{	return z*4 + y*2 + x;	}

	ISpatial_NODE*				_node_create	();
	void 						_node_destroy	(ISpatial_NODE* &P);

	void						_insert			(ISpatial_NODE* N, Fvector& n_center, float n_radius);
	void						_remove			(ISpatial_NODE* N, ISpatial_NODE* N_sub);
public:
	void						insert			(ISpatial* S);
	void						remove			(ISpatial* S);
};

#pragma pack(pop)
