#ifndef MXQSLIM_INCLUDED // -*- C++ -*-
#define MXQSLIM_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Surface simplification using quadric error metrics

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxQSlim.h,v 1.29 1999/02/18 15:26:07 garland Exp $

 ************************************************************************/

#include "MxStdSlim.h"
#include "MxQMetric3.h"

class MxQSlim : public MxStdSlim
{
protected:
    MxBlock<MxQuadric3> quadrics;

    void discontinuity_constraint(MxVertexID, MxVertexID, const MxFaceList&);
    void collect_quadrics();
    void transform_quadrics(const Mat4&);
    void constrain_boundaries();

public:

    Mat4 *object_transform;

public:
    MxQSlim(MxStdModel&);
    virtual ~MxQSlim() { }

    virtual void initialize();

    const MxQuadric3& vertex_quadric(MxVertexID v) { return quadrics(v); }
};

class MxQSlimEdge : public MxEdge, public MxHeapable
{
public:
    float vnew[3];
};

class MxEdgeQSlim : public MxQSlim
{
private:
    typedef MxSizedDynBlock<MxQSlimEdge*, 6> edge_list;

    MxBlock<edge_list> edge_links;

    //
    // Temporary variables used by methods
    MxVertexList star, star2;
    MxPairContraction conx_tmp;

protected:
    double check_local_compactness(uint v1, uint v2, const float *vnew);
    double check_local_inversion(uint v1, uint v2, const float *vnew);
    uint check_local_validity(uint v1, uint v2, const float *vnew);
    uint check_local_degree(uint v1, uint v2, const float *vnew);
    void apply_mesh_penalties(MxQSlimEdge *);
    void create_edge(MxVertexID i, MxVertexID j);
    void collect_edges();

    void compute_target_placement(MxQSlimEdge *);
    void finalize_edge_update(MxQSlimEdge *);

    virtual void compute_edge_info(MxQSlimEdge *);
    virtual void update_pre_contract(const MxPairContraction&);
    virtual void update_post_contract(const MxPairContraction&);
    virtual void update_pre_expand(const MxPairContraction&);
    virtual void update_post_expand(const MxPairContraction&);

public:
    MxEdgeQSlim(MxStdModel&);
    virtual ~MxEdgeQSlim();

    void initialize();
    void initialize(const MxEdge *edges, uint count);
    bool decimate(uint target, float);

    void apply_contraction(const MxPairContraction& conx);
    void apply_expansion(const MxPairContraction& conx);

    uint edge_count() const { return heap.size(); }
    const MxQSlimEdge *edge(uint i) const {return (MxQSlimEdge *)heap.item(i);}

public:
    void (*contraction_callback)(const MxPairContraction&, float);
};

class MxFaceQSlim : public MxQSlim
{
private:
    class tri_info : public MxHeapable
    {
    public:
	MxFaceID f;
	float vnew[3];
    };

    MxBlock<tri_info> f_info;

protected:
    void compute_face_info(MxFaceID);


public:
    MxFaceQSlim(MxStdModel&);

    void initialize();
    bool decimate(uint target, float);
};

// MXQSLIM_INCLUDED
#endif
