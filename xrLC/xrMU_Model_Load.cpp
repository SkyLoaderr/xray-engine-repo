#include "stdafx.h"
#include "xrMU_Model.h"

void xrMU_Model::Load(CStream& F)
{
	F.RstringZ			(m_name);
	Msg					("* Loading model: '%s'...",m_name);

	// READ: vertices
	vector<b_vertex>	b_vertices;
	b_vertices.resize	(F.Rdword());
	m_vertices.reserve	(b_vertices.size());
	F.Read				(b_vertices.begin(),b_vertices.size()*sizeof(b_vertex));

	// READ: faces
	vector<b_face>		b_faces;
	b_faces.resize		(F.Rdword());
	m_faces.reserve		(b_faces.size());
	F.Read				(b_faces.begin(),b_faces.size()*sizeof(b_face));

	// CONVERT and OPTIMIZE
	for (u32 it=0; it<b_faces.size(); it++)
	{
		b_face&	F			= b_faces[it];
		load_create_face	(b_vertices[F.v[0]],b_vertices[F.v[1]],b_vertices[F.v[2]],F);
	}

	// 
	Msg	("%5s v(%d/%d), f(%d/%d)","-",m_vertices.size(),b_vertices.size(),m_faces.size(),b_faces.size());
}

xrMU_Model::_face* xrMU_Model::load_create_face(Fvector& P1, Fvector& P2, Fvector& P3, b_face& B)
{
	_face*	_F			= mu_faces.create();
	_F->dwMaterial		= WORD(B.dwMaterial);
	_F->dwMaterialGame	= B.dwMaterialGame;

	// Vertices and adjacement info
	_F->VSet			(0,load_create_vertex(P1));
	_F->VSet			(1,load_create_vertex(P2));
	_F->VSet			(2,load_create_vertex(P3));

	// tc
	_F->tc[0]			= B.t[0];
	_F->tc[1]			= B.t[1];
	_F->tc[2]			= B.t[2];
	_F->CalcNormal		();

	// register
	m_faces.push_back	(_F);
	return _F;
}

xrMU_Model::_vertex* xrMU_Model::load_create_vertex(Fvector& P)
{
	// find similar
	for (u32 it=0; it<m_vertices.size(); it++)
	{
		if (m_vertices[it]->P.similar(P,.001f))	return m_vertices[it];
	}

	// create new
	_vertex*	_V		= mu_vertices.create();
	_V->P				= P;
	_V->N.set			(0,0,0);
	m_vertices.push_back(_V);
	return _V;
}
