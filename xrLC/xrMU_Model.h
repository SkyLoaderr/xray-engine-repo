#pragma once

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
	void		Load	(CStream& FS);
};

class xrMU_Reference
{
public:

public:
	void		Load	(CStream& FS);
};
