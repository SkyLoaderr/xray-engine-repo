/************************************************************************

  MxDualSlim

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxDualSlim.cxx,v 1.13 2000/03/09 18:08:26 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxDualSlim.h"
#include "MxVector.h"

typedef MxQuadric3 Quadric;

static inline Quadric corner_quadric(MxStdModel *m, uint f, uint i)
	{ return Quadric(m->corner(f, i), 1); }
static inline Quadric corner_quadric(MxStdModel *m, uint v)
	{ return Quadric(m->vertex(v), 1); }

MxDualSlim::MxDualSlim(MxStdModel *m0, MxDualModel *d0, MxFaceTree *t0)
    : heap(d0->edge_count()),
      face_nodes(m0->face_count()),
      target_edges(d0->edge_count())
{
    dual = d0;
    tree = t0;
    m = m0;

    weight_by_area = true;
    shape_policy = MX_SHAPE_ANY;
    root_cluster_count = tree->cluster_count();
    will_maintain_bounds = false;
}

void MxDualSlim::initialize_node(uint i)
{
    MxDualSlimNode& n = node_info(i);

    n.perimeter = m->compute_face_perimeter(i);
    n.faces.add(i);

    Vec3 avg_normal;
    m->compute_face_normal(i, avg_normal);

    if( !weight_by_area )
	n.Q_dir = Quadric(avg_normal, -1, 1);
    else
    {
	n.Q_dir = Quadric(avg_normal, -1, m->compute_face_area(i));
	n.Q_dir *= sqrt(n.Q_dir.area());
    }

    n.Q_fit  = corner_quadric(m, i, 0);
    n.Q_fit += corner_quadric(m, i, 1);
    n.Q_fit += corner_quadric(m, i, 2);
    n.nverts = 3;  // Must match the number of quadrics
}

void MxDualSlim::update_frame_bounds(MxFaceCluster& c,const MxFaceList& faces)
{
    for(uint i=0; i<faces.length(); i++)
    {
	c.accumulate_bounds(m->corner(faces[i], 0));
	c.accumulate_bounds(m->corner(faces[i], 1));
	c.accumulate_bounds(m->corner(faces[i], 2));
    }

    // Alternative (inferior but quicker) formulation
    // c.accumulate_bounds(c.child[0]);
    // c.accumulate_bounds(c.child[1]);
}

void MxDualSlim::update_node_bounds(uint id)
{
    MxDualSlimNode& n = node_info(id);
    MxFaceCluster& c = tree->cluster(tree->find_root_cluster(id));

    c.reset_bounds();
    update_frame_bounds(c, n.faces);
}

void MxDualSlim::initialize()
{
    uint i;

    for(i=0; i<m->face_count(); i++)
    {
	MxDualSlimNode& n = node_info(i);

	initialize_node(i);
	// NOTE: The MxFaceTree constructor has already filled in the
	//       cluster slots for the initial clusters.
    }

    // Construct the initial heap
    for(i=0; i<dual->edge_count(); i++)
    {
	MxDualSlimEdge *t = get_edge(i);
	t->id = i;
	compute_edge_info(t);
    }
}

void MxDualSlim::compute_edge_info(MxDualSlimEdge *e)
{
    MxDualEdge& edge = dual->edge(e->id);
    MxDualSlimNode& n1 = node_info(edge.v1);
    MxDualSlimNode& n2 = node_info(edge.v2);
    MxFaceCluster& c1 = tree->cluster(tree->find_root_cluster(edge.v1));
    MxFaceCluster& c2 = tree->cluster(tree->find_root_cluster(edge.v2));
    double cost = 0.0;

    MxFaceCluster new_frame;
    //
    // Set up the child links
    new_frame.child[0] = edge.v1;
    new_frame.child[1] = edge.v2;
    //
    // Set up the average normal of the proposed frame
    new_frame.clear_normal(); new_frame.add_normal(c1.total_normal());
    new_frame.add_normal(c2.total_normal()); new_frame.finalize_normal();

    //////////////////////////////////////////////////////////////////
    // Measure the planarity error
    //
    Quadric Q_fit = n1.Q_fit;
    Q_fit += n2.Q_fit;

    if( new_frame.compute_frame(Q_fit, n1.nverts + n2.nverts) )
    {
	Mat3 A = Q_fit.tensor();
	Vec3 v = Q_fit.vector();
	double k = Q_fit.offset();
	double d = new_frame.plane_offset();
	Vec3 n(new_frame.axis(MXFRAME_NORMAL));

	double fit_cost = n*(A*n) + 2*d*(v*n) + d*d*k;
	if( weight_by_area )
	    fit_cost /= Q_fit.area();

 	cost += fit_cost;
    }
    else
    {
	// !!BUG: Formulate a fallback policy for when the eigenvalues don't
	//        exist.
	fatal_error("BUG -- Can't proceed without valid frame.");
    }


    //////////////////////////////////////////////////////////////////
    // Measure the orientation error
    //
    Quadric Q_dir = n1.Q_dir;
    Q_dir += n2.Q_dir;

    if( weight_by_area )
	cost += Q_dir(new_frame.normal()) / Q_dir.area();
    else
	cost += Q_dir(new_frame.normal());


    //////////////////////////////////////////////////////////////////
    // Shape heuristics
    //
    if( shape_policy == MX_SHAPE_REGULAR )
    {
	// !!ASSUME: area weighting; we can rip the area out of Q_dir
	//
	SanityCheck(weight_by_area);
	double ir1 = n1.perimeter * n1.perimeter / n1.Q_dir.area();
	double ir2 = n2.perimeter * n2.perimeter / n2.Q_dir.area();

	SanityCheck(edge.border_length >= 0);
	double new_perim = n1.perimeter + n2.perimeter - 2*edge.border_length;
	double ir_new = new_perim*new_perim/Q_dir.area();

	double ir_cost, ir_old = MAX(ir1, ir2);
	if( FEQ(ir_new, 0.0) ) ir_cost = 0.0;
	else
	    // ir_cost = (1 + (ir_new - ir_old)/ir_new) * 0.5;
	    ir_cost =  (ir_new - ir_old)/ir_new;

	cost += ir_cost;
    }

    //////////////////////////////////////////////////////////////////
    // Having computed the cost, update this edge in the heap
    //
    e->heap_key(-cost);
    if(e->is_in_heap())
	heap.update(e);
    else
	heap.insert(e);
}

void MxDualSlim::contract(MxDualSlimEdge *e)
{
    MxDualEdge& edge = dual->edge(e->id);
    if( edge.v1 == MXID_NIL && edge.v2 == MXID_NIL ) return;

    MxDualSlimNode& n1 = node_info(edge.v1);
    MxDualSlimNode& n2 = node_info(edge.v2);
    uint i;

    n1.Q_dir += n2.Q_dir;
    n1.Q_fit += n2.Q_fit;
    n1.nverts += n2.nverts;
    n1.perimeter += n2.perimeter - 2*edge.border_length;

    // ??TODO: Remove duplicate quadrics from Q_fit

    // update face list
    for(i=0; i<n2.faces.length(); i++)
	n1.faces.add(n2.faces[i]);

    MxDualContraction conx;
    conx.n1 = edge.v1;
    conx.n2 = edge.v2;
    dual->contract(conx);
    uint new_cluster = tree->merge_clusters(conx.n1, conx.n2);
    root_cluster_count--;

    if( !tree->cluster(new_cluster).compute_frame(n1.Q_fit, n1.nverts) )
    {
	//!!BUG: Again, we need a fallback policy for finding a frame
	fatal_error("BUG -- Can't proceed without valid frame.");
    }

    if( will_maintain_bounds )
	update_node_bounds(conx.n1);

    // remove dead edges from heap
    for(i=0; i<conx.dead_edges.length(); i++)
	heap.remove(get_edge(conx.dead_edges[i]));

    // recompute edge info for all edges linked to n1
    for(i=0; i<dual->node_edges(conx.n1).length(); i++)
	compute_edge_info(get_edge(dual->node_edges(conx.n1)[i]));
}

bool MxDualSlim::aggregate(uint target)
{
    while( root_cluster_count > target )
    {
	MxDualSlimEdge *e = (MxDualSlimEdge *)heap.extract();
	if( !e ) return false;

	MxDualEdge& edge = dual->edge(e->id);
	SanityCheck( dual->node_is_valid(edge.v1) );
	SanityCheck( dual->node_is_valid(edge.v2) );

	contract(e);
    }

    return true;
}

bool MxDualSlim::limit_aggregate(float maxerr)
{
    maxerr = -maxerr;		// The heap uses negative error limits

    while( heap.top() && heap.top()->heap_key() > maxerr )
    {
	MxDualSlimEdge *e = (MxDualSlimEdge *)heap.extract();
	SanityCheck( e );

	MxDualEdge& edge = dual->edge(e->id);
	SanityCheck( dual->node_is_valid(edge.v1) );
	SanityCheck( dual->node_is_valid(edge.v2) );

	contract(e);
    }

    if( !heap.top() ) return false;

    return true;
}
