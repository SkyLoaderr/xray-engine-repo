#include "stdafx.h"
#include "ispatial.h"

static	Fvector	c_spatial_offset	[8]	= 
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
ISpatial::ISpatial(void)
{
	spatial_center.set		(0,0,0);
	spatial_radius			= 0;
	spatial_node_center.set	(0,0,0);
	spatial_node_radius		= 0;
	spatial_node_ptr		= NULL;
	spatial_sector			= NULL;
}

ISpatial::~ISpatial(void)
{
}

BOOL	ISpatial::spatial_inside()
{
	float	dr	= - spatial_node_radius + spatial_radius;
	if (spatial_center.x < spatial_node_center.x - dr)	return FALSE;
	if (spatial_center.x > spatial_node_center.x + dr)	return FALSE;
	if (spatial_center.y < spatial_node_center.y - dr)	return FALSE;
	if (spatial_center.y > spatial_node_center.y + dr)	return FALSE;
	if (spatial_center.z < spatial_node_center.z - dr)	return FALSE;
	if (spatial_center.z > spatial_node_center.z + dr)	return FALSE;
	return TRUE;
}

void	ISpatial::spatial_move	()
{
	//*** somehow it was determined that object has been moved
	//*** check if we are supposed to correct it's spatial location
	VERIFY		(spatial_node_ptr);
	if			(spatial_inside())	return;
	DB->remove	(this);
	DB->insert	(this);
}

//////////////////////////////////////////////////////////////////////////
void			ISpatial_NODE::_init			(ISpatial_NODE* _parent)				
{
	parent		=	_parent;
	children[0]	=	children[1]	=	children[2]	=	children[3]	=
	children[4]	=	children[5]	=	children[6]	=	children[7]	=	NULL;
}

void			ISpatial_NODE::_insert			(ISpatial* S)			
{	
	S->spatial_node_ptr	= this;
	items.push_back		(S);										
}

void			ISpatial_NODE::_remove			(ISpatial* S)			
{	
	S->spatial_node_ptr	= NULL;
	items.erase			(std::find(items.begin(),items.end(),S));	
}

//////////////////////////////////////////////////////////////////////////
ISpatial_NODE*	ISpatial_DB::_node_create		()
{
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
	allocator_pool.push_back	(P);
	P							= NULL;
}

void			ISpatial_DB::_insert	(ISpatial_NODE* N, Fvector& n_center, float n_radius)
{
	//*** we are assured that object lives inside our node

	// we have to make sure we aren't the leaf node
	if (n_radius<=c_spatial_min)
	{
		// this is leaf node
		N->_insert									(rt_insert_object);
		rt_insert_object->spatial_node_center.set	(n_center);
		rt_insert_object->spatial_node_radius		= n_radius;
		return;
	}

	// we have to check if it can be putted further down
	float	s_bounds	= rt_insert_object->spatial_radius;		// spatial bounds
	float	c_bounds	= n_radius/2;	// children bounds
	if (s_bounds<=c_bounds)
	{
		// object can be pushed further down - select "octant", calc node position
		Fvector&	s_center			=	rt_insert_object->spatial_center;
		u32			octant				=	_octant(s_center.x<n_center.x?0:1,s_center.y<n_center.y?0:1,s_center.z<n_center.z?0:1);
		Fvector		c_center;			c_center.mad	(n_center,c_spatial_offset[octant],n_radius/4.f);
		if (0==N->children[octant])		{
			N->children[octant]			=	_node_create();
			N->children[octant]->_init	(N);
		}
		_insert							(N->children[octant], c_center, c_bounds);
	}
	else
	{
		// we have to "own" this object (potentially it can be putted down sometimes...)
		N->_insert									(rt_insert_object);
		rt_insert_object->spatial_node_center.set	(n_center);
		rt_insert_object->spatial_node_radius		= n_radius;
	}
}

void			ISpatial_DB::insert		(ISpatial* S)
{
	rt_insert_object	= S;
	_insert				(root,v_center,f_bounds);
}

void			ISpatial_DB::_remove	(ISpatial_NODE* N, ISpatial_NODE* N_sub)
{
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
	ISpatial_NODE* N	= S->spatial_node_ptr;
	N->_remove			(S);

	// Recurse
	if (N->_empty())					_remove(N->parent,N);
}
