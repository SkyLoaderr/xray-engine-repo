/************************************************************************

  Part 2 of the MxFaceTree package -- Distance Queries

  The code in this file implements the methods for performing distance
  queries on face trees.  Since this code is moderately large, and not
  always necessary, it's here in its very own file.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxFaceTree-2.cxx,v 1.2 1998/10/26 21:08:50 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxFaceTree.h"
#include "MxHeap.h"
#include "MxGeom3D.h"

class PendingProbe : public MxHeapable
{
public:
    uint id;
    double min, max;

    PendingProbe(uint i) { id=i; }
};

void MxFaceProbe::clear()
{
    id = MXID_NIL;
    dist = HUGE;

    upper_bound = HUGE;
    lower_bound = 0;

    visited = leaf = internal = 0;
}

double MxFaceTree::exhaustive_min_sqrdistance(const double *v,
					      MxFaceProbe *hit)
{
    hit->clear();

    for(uint i=0; i<m->face_count(); i++)
    {
	if( m->face_is_valid(i) )
	{
	    double d2 =  triangle_project_point(Vec3(m->corner(i, 0)),
						Vec3(m->corner(i, 1)),
						Vec3(m->corner(i, 2)),
						Vec3(v));
	    if( d2 < hit->dist )
	    {
		hit->dist = d2;
		hit->id = i;
	    }
	}
    }

    hit->visited = m->face_count();
    hit->leaf = hit->visited;
    hit->internal = 0;

    return hit->dist;
}

void MxFaceTree::bound_cluster_sqrdistance(uint target, const double *v,
					   double *min, double *max)
{
    if( cluster_is_leaf(target) )
    {
	// For leaves, we compute the EXACT distance of v to
	// the target face.
	//
	double d = triangle_project_point(Vec3(m->corner(target, 0)),
					  Vec3(m->corner(target, 1)),
					  Vec3(m->corner(target, 2)),
					  Vec3(v));
	*min = *max = d;
    }
    else
    {
	// For multi-face clusters, we need to come up with a conservative
	// estimate of the distance.  The criterion is that the distance d
	// to any surface point in the cluster be bounded by [min,max].
	// In other words, min<=d<=max.
	//
	*min = cluster(target).compute_closest_sqrdistance(v);
	*max = cluster(target).compute_max_sqrdistance(v);
    }
}

double MxFaceTree::find_min_sqrdistance(const double *v,
					MxFaceProbe *hit,
					uint root)
{
    MxHeap heap(100);

    if( root==MXID_NIL )  root = find_root_cluster(0);
    hit->clear();

    // Start things out at the root
    //
    PendingProbe *probe = new PendingProbe(root);
    bound_cluster_sqrdistance(probe->id, v, &probe->min, &probe->max);
    //
    // This provides us with our first bounds estimates.
    hit->lower_bound = probe->min;
    hit->upper_bound = probe->max;

    while( probe )
    {
	hit->visited++;
	uint target = probe->id;

	if( probe->max < hit->upper_bound ) hit->upper_bound = probe->max;

	if( cluster_is_leaf(target) )
	{
	    SanityCheck( target < m->face_count() );
	    hit->leaf++;

	    double d = probe->min; // NOTE: This is exact for leaves
	    if( d < hit->dist )
	    {
		hit->dist = d;
		hit->id = target;
	    }
	}
	else if( probe->min < hit->upper_bound )
	{
	    // Since this node isn't beyond the upper bound, we need
	    // to examine things further by expanding its children.
	    hit->internal++;

	    PendingProbe *child;

	    child = new PendingProbe(cluster(target).child[0]);
	    bound_cluster_sqrdistance(child->id, v, &child->min, &child->max);
	    heap.insert(child, -child->min);

	    child = new PendingProbe(cluster(target).child[1]);
	    bound_cluster_sqrdistance(child->id, v, &child->min, &child->max);
	    heap.insert(child, -child->min);
	}

	delete probe;

	// If we've necessarily found the closest face, ignore the rest
	// of the possibilities.
	//
	if( hit->dist < ((PendingProbe *)heap.top())->min )
	    break;

	probe = (PendingProbe *)heap.extract();
    }

    // Make sure any remaining entries in the heap are freed.
    //
    for(uint i=0; i<heap.size(); i++)
	delete ((PendingProbe*)heap.item(i));

    return hit->dist;
}
