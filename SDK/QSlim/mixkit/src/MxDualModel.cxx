/************************************************************************

  MxDualModel

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxDualModel.cxx,v 1.11 2000/11/20 20:36:38 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxDualModel.h"
#include "MxVector.h"

MxDualModel::MxDualModel(MxStdModel *m0)
    : node_labels(m0->face_count()),
      node_marks(m0->face_count()),
      edge_links(m0->face_count()), edges(3*m0->face_count()/2)
{
    m = m0;

    for(uint i=0; i<m->face_count(); i++)
    {
	node_labels[i] = i;
	node_marks[i] = 0x0;
    }

    collect_edges();
}

void MxDualModel::mark_node_neighbors(uint n,  uint mark)
{
    for(uint i=0; i<node_edges(n).length(); i++)
    {
	MxDualEdge& e = edge(node_edges(n)[i]);
	node_mark(e.opposite_vertex(n), mark);

    }
}

void MxDualModel::collect_edges()
{
    MxVertexList star;
    MxFaceList faces;

    for(uint i=0; i<m->vert_count(); i++)
    {
	star.reset();
	m->collect_vertex_star(i, star);

	for(uint j=0; j<star.length(); j++)
	    if( i < star[j] )
	    {
		faces.reset();
		m->collect_edge_neighbors(i, star[j], faces);
		if( faces.length() == 2 )
		{
		    edges.add();
		    uint edge_id = edges.last_id();
		    MxDualEdge& edge = edges[edge_id];

		    edge.v1 = faces[0];
		    edge.v2 = faces[1];
		    SanityCheck(edge.v1 != edge.v2);
		    node_edges(edge.v1).add(edge_id);
		    node_edges(edge.v2).add(edge_id);
		    edge.border_length =
			sqrt(mxv_L2(m->vertex(i), m->vertex(star[j]), 3));
		}
		else if( faces.length() > 2 )
		    mxmsg_signal(MXMSG_NOTE, "Ignoring non-manifold edge");
		// ELSE: ignore boundary edges
	    }
    }
}

void MxDualModel::update_border_lengths(const MxDualContraction& conx)
{
    uint v1 = conx.n1;

    for(uint i=0; i<conx.dead_edges.length(); i++)
    {
	uint target = conx.dead_edges[i];
	const MxDualEdge& dead = edge(target);
	uint o = dead.v1;
	if( o==v1 ) continue;

	for(uint j=0; j<node_edges(v1).length(); j++)
	{
	    uint k = node_edges(v1)[j];

	    if( edge(k).opposite_vertex(v1) == o )
		edge(k).border_length += dead.border_length;
	}
    }
}

void MxDualModel::contract(MxDualContraction& conx)
{
    uint n1 = conx.n1;
    uint n2 = conx.n2;
    conx.dead_edges.reset();

    SanityCheck( node_is_valid(n1) );
    SanityCheck( node_is_valid(n2) );

    node_labels(n2) = n1;

    mark_node_neighbors(n2, 0);		// unmark all neighbors of n2
    node_mark(n1, 1);			// mark n1 and
    mark_node_neighbors(n1, 1);		// all its neighbors

    // Relink/remove all edges of n2
    //
    for(uint i=0; i<node_edges(n2).length(); i++)
    {
	uint e_id = node_edges(n2)[i];
	MxDualEdge& e = edge(e_id);
	uint o = e.opposite_vertex(n2);

	if( !node_mark(o) )
	{
	    // Non-duplicate edge -- relink to n1
	    e.v1 = n1;
	    e.v2 = o;
	    node_edges(n1).add(e_id);
	}
	else
	{
	    // Duplicate edge -- remove it
	    uint j;
	    varray_find(node_edges(o), e_id, &j);
	    node_edges(o).remove(j);
	    e.v1 = o;
	    e.v2 = MXID_NIL;
	    conx.dead_edges.add(e_id);
	}
    }

    node_edges(n2).reset();

    update_border_lengths(conx);
}

bool MxDualModel::meshedge_is_boundary(const MxEdge& e, uint n1)
{
    MxFaceList faces;
    m->collect_edge_neighbors(e.v1, e.v2, faces);
    if( !faces.length() ) return false;

    uint root = node_ancestor(n1);
    uint count = 0;

    for(uint i=0; i<faces.length(); i++)
        if( root==node_ancestor(faces[i]) )
	    count++;

    return count>0 && count!=faces.length();
}

bool MxDualModel::meshedge_is_boundary(const MxEdge& e, uint n1, uint n2)
{
    MxFaceList faces;
    m->collect_edge_neighbors(e.v1, e.v2, faces);
    if( !faces.length() ) return false;

    uint root1 = node_ancestor(n1);
    uint root2 = node_ancestor(n2);

    uint count1 = 0;
    uint count2 = 0;

    for(uint i=0; i<faces.length(); i++)
    {
        uint r = node_ancestor(faces[i]);

        if( r==root1 )
            count1++;
        else if( r==root2 )
            count2++;
        else
            return false;
    }

    return count1>0 && count2>0;
}

void MxDualModel::compute_mesh_boundary(uint n1, MxEdgeList& edges)
{
    edges.reset();

    // !!BUG: Yes, visiting every edge is wasteful.  But it was easier to
    //        cut, paste, and massage the appropriate behavior for a quick
    //        result.  Fix this later!
    //
    MxVertexList star;

    for(MxVertexID i=0; i<m->vert_count(); i++)
    {
        star.reset();
        m->collect_vertex_star(i, star);

        for(uint j=0; j<star.length(); j++)
            if( i < star[j] )
            {
                MxEdge e(i, star[j]);

                if( meshedge_is_boundary(e, n1) )
                    edges.add(e);
            }
    }
}

void MxDualModel::compute_mesh_boundary(uint n1, uint n2, MxEdgeList& edges)
{
    edges.reset();

    // !!BUG: Yes, visiting every edge is wasteful.  But it was easier to
    //        cut, paste, and massage the appropriate behavior for a quick
    //        result.  Fix this later!
    //
    MxVertexList star;

    for(MxVertexID i=0; i<m->vert_count(); i++)
    {
        star.reset();
        m->collect_vertex_star(i, star);

        for(uint j=0; j<star.length(); j++)
            if( i < star[j] )
            {
                MxEdge e(i, star[j]);

                if( meshedge_is_boundary(e, n1, n2) )
                    edges.add(e);
            }
    }
}
