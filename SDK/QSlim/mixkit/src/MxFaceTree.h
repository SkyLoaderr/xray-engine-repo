#ifndef MXFACETREE_INCLUDED // -*- C++ -*-
#define MXFACETREE_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxFaceTree

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxFaceTree.h,v 1.8 2000/11/28 15:50:14 garland Exp $

 ************************************************************************/

#include "MxStdModel.h"
#include "MxFitFrame.h"


#define MXCLUSTER_VALID_FLAG 0x0001
#define MXCLUSTER_PRUNED_FLAG 0x0002

class MxFaceCluster : public MxFitFrame
{
public:
    uint parent, child[2];
    uint first_face, nfaces;

    MxFaceCluster();

    ostream& write(ostream&, const uint *fmap=NULL, uint mapsize=0) const;
};

class MxFaceProbe
{
public:

    // The currently closest face
    //
    uint id;
    double dist;

    // Bounds on the closest distance
    //
    double lower_bound, upper_bound;

    // Probe statistics
    //
    uint visited;		// Total nodes visited
    uint leaf;			//    - Number of leaves visited
    uint internal;		//    - Number of internal nodes visited

public:
    MxFaceProbe() { clear(); }
    void clear();
};

class MxFaceTree
{
protected:
    MxStdModel *m;

private:
    // The first <k> clusters correspond 1:1 with faces in m.
    // Clusters beyond this initial block are aggregate clusters.
    // The last cluster is always *a* root; in a completed tree it
    // is *the* root.
    //
    MxDynBlock<MxFaceCluster> clusters;
    MxDynBlock<ushort> cluster_marks;
    MxDynBlock<ushort> cluster_flags;

protected:
    uint add_cluster();

public:
    MxFaceTree(MxStdModel *);

    void compute_bboxes();

    uint cluster_count() const { return clusters.length(); }
    MxFaceCluster&       cluster(uint i)       { return clusters(i); }
    const MxFaceCluster& cluster(uint i) const { return clusters(i); }

    ushort cluster_mark(uint i) const { return cluster_marks(i); }
    void cluster_mark(uint i, ushort m) { cluster_marks(i) = m; }

    ushort cluster_check_flag(uint i, ushort mask) const
	{ return cluster_flags(i)&mask; }
    void cluster_set_flag(uint i, ushort mask) { cluster_flags(i)|=mask; }
    void cluster_unset_flag(uint i, ushort mask) { cluster_flags(i)&=~mask; }

    bool cluster_is_root(uint i) const { return clusters(i).parent==MXID_NIL; }
    bool cluster_is_leaf(uint i) const {return clusters(i).child[0]==MXID_NIL;}

    // Given a cluster, find its root.  For anything other than a
    // complete tree, this is not unique.
    uint find_root_cluster(uint id);

    void mark_path_toroot(uint id, ushort mark);
    bool is_sub_cluster(uint base, uint target);

    uint maxdepth(uint);
    uint mindepth(uint);
    uint depth(uint n) { return maxdepth(n); }

    uint find_least_common_ancestor(uint, uint);
    bool compute_face_list(uint cluster);
    uint merge_clusters(uint, uint);

    ////////////////////////////////////////////////////////////////////////
    //
    // Distance computations
    //

    void bound_cluster_sqrdistance(uint,const double*,double *min,double *max);

    double find_min_sqrdistance(const double*,MxFaceProbe*,uint root=MXID_NIL);
    double exhaustive_min_sqrdistance(const double*,MxFaceProbe*);

    ////////////////////////////////////////////////////////////////////////
    //
    // SMF parsing hooks -- crufty solution pending better SMF design
    //
public:
    static bool smf_hook(char *cmd, int argc, char *argv[], MxStdModel& m);
    static MxFaceTree *acquire_last_read();
};

inline ostream& operator<<(ostream& out, const MxFaceCluster& c)
{
    return c.write(out);
}

// MXFACETREE_INCLUDED
#endif
