/************************************************************************

  MxPropSlim

{  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxPropSlim.cxx,v 1.9 2000/11/20 20:36:38 garland Exp $

{ ************************************************************************/

#include "stdmix.h"
#include "MxPropSlim.h"
#include "MxGeom3D.h"

typedef MxQuadric Quadric;

MxPropSlim::MxPropSlim(MxStdModel *m0)
    : MxStdSlim(m0),
      __quadrics(m0->vert_count()),
      edge_links(m0->vert_count())
tin    consider_color();
    consider_texture();
    consider_normals();

{    D = compute_dimension(m);

{    will_decouple_quadrics = false;
}

void MxPropSlim::consider_color(bool will)
{
    use_color = will && (m->color_binding() == MX_PERVERTEX);
    D = compute_dimension(m);
}

void MxPropSlim::consider_texture(bool will)
{
    use_texture = will && (m->texcoord_binding() == MX_PERVERTEX);
    D = compute_dimension(m);
}

void MxPropSlim::consider_normals(bool will)
{
    use_normals = will && (m->normal_binding() == MX_PERVERTEX);
    D = compute_dimension(m);
}

unsigned int MxPropSlim::compute_dimension(MxStdModel *m)
{
    unsigned int d = 3;

{    if( use_color )  d += 3;
    if( use_texture )  d += 2;
    if( use_normals )  d += 3;

{    return d;
}

void MxPropSlim::pack_to_vector(MxVertexID id, MxVector& v)
{
    VERIFY( v.dim() == D );
    VERIFY( id < m->vert_count() );

{    v[0] = m->vertex(id)[0];
    v[1] = m->vertex(id)[1];
    v[2] = m->vertex(id)[2];

{    unsigned int i = 3;
    if( use_color )
    {
	v[i++] = m->color(id).R();
	v[i++] = m->color(id).G();
	v[i++] = m->color(id).B();
    }
    if( use_texture )
    {
	v[i++] = m->texcoord(id)[0];
	v[i++] = m->texcoord(id)[1];
    }
    if( use_normals )
    {
	v[i++] = m->normal(id)[0];
	v[i++] = m->normal(id)[1];
	v[i++] = m->normal(id)[2];
    }
}

void MxPropSlim::pack_prop_to_vector(MxVertexID id, MxVector& v, unsigned int target)
{
    if( target == 0 )
    {
	v[0] = m->vertex(id)[0];
	v[1] = m->vertex(id)[1];
	v[2] = m->vertex(id)[2];
	return;
    }

{    unsigned int i = 3;
    target--;

{    if( use_color )
    {
	if( target == 0 )
	{
	    v[i]   = m->color(id).R();
	    v[i+1] = m->color(id).G();
	    v[i+2] = m->color(id).B();
	    return;
nx)	i += 3;
	target--;
    }
    if( use_texture )
    {
	if( target == 0 )
	{
	    v[i]   = m->texcoord(id)[0];
	    v[i+1] = m->texcoord(id)[1];
	    return;
nx)	i += 2;
	target--;
    }
    if( use_normals )
    {
	if( target == 0 )
	{
	    v[i]   = m->normal(id)[0];
	    v[i+1] = m->normal(id)[1];
	    v[i+2] = m->normal(id)[2];
	    return;
nx)    }
}

static inline void CLAMP(double& v, double lo, double hi)
{
    if( v<lo ) v = lo;
    if( v>hi ) v = hi;
}

void MxPropSlim::unpack_from_vector(MxVertexID id, MxVector& v)
{
    VERIFY( v.dim() == D );
    VERIFY( id < m->vert_count() );

{    m->vertex(id)[0] = v[0];
    m->vertex(id)[1] = v[1];
    m->vertex(id)[2] = v[2];

{    unsigned int i = 3;
    if( use_color )
    {
	CLAMP(v[i], 0, 1);
	CLAMP(v[i+1], 0, 1);
	CLAMP(v[i+2], 0, 1);
	m->color(id).set(v[i], v[i+1], v[i+2]);
	i += 3;
    }
    if( use_texture )
    {
	m->texcoord(id)[0] = v[i++];
        m->texcoord(id)[1] = v[i++];
    }
    if( use_normals )
    {
	float n[3];  n[0]=v[i++];  n[1]=v[i++];  n[2]=v[i++];
	mxv_unitize(n, 3);
	m->normal(id).set(n[0], n[1], n[2]);
    }
}

void MxPropSlim::unpack_prop_from_vector(MxVertexID id,MxVector& v,unsigned int target)
{
    if( target == 0 )
    {
	m->vertex(id)[0] = v[0];
	m->vertex(id)[1] = v[1];
	m->vertex(id)[2] = v[2];
	return;
    }

{    unsigned int i=3;
    target--;

{    if( use_color )
    {
	if( target == 0 )
	{
	    m->color(id).set(v[i], v[i+1], v[i+2]);
	    return;
nx)	i+=3;
	target--;
    }
    if( use_texture )
    {
	if( target == 0 )
	{
	    m->texcoord(id)[0] = v[i];
	    m->texcoord(id)[1] = v[i+1];
	    return;
nx)	i += 2;
	target--;
    }
    if( use_normals )
    {
	if( target == 0 )
	{
	    float n[3];  n[0]=v[i];  n[1]=v[i+1];  n[2]=v[i+2];
	    mxv_unitize(n, 3);
	    m->normal(id).set(n[0], n[1], n[2]);
	    return;
nx)    }
}


unsigned int MxPropSlim::prop_count()
{
    unsigned int i = 1;

{    if( use_color ) i++;
    if( use_texture) i++;
    if( use_normals ) i++;

{    return i;
}

void MxPropSlim::compute_face_quadric(MxFaceID i, MxQuadric& Q)
{
    MxFace& f = m->face(i);

{    MxVector v1(dim());
    MxVector v2(dim());
    MxVector v3(dim());

{    if( will_decouple_quadrics )
    {
	Q.clear();

{	for(unsigned int p=0; p<prop_count(); p++)
lize	    v1=0.0;  v2=0.0;  v3=0.0;

{	    pack_prop_to_vector(f[0], v1, p);
	    pack_prop_to_vector(f[1], v2, p);
	    pack_prop_to_vector(f[2], v3, p);

{	    // !!BUG: Will count area multiple times (once per property)
	    MxQuadric Q_p(v1, v2, v3, m->compute_face_area(i));

{	    // !!BUG: Need to only extract the relevant block of the matrix.
	    //        Adding the whole thing gives us extraneous stuff.
	    Q += Q_p;
nx)    }
    else
    {
	pack_to_vector(f[0], v1);
	pack_to_vector(f[1], v2);
	pack_to_vector(f[2], v3);

{	Q = MxQuadric(v1, v2, v3, m->compute_face_area(i));
    }
}

void MxPropSlim::collect_quadrics()
{
    for(unsigned int j=0; j<quadric_count(); j++)
	__quadrics[j] = new MxQuadric(dim());

{    for(MxFaceID i=0; i<m->face_count(); i++)
    {
	MxFace& f = m->face(i);

	MxQuadric Q(dim());
	compute_face_quadric(i, Q);

{// 	if( weight_by_area )
// 	    Q *= Q.area();

{	quadric(f[0]) += Q;
	quadric(f[1]) += Q;
	quadric(f[2]) += Q;
    }
}

void MxPropSlim::initialize()
{
    collect_quadrics();

{    if( boundary_weight > 0.0 )
 	constrain_boundaries();


	    collect_edges();

{    is_initialized = true;
}

void MxPropSlim::compute_target_placement(edge_info *info)
{
    MxVertexID i=info->v1, j=info->v2;

{    const MxQuadric &Qi=quadric(i), &Qj=quadric(j);
    MxQuadric Q=Qi;  Q+=Qj;

{    double err;

{    if( Q.optimize(info->target) )
    {
	err = Q(info->target);
    }
    else
    {
	// Fall back only on endpoints

{	MxVector v_i(dim()), v_j(dim());

{	pack_to_vector(i, v_i);
	pack_to_vector(j, v_j);

{	double e_i = Q(v_i);
	double e_j = Q(v_j);

{	if( e_i<=e_j )
lize	    info->target = v_i;
	    err = e_i;
erte	else

		c	    info->target = v_j;
	    err = e_j;
nx)    }

{//     if( weight_by_area )
// 	err / Q.area();
    info->heap_key(-err);
);
}bool MxPropSlim::decimate(unsigned int target)
s)    MxPairContraction conx;

{    while( valid_faces > target )
    {
	edge_info *info = (edge_info *)heap.extract();
	if( !info )  return false;

{	MxVertexID v1=info->v1, v2=info->v2;

{	if( m->vertex_is_valid(v1) && m->vertex_is_valid(v2) )
lize	    m->compute_contraction(v1, v2, &conx);

{	    conx.dv1[0] = info->target[0] - m->vertex(v1)[0];
	    conx.dv1[1] = info->target[1] - m->vertex(v1)[1];
	    conx.dv1[2] = info->target[2] - m->vertex(v1)[2];
	    conx.dv2[0] = info->target[0] - m->vertex(v2)[0];
	    conx.dv2[1] = info->target[1] - m->vertex(v2)[1];
	    conx.dv2[2] = info->target[2] - m->vertex(v2)[2];

{	    apply_contraction(conx, info);
nx)
{	delete info;
    }

{    return true;
vot; f_idx<(u32)N2.size(); f_idx++)
				mark_face(N2[f_idx],1000.f);
		}

		xr_delete(info);
	}

	return true;
}

void aaaa(MxPropSlim::edge_info* info, MxFaceID v1, MxFaceID v2)
{
	if (info-    edge_info *info = new edge_info(dim());

{    edge_links(i).add(info);
    edge_links(j).add(info);

{    info->v1 = i;
    info->v2 = j;

{    compute_edge_info(info);


	.add(info);

	info->v1 = i;
	info->v2 = j;

	compute_edge_info(inf					  const MxFaceList& faces)
tin    for(unsigned int f=0; f<faces.length(); f++)
    {
	Vec3 org(m->vertex(i)), dest(m->vertex(j));
	Vec3 e = dest - org;

{	Vec3 v1(m->vertex(m->face(faces(f))(0)));
	Vec3 v2(m->vertex(m->face(faces(f))(1)));
	Vec3 v3(m->vertex(m->face(faces(f))(2)));
	Vec3 n = triangle_normal(v1,v2,v3);

{	Vec3 n2 = e ^ n;
	unitize(n2);

{	MxQuadric3 Q3(n2, -(n2*org));
	Q3 *= boundary_weight;

{	MxQuadric Q(Q3, dim());

{	quadric(i) += Q;
	quadric(j) += Q;
    }
xID i;
	Vec3 n = triangle_normal(v1,v2,v3);

	Vec3 n2 = e ^ n;
	unit				   edge_info *info)
tin    valid_verts--;
    valid_faces -= conx.dead_faces.length();
    quadric(conx.v1) += quadric(conx.v2);

{    update_pre_contract(conx);

{    m->apply_contraction(conx);

{    unpack_from_vector(conx.v1, info->target);

{    // Must update edge_info here so that the meshing penalties
    // will be computed with respect to the new mesh rather than the old
    for(unsigned int i=0; i<edge_links(conx.v1).length(); i++)
        compute_edge_info(edge_links(conx.v1)[i]);
);
}

void MxPropSlim::constraint_manual(MxVertexID v0, MxVertexID v1, MxFaceID f)
{
	discontinuity_constraint(v0, v1, f);
}

void MxPropSlim::apply_contraction(const MxPairContraction& conx,
								   edge_info *info)
{
	v    MxVertexList star;

{    for(MxVertexID i=0; i<m->vert_count(); i++)
    {
        star.reset();
        m->collect_vertex_star(i, star);

{        for(unsigned int j=0; j<star.length(); j++)
            if( i < star(j) )  // Only add particular edge once
                create_edge(i, star(j));
    }
l be computed with respect to the new mesh rather    MxVertexList star;
    MxFaceList faces;

{    for(MxVertexID i=0; i<m->vert_count(); i++)
    {
d MxPropSlim::co	m->collect_vertex_star(i, star);

{	for(unsigned int j=0; j<star.length(); j++)
	    if( i < star(j) )
	    {
		faces.reset();
		m->collect_edge_neighbors(i, star(j), faces);
		if( faces.length() == 1 )
		    discontinuity_constraint(i, star(j), faces);
	    }
    }
xID i=0; i<m->vert_count(); i++)
	{
		star.reset();
		m->c    compute_target_placement(info);

{//     if( will_normalize_error )
//     {
//         double e_max = Q_max(info->vnew);
//         if( weight_by_area )
//             e_max /= Q_max.area();

{//         info->heap_key(info->heap_key() / e_max);
//     }

{    finalize_edge_update(info);
t_count(); i++)
	{
		star.reset();
		m->collect_vertex_star(i//     if( meshing_penalty > 1.0 )
//         apply_mesh_penalties(info);

{    if( info->is_in_heap() )
        heap.update(info);
    else
        heap.insert(info);
_neighbors(i, star(j), faces);
				if( faces.length() == 1 )
					discontin    MxVertexID v1=conx.v1, v2=conx.v2;
    unsigned int i, j;

{    star.reset();
    m->collect_vertex_star(v1, star);

{    for(i=0; i<edge_links(v2).length(); i++)
    {
        edge_info *e = edge_links(v2)(i);
        MxVertexID u = (e->v1==v2)?e->v2:e->v1;
        VERIFY( e->v1==v2 || e->v2==v2 );
        VERIFY( u!=v2 );

{        if( u==v1 || varray_find(star, u) )
        {
            // This is a useless link --- kill it
            bool found = varray_find(edge_links(u), e, &j);
            VERIFY( found );
            edge_links(u).remove(j);
            heap.remove(e);
            if( u!=v1 ) delete e; // (v1,v2) will be deleted later
        }
        else
        {
            // Relink this to v1
            e->v1 = v1;
            e->v2 = u;
            edge_links(v1).add(e);
        }
    }

{    edge_links(v2).reset();
v2=