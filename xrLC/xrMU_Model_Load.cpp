#include "stdafx.h"
#include "xrMU_Model.h"

void xrMU_Reference::Load(IReader& F)
{
	b_mu_reference		R;
	F.r					(&R,sizeof(R));
	model				= pBuild->mu_models[R.model_index];
	xform				= R.transform;
	flags				= R.flags;
	sector				= R.sector;

	c_scale.rgb.set		(1,1,1);
	c_scale.hemi		= 1;
	c_scale.sun			= 1;
	c_bias.rgb.set		(0,0,0);
	c_bias.hemi			= 0;
	c_bias.sun			= 0;
}

void xrMU_Model::Load	(IReader& F)
{
	F.r_stringZ			(m_name);

	// READ: vertices
	xr_vector<b_vertex>	b_vertices;
	b_vertices.resize	(F.r_u32());
	m_vertices.reserve	(b_vertices.size());
	F.r					(&*b_vertices.begin(),(u32)b_vertices.size()*sizeof(b_vertex));

	// READ: faces
	xr_vector<b_face>	b_faces;
	b_faces.resize		(F.r_u32());
	m_faces.reserve		(b_faces.size());
	F.r					(&*b_faces.begin(),(u32)b_faces.size()*sizeof(b_face));

	// READ: lod-ID
	F.r					(&m_lod_ID,2);

	// CONVERT and OPTIMIZE
	for (u32 v_it=0; v_it<b_vertices.size(); v_it++)
	{
		create_vertex	(b_vertices[v_it]);
	}
	for (u32 f_it=0; f_it<b_faces.size(); f_it++)
	{
		b_face&	F		= b_faces[f_it];
		create_face		(m_vertices[F.v[0]],m_vertices[F.v[1]],m_vertices[F.v[2]],F);
	}
/*
	// CONVERT and OPTIMIZE
	for (u32 it=0; it<b_faces.size(); it++)
	{
		b_face&	F			= b_faces[it];
		load_create_face	(b_vertices[F.v[0]],b_vertices[F.v[1]],b_vertices[F.v[2]],F);
	}
*/
	// 
	clMsg	("* Loading model: '%s' - v(%d/%d), f(%d/%d)",*m_name,m_vertices.size(),b_vertices.size(),m_faces.size(),b_faces.size());
}

xrMU_Model::_face* xrMU_Model::create_face(_vertex* v0, _vertex* v1, _vertex* v2, b_face& B)
{
	_face*	_F			= mu_faces.create();
	_F->dwMaterial		= u16(B.dwMaterial);
	_F->dwMaterialGame	= B.dwMaterialGame;
	R_ASSERT			(B.dwMaterialGame<65536);

	// Vertices and adjacement info
	_F->VSet			(0,v0);
	_F->VSet			(1,v1);
	_F->VSet			(2,v2);

	// tc
	_F->tc[0]			= B.t[0];
	_F->tc[1]			= B.t[1];
	_F->tc[2]			= B.t[2];
	_F->CalcNormal		();

	// register
	m_faces.push_back	(_F);
	return _F;
}

xrMU_Model::_face* xrMU_Model::load_create_face(Fvector& P1, Fvector& P2, Fvector& P3, b_face& B)
{
	return				create_face(load_create_vertex(P1),load_create_vertex(P2),load_create_vertex(P3),B);
}

xrMU_Model::_vertex* xrMU_Model::create_vertex(Fvector& P)
{
	_vertex*	_V		= mu_vertices.create();
	_V->P				= P;
	_V->N.set			(0,0,0);
	m_vertices.push_back(_V);
	return				_V;
}

xrMU_Model::_vertex* xrMU_Model::load_create_vertex(Fvector& P)
{
	// find similar
	for (u32 it=0; it<m_vertices.size(); it++)
	{
		if (m_vertices[it]->P.similar(P,.001f))	return m_vertices[it];
	}
	// create new
	return				create_vertex(P);
}
