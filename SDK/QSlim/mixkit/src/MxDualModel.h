#ifndef MXDUALMODEL_INCLUDED // -*- C++ -*-
#define MXDUALMODEL_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxDualModel

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxDualModel.h,v 1.9 1999/07/13 17:14:34 garland Exp $

 ************************************************************************/

#include "MxStdModel.h"

typedef MxSizedDynBlock<uint, 3> MxDualEdgeList;

class MxDualEdge : public MxEdge
{
public:
    float border_length;	// Length of the border between clusters
};

class MxDualContraction
{
public:
    uint n1, n2;
    MxDualEdgeList dead_edges;
};

class MxDualModel
{
private:
    MxDynBlock<MxDualEdge> edges;		// All the edges of the model
    MxBlock<MxDualEdgeList> edge_links;		// Edge links for each node

    MxBlock<uint> node_labels;			// Each node is labeled
    MxBlock<uint> node_marks;			// Each node has a mark

    void update_border_lengths(const MxDualContraction&);
    
protected:
    MxStdModel *m;

    void collect_edges();

public:
    MxDualModel(MxStdModel *);


    ////////////////////////////////////////////////////////////////////////
    //  Fundamental operations
    //
    uint& node_label(uint i)       { return node_labels(i); }
    uint  node_label(uint i) const { return node_labels(i); }
	
    bool node_is_valid(uint i) const { return node_labels(i) == i; }
    uint node_ancestor(uint i) const
    {
	while(!node_is_valid(i))  i = node_label(i);
	return i;
    }
  	
    uint node_mark(uint i) const { return node_marks(i); }
    void node_mark(uint i, uint m) { node_marks(i) = m; }
    void mark_node_neighbors(uint n, uint mark);

    MxDualEdgeList&       node_edges(uint i)       { return edge_links(i); }
    const MxDualEdgeList& node_edges(uint i) const { return edge_links(i); }

    uint edge_count() const { return edges.length(); }
    MxDualEdge&       edge(uint i)       { return edges(i); }
    const MxDualEdge& edge(uint i) const { return edges(i); }

    ////////////////////////////////////////////////////////////////////////
    //  Dual contraction merges sets of faces
    //
    void contract(MxDualContraction&);

    ////////////////////////////////////////////////////////////////////////
    //  Calculations which tie dual graph to original mesh
    //
    bool meshedge_is_boundary(const MxEdge& e, uint n1);
    bool meshedge_is_boundary(const MxEdge& e, uint n1, uint n2);
    void compute_mesh_boundary(uint n1, MxEdgeList& edges);
    void compute_mesh_boundary(uint n1, uint n2, MxEdgeList& edges);
};

// MXDUALMODEL_INCLUDED
#endif
