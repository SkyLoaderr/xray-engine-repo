/************************************************************************

MxPropSlim

Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.

$Id: MxPropSlim.cxx,v 1.9 2000/11/20 20:36:38 garland Exp $

************************************************************************/

#include "stdmix.h"
#include "MxPropSlim.h"
#include "MxGeom3D.h"

typedef MxQuadric Quadric;

MxPropSlim::MxPropSlim(MxStdModel *m0)
: MxStdSlim(m0),
__quadrics(m0->vert_count()),
edge_links(m0->vert_count())
{
	consider_color();
	consider_texture();
	consider_normals();

	D = compute_dimension(m);

	will_decouple_quadrics = false;
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

	if( use_color )  d += 3;
	if( use_texture )  d += 2;
	if( use_normals )  d += 3;

	return d;
}

void MxPropSlim::pack_to_vector(MxVertexID id, MxVector& v)
{
	VERIFY( v.dim() == D );
	VERIFY( id < m->vert_count() );

	v[0] = m->vertex(id)[0];
	v[1] = m->vertex(id)[1];
	v[2] = m->vertex(id)[2];

	unsigned int i = 3;
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

	unsigned int i = 3;
	target--;

	if( use_color )
	{
		if( target == 0 )
		{
			v[i]   = m->color(id).R();
			v[i+1] = m->color(id).G();
			v[i+2] = m->color(id).B();
			return;
		}
		i += 3;
		target--;
	}
	if( use_texture )
	{
		if( target == 0 )
		{
			v[i]   = m->texcoord(id)[0];
			v[i+1] = m->texcoord(id)[1];
			return;
		}
		i += 2;
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
		}
	}
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

	m->vertex(id)[0] = v[0];
	m->vertex(id)[1] = v[1];
	m->vertex(id)[2] = v[2];

	unsigned int i = 3;
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

	unsigned int i=3;
	target--;

	if( use_color )
	{
		if( target == 0 )
		{
			m->color(id).set(v[i], v[i+1], v[i+2]);
			return;
		}
		i+=3;
		target--;
	}
	if( use_texture )
	{
		if( target == 0 )
		{
			m->texcoord(id)[0] = v[i];
			m->texcoord(id)[1] = v[i+1];
			return;
		}
		i += 2;
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
		}
	}
}


unsigned int MxPropSlim::prop_count()
{
	unsigned int i = 1;

	if( use_color ) i++;
	if( use_texture) i++;
	if( use_normals ) i++;

	return i;
}

void MxPropSlim::compute_face_quadric(MxFaceID i, MxQuadric& Q)
{
	MxFace& f = m->face(i);

	MxVector v1(dim());
	MxVector v2(dim());
	MxVector v3(dim());

	if( will_decouple_quadrics )
	{
		Q.clear();

		for(unsigned int p=0; p<prop_count(); p++)
		{
			v1=0.0;  v2=0.0;  v3=0.0;

			pack_prop_to_vector(f[0], v1, p);
			pack_prop_to_vector(f[1], v2, p);
			pack_prop_to_vector(f[2], v3, p);

			// !!BUG: Will count area multiple times (once per property)
			MxQuadric Q_p(v1, v2, v3, m->compute_face_area(i));

			// !!BUG: Need to only extract the relevant block of the matrix.
			//        Adding the whole thing gives us extraneous stuff.
			Q += Q_p;
		}
	}
	else
	{
		pack_to_vector(f[0], v1);
		pack_to_vector(f[1], v2);
		pack_to_vector(f[2], v3);

		Q = MxQuadric(v1, v2, v3, m->compute_face_area(i));
	}
}

void MxPropSlim::collect_quadrics()
{
	for(unsigned int j=0; j<quadric_count(); j++)
		__quadrics[j] = xr_new<MxQuadric>(dim());

	for(MxFaceID i=0; i<m->face_count(); i++)
	{
		MxFace& f = m->face(i);

		MxQuadric Q(dim());
		compute_face_quadric(i, Q);

		// 	if( weight_by_area )
		// 	    Q *= Q.area();
ou		quadric(f[0]) += Q;
		quadric(f[1]) += Q;
		quadric(f[2]) += Q;
	}
}

void MxPropSlim::initialize()
{
	collect_quadrics();

	if( boundary_weight > 0.0 )
		constrain_boundaries();


		collect_edges();

	is_initialized = true;
}

void MxPropSlim::compute_target_placement(edge_info *info)
{
	MxVertexID i=info->v1, j=info->v2;

	const MxQuadric &Qi=quadric(i), &Qj=quadric(j);
	MxQuadric Q=Qi;  Q+=Qj;

	double err;
ou	if( Q.optimize(info->target) )

		c		err = Q(info->target);
erte	else

		c		// Fall back only on endpoints
ou		MxVector v_i(dim()), v_j(dim());
ou		pack_to_vector(i, v_i);
		pack_to_vector(j, v_j);
ou		double e_i = Q(v_i);
		double e_j = Q(v_j);
ou		if( e_i<=e_j )
 ){
			info->target = v_i;
			err = e_i;
r(MxV		else
 ){
			info->target = v_j;
			err = e_j;
		}
	}

	//     if( weight_by_area )
	// 	err / Q.area();
	info->heap_key(-err);
);
}bool MxPropSlim::decimate(unsigned int target, float max_error)
s)
{
	MxPairContraction conx;

	max_error								+= EDGE_BASE_ERROR;
	while( valid_faces > target )
	{
		MxHeapable *top						= heap.top();
		if( !top )							{ return false; }
		if( -top->heap_key()>max_error )	{ return true;	}

		edge_info *info						= (edge_info *)heap.extract();
		MxVertexID v1						= info->v1;
		MxVertexID v2						= info->v2;

		if( m->vertex_is_valid(v1) && m->vertex_is_valid(v2) ){
			m->compute_contraction				(v1, v2, &conx);
et			conx.dv1[0] = info->target[0] - m->vertex(v1)[0];
			conx.dv1[1] = info->target[1] - m->vertex(v1)[1];
			conx.dv1[2] = info->target[2] - m->vertex(v1)[2];
			conx.dv2[0] = info->target[0] - m->vertex(v2)[0];
			conx.dv2[1] = info->target[1] - m->vertex(v2)[1];
			conx.dv2[2] = info->target[2] - m->vertex(v2)[2];
o);

			const MxFaceList& N2			= conx.delta_faces;
			for (u32 f_idx=conx.delta_pivot; f_idx<(u32)N2.size(); f_idx++)
				mark_face(N2[f_idx],1000.f);
		}

		xr_delete(info);
	}

	return true;
}

void aaaa(MxPropSlim::edge_info* info, MxFaceID v1, MxFaceID v2)
{
	if (info->v1==v1||info->v2==v1||info->v1==v2||info->v2==v2)
		info->heap_key(-1000.f);
}

void MxPropSlim::mark_face(MxFaceID id, float err)
{
	MxFace& F = m->face(id);

	.add(info);

	info->v1 = i;
	info->v2 = j;

	compute_edge_info(info);
}

void MxPropSlim::discontinuity_	for(unsigned int f=0; f<faces.length(); f++)

		c		Vec3 org(m->vertex(i)), dest(m->vertex(j));
		Vec3 e = dest - org;

			Vec3 v1(m->vertex(m->face(faces(f))(0)));
		Vec3 v2(m->vertex(m->face(faces(f))(1)));
		Vec3 v3(m->vertex(m->face(faces(f))(2)));
		Vec3 n = triangle_normal(v1,v2,v3);

			Vec3 n2 = e ^ n;
		unitize(n2);

			MxQuadric3 Q3(n2, -(n2*org));
		Q3 *= boundary_weight;

			MxQuadric Q(Q3, dim());

			quadric(i) += Q;
		quadric(j) += Q;
ertexID i;
	Vec3 n = triangle_normal(v1,v2,v3);

	Vec3 n2 = e ^ n;
	unitize(n2);

	MxQuadric3 Q3(n2, -(n2*org));
	Q3 *= boundary_weight;

	if( weighting_policy == MX_WEIGHT_AREA ||
		weighting_policy == MX_WEIGHT_AREA_AVG )
	{
		Q3.set_area(norm2(e));
		Q3 *= Q3.area();
	}

	MxQuadric Q(Q3, dim());

	quadric(i) += Q;
	quadric(j) += Q;
}

void MxPropSlim::discontinuity_constraint(MxVertexID i, MxVertexID j,
										  const Mx	for(unsigned int i=0; i<edge_links(conx.v1).length(); i++)
 f++)
		discontinuity_constraint(i,j,faces[f]);
}

void MxPropSlim::constraint_manual(MxVertexID v0, MxVertexID v1, MxFaceID f)
{
	discontinuity_constraint(v0, v1, f);
}

void MxPropSlim::apply_contraction(const MxPairContraction& conx,
								   edge_info *info)
{
	valid_verts--;
	valid_faces -= conx.dead_faces.length();
	quadric(conx.v1) += quadric(conx.v2);

	update_pre_contract(conx);		for(unsigned int j=0; j<star.length(); j++)
1, info->target);

	// Must update edge_info here so that the meshing penalties
	// will be computed with respect to the new mesh rather than the old
	for(unsigned int i=0; i<(unsigned int)edge_links(conx.v1).length(); i++)
		compute_edge_info(edge_links(conx.v1)[i]);
}



//		for(unsigned int j=0; j<star.length(); j++)
////////
//
// These were copied *unmodified* from MxEdgeQSlim
// (with some unsupported features commented out).
//

void MxPropSlim::collect_edges()
{
	MxVertexList star;

	foertexID i=0; i<m->vert_count(); i++)
	{
		star.reset();
		m->collect_vertex_star(i, star);

		for(unsigned int j=0; j<(unsigned int)star.length(); j++)
			if( i < star(j) )  // Only add particular edge once
				create_edge(i, star(j));
	}
}

void MxPropSlim::constrain_boundaries()
{
	MxVertexList star;
	MxFaceList faces;

	for(MxVertexID i=0; i<m->vert_count(); i++)
	{
		star.reset();
		m->collect_vertex_star(i	//     if( meshing_penalty > 1.0 )
	//         apply_mesh_penalties(info);
th(); j++){
			if( i < star(j) )
			{
				faces.reset();
				m->collect_edge_neighbors(i, star(j), faces);
				if( faces.length() == 1 )
					discontinuity_constraint(i, star(j), faces);
			}
		}
	}
}

void MxPropSlim::compute_edge_info(edge_info *info)
{
		for(i=0; i<edge_links(v2).length(); i++)
lize_error )
	//     {
	//         double e_max = Q_max(info->vnew);
	//         if( weight_by_area )
	//             e_max /= Q_max.area();

	//         info->heap_key(info->heap_key() / e_max);
	//     }

	finalize_edge_update(info);
}

void MxPropSlim::finalize_edge_update(edge_info *info)
{
	if( meshing_penalty > 1.0 )
		apply_mesh_penalties(info);

	if( info->is_in_heap() )
		heap.update(info);
	else
		heap.insert(info);
}

void MxPropSlim::update_pre_contract(const MxPairContraction& conx)
{
	MxVertexID v1=conx.v1, v2=