#include "stdafx.h"
#include "ispatial.h"
#include "render.h"

ISpatial_DB					g_SpatialSpace;

Fvector	c_spatial_offset	[8]	= 
{
	{ -1, -1, -1	},
	{  1, -1, -1	},
	{ -1,  1, -1	},
	{  1,  1, -1	},
	{ -1, -1,  1	},
	{  1, -1,  1	},
	{ -1,  1,  1	},
	{  1,  1,  1	}
};

//////////////////////////////////////////////////////////////////////////
ISpatial::ISpatial			(void)
{
	spatial.center.set		(0,0,0);
	spatial.radius			= 0;
	spatial.node_center.set	(0,0,0);
	spatial.node_radius		= 0;
	spatial.node_ptr		= NULL;
	spatial.sector			= NULL;
}

BOOL	ISpatial::spatial_inside()
{
	float	dr	= -(- spatial.node_radius + spatial.radius);
	if (spatial.center.x < spatial.node_center.x - dr)	return FALSE;
	if (spatial.center.x > spatial.node_center.x + dr)	return FALSE;
	if (spatial.center.y < spatial.node_center.y - dr)	return FALSE;
	if (spatial.center.y > spatial.node_center.y + dr)	return FALSE;
	if (spatial.center.z < spatial.node_center.z - dr)	return FALSE;
	if (spatial.center.z > spatial.node_center.z + dr)	return FALSE;
	return TRUE;
}

BOOL	verify_sp	(ISpatial* sp, Fvector& node_center, float node_radius)
{
	float	dr	= -(- node_radius + sp->spatial.radius);
	if (sp->spatial.center.x < node_center.x - dr)	return FALSE;
	if (sp->spatial.center.x > node_center.x + dr)	return FALSE;
	if (sp->spatial.center.y < node_center.y - dr)	return FALSE;
	if (sp->spatial.center.y > node_center.y + dr)	return FALSE;
	if (sp->spatial.center.z < node_center.z - dr)	return FALSE;
	if (sp->spatial.center.z > node_center.z + dr)	return FALSE;
	return TRUE;
}

void	ISpatial::spatial_register	()
{
	if (spatial.node_ptr)
	{
		// already registered - nothing to do
	} else {
		// register
		g_SpatialSpace.insert	(this);
		spatial.sector			= ::Render->detectSector(spatial.center);
	}
}

void	ISpatial::spatial_unregister()
{
	if (spatial.node_ptr)
	{
		// remove
		g_SpatialSpace.remove	(this);
		spatial.node_ptr		= NULL;
		spatial.sector			= NULL;
	} else {
		// already unregistered
	}
}

void	ISpatial::spatial_move	()
{
	if (spatial.node_ptr)
	{
		//*** somehow it was determined that object has been moved
		IRender_Sector*		S				= ::Render->detectSector(spatial.center);
		if (S)				spatial.sector	= S;

		//*** check if we are supposed to correct it's spatial location
		if			(spatial_inside())	return;		// ???
		g_SpatialSpace.remove	(this);
		g_SpatialSpace.insert	(this);
	} else {
		//*** we are not registered yet, or already unregistered
		//*** ignore request
	}
}

//////////////////////////////////////////////////////////////////////////
void			ISpatial_NODE::_init			(ISpatial_NODE* _parent)
{
	parent		=	_parent;
	children[0]	=	children[1]	=	children[2]	=	children[3]	=
	children[4]	=	children[5]	=	children[6]	=	children[7]	=	NULL;
	items.clear();
}

void			ISpatial_NODE::_insert			(ISpatial* S)			
{	
	S->spatial.node_ptr			= this;
	items.push_back				(S);
	g_SpatialSpace.stat_objects	++;
}

void			ISpatial_NODE::_remove			(ISpatial* S)			
{	
	S->spatial.node_ptr	= NULL;
	xr_vector<ISpatial*>::iterator	it = std::find(items.begin(),items.end(),S);
	VERIFY				(it!=items.end());
	items.erase			(it);
	g_SpatialSpace.stat_objects	--;
}

//////////////////////////////////////////////////////////////////////////
void			ISpatial_DB::initialize(Fbox& BB)
{
	Fvector bbc,bbd;
	BB.get_CD				(bbc,bbd);

	allocator_pool.reserve	(128);
	m_center.set			(bbc);
	m_bounds				= _max(_max(bbd.x,bbd.y),bbd.z);
	rt_insert_object		= NULL;
	m_root					= _node_create();
	m_root->_init			(NULL);
}
ISpatial_NODE*	ISpatial_DB::_node_create		()
{
	stat_nodes	++;
	if (allocator_pool.empty())			return allocator.create();
	else								
	{
		ISpatial_NODE*	N		= allocator_pool.back();
		allocator_pool.pop_back	();
		return			N;
	}
}
void			ISpatial_DB::_node_destroy(ISpatial_NODE* &P)
{
	stat_nodes	--;
	allocator_pool.push_back	(P);
	P							= NULL;
}

void			ISpatial_DB::_insert	(ISpatial_NODE* N, Fvector& n_C, float n_R)
{
	//*** we are assured that object lives inside our node
	float	n_vR	= 2*n_R;
	VERIFY	(N);
	VERIFY	(verify_sp(rt_insert_object,n_C,n_vR));

	// we have to make sure we aren't the leaf node
	if (n_R<=c_spatial_min)
	{
		// this is leaf node
		N->_insert									(rt_insert_object);
		rt_insert_object->spatial.node_center.set	(n_C);
		rt_insert_object->spatial.node_radius		= n_vR;		// vR
		return;
	}

	// we have to check if it can be putted further down
	float	s_R			= rt_insert_object->spatial.radius;		// spatial bounds
	float	c_R			= n_R/2;								// children bounds
	if (s_R<c_R)
	{
		// object can be pushed further down - select "octant", calc node position
		Fvector&	s_C					=	rt_insert_object->spatial.center;
		u32			octant				=	_octant	(n_C,s_C);
		Fvector		c_C;				c_C.mad	(n_C,c_spatial_offset[octant],c_R);
		VERIFY			(octant == _octant(n_C,c_C));				// check table assosiations
		ISpatial_NODE*	&chield			= N->children[octant];

		if (0==chield)	{
			chield			=	_node_create();
			VERIFY			(chield);
			chield->_init	(N);
			VERIFY			(chield);
		}
		VERIFY			(chield);
		_insert			(chield, c_C, c_R);
		VERIFY			(chield);
	}
	else
	{
		// we have to "own" this object (potentially it can be putted down sometimes...)
		N->_insert									(rt_insert_object);
		rt_insert_object->spatial.node_center.set	(n_C);
		rt_insert_object->spatial.node_radius		= n_vR;
	}
}

BOOL			f_valid					(float f)		{	return _finite(f) && !_isnan(f);	}
void			ISpatial_DB::insert		(ISpatial* S)
{
	stat_insert.Begin	();
	VERIFY				(f_valid(S->spatial.radius) && f_valid(S->spatial.center.x) && f_valid(S->spatial.center.y) && f_valid(S->spatial.center.z) );
	if (verify_sp(S,m_center,m_bounds))
	{
		// Object inside our DB
		rt_insert_object			= S;
		_insert						(m_root,m_center,m_bounds);
		VERIFY						(S->spatial_inside());
	} else {
		// Object outside our DB, put it into root node and hack bounds
		// Object will reinsert itself until fits into "real", "controlled" space
		m_root->_insert				(S);
		S->spatial.node_center.set	(m_center);
		S->spatial.node_radius		=	m_bounds;
	}
	stat_insert.End		();

	VERIFY		(verify());
}

void			ISpatial_DB::_remove	(ISpatial_NODE* N, ISpatial_NODE* N_sub)
{
	if (0==N)							return;

	//*** we are assured that node contains N_sub and this subnode is empty
	u32 octant	= u32(-1);
	if (N_sub==N->children[0])			octant = 0;
	else if (N_sub==N->children[1])		octant = 1;
	else if (N_sub==N->children[2])		octant = 2;
	else if (N_sub==N->children[3])		octant = 3;
	else if (N_sub==N->children[4])		octant = 4;
	else if (N_sub==N->children[5])		octant = 5;
	else if (N_sub==N->children[6])		octant = 6;
	else if (N_sub==N->children[7])		octant = 7;
	_node_destroy						(N->children[octant]);

	// Recurse
	if (N->_empty())					_remove(N->parent,N);
}

void			ISpatial_DB::remove		(ISpatial* S)
{
	stat_remove.Begin	();
	ISpatial_NODE* N	= S->spatial.node_ptr;
	N->_remove			(S);

	// Recurse
	if (N->_empty())					_remove(N->parent,N);
	stat_remove.End		();

	VERIFY		(verify());
}
