#ifndef MXDUALSLIM_INCLUDED // -*- C++ -*-
#define MXDUALSLIM_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxDualSlim

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxDualSlim.h,v 1.6 2000/03/09 18:00:54 garland Exp $

 ************************************************************************/

#include "MxDualModel.h"
#include "MxFaceTree.h"
#include "MxQMetric3.h"
#include "MxHeap.h"

#define MX_SHAPE_ANY 0
#define MX_SHAPE_REGULAR 1

class MxDualSlimEdge : public MxHeapable { public: uint id; };

class MxDualSlimNode
{
public:
    MxQuadric3 Q_dir;		// Error form for normal orientation
    MxQuadric3 Q_fit;		// Error form for planarity

    MxFaceList faces;		// All the faces in this cluster

    float perimeter;		// The length of the cluster's perimeter
    uint nverts;		// The number of vertices in this cluster
};

class MxDualSlim
{
private:
    MxHeap heap;
    MxBlock<MxDualSlimEdge> target_edges;
    MxBlock<MxDualSlimNode> face_nodes;

protected:
    MxStdModel *m;
    MxDualModel *dual;
    MxFaceTree *tree;

private:
    void initialize_node(uint);

protected:
    MxDualSlimEdge       *get_edge(uint i)       { return &target_edges(i); }
    const MxDualSlimEdge *get_edge(uint i) const { return &target_edges(i); }

    void update_frame_bounds(MxFaceCluster&, const MxFaceList&);
    void update_node_bounds(uint);
    void compute_edge_info(MxDualSlimEdge *e);
    void contract(MxDualSlimEdge *e);

public:
    bool weight_by_area;
    uint shape_policy;
    uint root_cluster_count;
    bool will_maintain_bounds;

public:
    MxDualSlim(MxStdModel *m0, MxDualModel *d0, MxFaceTree *t0);

    MxDualSlimNode&       node_info(uint i)       { return face_nodes(i); }
    const MxDualSlimNode& node_info(uint i) const { return face_nodes(i); }

    void initialize();
    bool aggregate(uint target);
    bool limit_aggregate(float maxerr);
};




// MXDUALSLIM_INCLUDED
#endif
