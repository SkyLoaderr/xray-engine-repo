#ifndef MXPROPSLIM_INCLUDED // -*- C++ -*-
#define MXPROPSLIM_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxPropSlim

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxPropSlim.h,v 1.6 1998/10/26 21:09:15 garland Exp $

 ************************************************************************/

#include "MxStdSlim.h"
#include "MxQMetric.h"


class MxPropSlim : public MxStdSlim
{
private:
    uint D;

    bool use_color;
    bool use_texture;
    bool use_normals;

    class edge_info : public MxHeapable
    {
    public:
	MxVertexID v1, v2;
	MxVector target;

	edge_info(uint D) : target(D) { }
    };
    typedef MxSizedDynBlock<edge_info*, 6> edge_list;


    MxBlock<edge_list> edge_links;	// 1 per vertex
    MxBlock<MxQuadric*> __quadrics;	// 1 per vertex

    //
    // Temporary variables used by methods
    MxVertexList star, star2;
    MxPairContraction conx_tmp;

protected:
    uint compute_dimension(MxStdModel *);
    void pack_to_vector(MxVertexID, MxVector&);
    void unpack_from_vector(MxVertexID, MxVector&);
    uint prop_count();
    void pack_prop_to_vector(MxVertexID, MxVector&, uint);
    void unpack_prop_from_vector(MxVertexID, MxVector&, uint);

    void compute_face_quadric(MxFaceID, MxQuadric&);
    void collect_quadrics();

    void create_edge(MxVertexID, MxVertexID);
    void collect_edges();
    void constrain_boundaries();
    void discontinuity_constraint(MxVertexID, MxVertexID, const MxFaceList&);
    void compute_edge_info(edge_info *);
    void finalize_edge_update(edge_info *);
    void compute_target_placement(edge_info *);

    void apply_contraction(const MxPairContraction&, edge_info *);
    void update_pre_contract(const MxPairContraction&);

public:
    bool will_decouple_quadrics;

public:
    MxPropSlim(MxStdModel *);

    uint dim() const { return D; }

    void consider_color(bool will=true);
    void consider_texture(bool will=true);
    void consider_normals(bool will=true);

    uint quadric_count() const { return __quadrics.length(); }
    MxQuadric&       quadric(uint i)       { return *(__quadrics(i)); }
    const MxQuadric& quadric(uint i) const { return *(__quadrics(i)); }


    void initialize();
    bool decimate(uint);

};

// MXPROPSLIM_INCLUDED
#endif
