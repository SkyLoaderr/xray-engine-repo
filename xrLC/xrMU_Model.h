#pragma once

#include "xrPool.h"

class xrMU_Model
{
public:
	struct	_vertex;
	struct	_face;
	typedef	vector<_vertex*>		v_vertices;
	typedef	v_vertices::iterator	v_vertices_it;
	typedef vector<_face*>			v_faces;
	typedef v_faces::iterator		v_faces_it;

	//** 
	struct	_vertex
	{
		Fvector		P;
		Fvector		N;
		v_faces		adjacent;

	public:
		void		prep_add			(_face* F);
		void		prep_remove			(_face* F);
		void		calc_normal_adjacent();
	};

	//**
	struct	_face
	{
	public:
		_vertex*	v	[3];
		Fvector2	tc	[3];
		Fvector		N;

		WORD		dwMaterial;		// index of material
		WORD		dwMaterialGame;	// unique-id of game material (must persist up to game-CForm saving)

		union			
		{
			bool			bSplitted;		//
			bool			bOpaque;		// For ray-tracing speedup
			bool			bProcessed;
		};

	public:
		Shader_xrLC&	Shader			();
		void			CacheOpacity	();
		bool			VContains		( _vertex* pV);					// Does the face contains this vertex?
		void			VReplace		( _vertex* what, _vertex* to);	// Replace ONE vertex by ANOTHER
		int				VIndex			( _vertex* pV);
		void			VSet			( int idx, _vertex* V);
		void			VSet			(_vertex *V1, _vertex *V2, _vertex *V3);
		BOOL			isDegenerated	();
		float			EdgeLen			(int edge);
		void			EdgeVerts		(int e, _vertex** A, _vertex** B);
		void			CalcNormal		();
		void			CalcNormal2		();
		float			CalcArea		();
		float			CalcMaxEdge		();
		void			CalcCenter		(Fvector &C);
		BOOL			RenderEqualTo	(Face *F);
	};
public:
	string128			m_name;
	v_vertices			m_vertices;
	v_faces				m_faces;
private:
	_face*				load_create_face	(Fvector& P1, Fvector& P2, Fvector& P3, b_face& F);
	_vertex*			load_create_vertex	(Fvector& P);
public:
	void				Load				(CStream& FS);
	void				calc_normals		();
};

class xrMU_Reference
{
public:

public:
	void				Load	(CStream& FS);
};

extern	poolSS<xrMU_Model::_vertex,256>	mu_vertices;
extern	poolSS<xrMU_Model::_face,256>	mu_faces;
