/************************************************************************

  MxFaceTree

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxFaceTree.cxx,v 1.12 2000/11/20 20:36:38 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxFaceTree.h"
#include "MxVector.h"

MxFaceCluster::MxFaceCluster()
    : MxFitFrame()
{
    parent = MXID_NIL;
    child[0] = child[1] = MXID_NIL;

    first_face = MXID_NIL;
    nfaces = 0;
}

ostream& MxFaceCluster::write(ostream& out,const uint *fmap,uint mapsize) const
{
    uint left = child[0],  right = child[1];
    if( left < mapsize )  left = fmap[left];
    if( right < mapsize ) right = fmap[right];

    out << "f^ " << left+1 << " " << right+1 << endl;

    out << "	fo "; mxv_write(out, origin(), 3) << endl;

    out << "	fe ";
    mxv_write(out, axis(0), 3) << " ";
    mxv_write(out, axis(1), 3) << " ";
    mxv_write(out, axis(2), 3) << endl;

    out << "	fd " << plane_offset() << endl;

    out << "	fn ";
    mxv_write(out, normal(), 3) << endl;

    out << "	fx ";
    mxv_write(out, min(), 3) << " ";
    mxv_write(out, max(), 3) << endl;

    return out;
}

static MxFaceTree *__input_tree = NULL;

bool MxFaceTree::smf_hook(char *cmd, int argc, char *argv[], MxStdModel& m)
{
    MxFaceTree *tree = __input_tree;


    if( streq(cmd, "f^") )
    {
	if( !tree )
	{
	    tree = new MxFaceTree(&m);
	    __input_tree = tree;
	}
	tree->merge_clusters(atoi(argv[0])-1, atoi(argv[1])-1);
    }
    else if( streq(cmd, "fo") )
    {
	MxFaceCluster& c = tree->cluster(tree->cluster_count()-1);
	for(uint i=0; i<3; i++)
	    c.origin()[i] = atof(argv[i]);
    }
    else if( streq(cmd, "fe") )
    {
	MxFaceCluster& c = tree->cluster(tree->cluster_count()-1);
	for(uint i=0; i<3; i++) for(uint j=0; j<3; j++)
	    c.axis(i)[j] = atof(argv[i*3 + j]);
    }
    else if( streq(cmd, "fd") )
    {
	MxFaceCluster& c = tree->cluster(tree->cluster_count()-1);
	c.plane_offset(atof(argv[0]));
    }
    else if( streq(cmd, "fn") )
    {
	MxFaceCluster& c = tree->cluster(tree->cluster_count()-1);
	for(uint i=0; i<3; i++)
	    c.normal()[i] = atof(argv[i]);
    }
    else if( streq(cmd, "fx") )
    {
	MxFaceCluster& c = tree->cluster(tree->cluster_count()-1);
	uint i;
	for(i=0; i<3; i++)
	    c.min()[i] = atof(argv[i]);
	for(uint j=0; j<3; j++)
	    c.max()[j] = atof(argv[i+j]);
    }
    else
	return false;

    return true;
}

MxFaceTree *MxFaceTree::acquire_last_read()
{
    MxFaceTree *tree = __input_tree;
    __input_tree = NULL;
    return tree;
}

MxFaceTree::MxFaceTree(MxStdModel *m0)
    : clusters(m0->face_count()),
      cluster_marks(m0->face_count()),
      cluster_flags(m0->face_count())
{
    m = m0;

    for(uint i=0; i<m->face_count(); i++)
    {
	add_cluster();

	clusters[i].first_face = i;
	clusters[i].nfaces = 1;

	double n[3];
	m->compute_face_normal(i, n);
	clusters[i].set_normal(n);
	clusters[i].compute_frame(m->corner(i, 0),
				  m->corner(i, 1),
				  m->corner(i, 2));
    }
}

uint MxFaceTree::add_cluster()
{
    clusters.add();
    uint id = clusters.last_id();
    cluster_marks.add();
    cluster_flags.add();

    cluster_marks[id] = 0x0;
    cluster_flags[id] = 0x0;

    return id;
}

uint MxFaceTree::find_root_cluster(uint id)
{
    while( !cluster_is_root(id) )
	id = clusters(id).parent;

    return id;
}

void MxFaceTree::mark_path_toroot(uint id, ushort mark)
{
    while( !cluster_is_root(id) )
    {
	cluster_mark(id, mark);
	id = clusters(id).parent;
    }
}

bool MxFaceTree::is_sub_cluster(uint base, uint target)
{
    uint id = base;

    do {
	if( id == target )
	    return true;
	id = clusters(id).parent;
    } while( id != MXID_NIL );

    return false;
}

uint MxFaceTree::maxdepth(uint id)
{
    if( id == MXID_NIL ) return 0;
    else
    {
	uint d1 = maxdepth(clusters(id).child[0]);
	uint d2 = maxdepth(clusters(id).child[1]);
	return MAX(d1, d2) + 1;
    }
}

uint MxFaceTree::mindepth(uint id)
{
    if( id == MXID_NIL ) return 0;
    else
    {
	uint d1 = maxdepth(clusters(id).child[0]);
	uint d2 = maxdepth(clusters(id).child[1]);
	return MIN(d1, d2) + 1;
    }
}

uint MxFaceTree::find_least_common_ancestor(uint id1, uint id2)
{
    if( id1 == MXID_NIL ) return id2;
    if( id2 == MXID_NIL ) return id1;

    mark_path_toroot(id2, 0);
    mark_path_toroot(id1, 1);

    while( !cluster_mark(id2) )   id2 = clusters(id2).parent;
    return id2;
}

bool MxFaceTree::compute_face_list(uint id)
{
    MxFaceCluster &current=cluster(id),
	&left=cluster(current.child[0]),
	&right=cluster(current.child[1]);

    current.nfaces = left.nfaces + right.nfaces;

    if( right.first_face == left.first_face + left.nfaces )
	current.first_face = left.first_face;
    else if( left.first_face == right.first_face + right.nfaces )
	current.first_face = right.first_face;
    else
    {
	current.first_face = MXID_NIL;
	return false;
    }

    return true;
}

uint MxFaceTree::merge_clusters(uint x, uint y)
{
    // Make sure we're dealing with top-level clusters
    x = find_root_cluster(x);
    y = find_root_cluster(y);
    SanityCheck(x != y);

    // Allocate and link a new cluster
    uint z = add_cluster();
    MxFaceCluster &current=cluster(z), &left=cluster(x), &right=cluster(y);

    current.child[0] = x;
    current.child[1] = y;
    left.parent = right.parent = z;

    compute_face_list(z);

    // Set up avg & total normals
    current.clear_normal();
    current.add_normal(left.total_normal());
    current.add_normal(right.total_normal());
    current.finalize_normal();

    // !!BUG: Set up the frame?

    return z;
}

// compute bounding boxes for all nodes in the tree
void MxFaceTree::compute_bboxes()
{
    uint i;
    
    // reset all bounding boxes
    for(i=0; i<cluster_count(); i++)
	cluster(i).reset_bounds();

    // for each leaf, propagate information up the tree
    for(uint leaf=0; leaf<m->face_count(); leaf++)
    {
	i = leaf;
	while( i!=MXID_NIL )
	{
	    cluster(i).accumulate_bounds(m->corner(leaf, 0));
	    cluster(i).accumulate_bounds(m->corner(leaf, 1));
	    cluster(i).accumulate_bounds(m->corner(leaf, 2));

	    i = cluster(i).parent;
	}
    }
}
