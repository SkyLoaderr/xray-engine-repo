#pragma once

#include "shader_xrlc.h"

class base_lighting
{
	xr_vector<R_Light>		rgb;
	xr_vector<Fvector>		hemi;
	float					hemi_e;
	xr_vector<Fvector>		sun;
	float					sun_e;
};
class base_color
{
	Fvector					rgb;
	float					hemi;
	float					sun;
};
class base_Vertex	
{
public: 
	Fvector					P;
	Fvector					N;
	base_color				C;		// all_lighting info
	float					L_hemi;	// hemisphere
public:
	virtual ~base_Vertex()	= 0; 
};
class base_Face
{
public: 
	Fvector					basis_tangent		[3];
	Fvector					basis_binormal		[3];

	WORD					dwMaterial;			// index of material
	WORD					dwMaterialGame;		// unique-id of game material (must persist up to game-CForm saving)
	union			{
		bool				bSplitted;			//
		bool				bProcessed;
	};
	bool					bDisableShadowCast;
	bool					bOpaque;			// For ray-tracing speedup

	virtual	Shader_xrLC&	Shader				( );
	virtual void			CacheOpacity		( );
	virtual Fvector2*		getTC0				( ) = 0;

	base_Face();
	virtual ~base_Face() = 0; 
};		

class Vertex;
class Face;
class Material;
class Edge;
class CLightmap;

// Typedefs
typedef xr_vector<Vertex*>			vecVertex;
typedef vecVertex::iterator			vecVertexIt;

typedef xr_vector<Face*>			vecFace;
typedef vecFace::iterator			vecFaceIt;

typedef vecFace						vecAdj;
typedef vecAdj::iterator			vecAdjIt;

extern const int edge2idx			[3][2];

#include "xrUVpoint.h"

class Vertex	: public base_Vertex
{
public:
	vecAdj		adjacent;

	Vertex*		CreateCopy_NOADJ	();
	IC BOOL		similar				(Vertex &V, float eps)
	{
		return P.similar(V.P,eps);
	}
	IC	Vertex*	CreateCopy			()
	{
		Vertex* V = CreateCopy_NOADJ();
		V->adjacent = adjacent;
		return V;
	}
	IC	void	prep_add			(Face* F)
	{
		for (vecFaceIt I=adjacent.begin(); I!=adjacent.end(); I++)
			if (F==(*I)) return;
		adjacent.push_back(F);
	}
	IC	void	prep_remove			(Face* F)
	{
		vecFaceIt	I = std::find(adjacent.begin(),adjacent.end(),F);
		if (I!=adjacent.end())	adjacent.erase(I);
	}

	void		normalFromAdj		();

	Vertex();
	virtual ~Vertex();
};

struct _TCF {
	Fvector2		uv	[3];

	IC void	barycentric	(Fvector2 &P, float &u, float &v, float &w)
	{
		Fvector2 	kV02; kV02.sub(uv[0],uv[2]);
		Fvector2 	kV12; kV12.sub(uv[1],uv[2]);
		Fvector2 	kPV2; kPV2.sub(P,    uv[2]);
		
		float		fM00 = kV02.dot(kV02);
		float		fM01 = kV02.dot(kV12);
		float		fM11 = kV12.dot(kV12);
		float		fR0  = kV02.dot(kPV2);
		float		fR1  = kV12.dot(kPV2);
		float		fDet = fM00*fM11 - fM01*fM01;
		
		u			= (fM11*fR0 - fM01*fR1)/fDet;
		v			= (fM00*fR1 - fM01*fR0)/fDet;
		w			= 1.0f - u - v;
	}
	IC void	barycentric	(Fvector2 &P, Fvector &B)
	{	barycentric(P,B.x,B.y,B.z); }
	IC bool	isInside	(float u, float v, float w)
	{	return (u>=0 && u<=1) && (v>=0 && v<=1) && (w>=0 && w<=1); }
	IC bool	isInside	(Fvector &B)
	{	return	isInside	(B.x,B.y,B.z); }
	IC bool	isInside	(Fvector2 &P, Fvector &B) {
		barycentric(P,B);
		return isInside(B);
	}
};

class Face	: public base_Face
{
public:
	Vertex*					v[3];			// vertices
	Fvector					N;				// face normal
	svector<_TCF,2>			tc;				// TC

	void*					pDeflector;		// does the face has LM-UV map?
	svector<CLightmap*,1>	lmap_layers;

	virtual Fvector2*		getTC0				( ) { return tc[0].uv; }

	void			CalcNormal		();
	void			CalcNormal2		();

	//------------------------------//
	void Failure					();
	void Verify						();

	// Does the face contains this vertex?
	IC bool	VContains	(Vertex* pV)
	{
		return VIndex(pV)>=0;
	};

	// Replace ONE vertex by ANOTHER
	IC void	VReplace	(Vertex* what, Vertex* to)
	{
		if (v[0]==what) { v[0]=to; what->prep_remove(this); to->prep_add(this); }
		if (v[1]==what) { v[1]=to; what->prep_remove(this); to->prep_add(this); }
		if (v[2]==what) { v[2]=to; what->prep_remove(this); to->prep_add(this); }
	};
	IC void	VReplace_not_remove(Vertex* what, Vertex* to)
	{
		if (v[0]==what) { v[0]=to; to->prep_add(this); }
		if (v[1]==what) { v[1]=to; to->prep_add(this); }
		if (v[2]==what) { v[2]=to; to->prep_add(this); }
	};
	IC	int		VIndex	(Vertex* pV)
	{
		if (v[0]==pV) return 0;
		if (v[1]==pV) return 1;
		if (v[2]==pV) return 2;
		return -1;
	};

	IC	void	SetVertex(int idx, Vertex* V)
	{
		v[idx]=V; V->prep_add(this);
	};
	IC void		SetVertices(Vertex *V1, Vertex *V2, Vertex *V3)
	{
		SetVertex(0,V1);
		SetVertex(1,V2);
		SetVertex(2,V3);
	};
	IC BOOL		isDegenerated()
	{
		return (v[0]==v[1] || v[0]==v[2] || v[1]==v[2]);
	};
	IC float	EdgeLen	(int edge)
	{
		Vertex* V1 = v[edge2idx[edge][0]];
		Vertex* V2 = v[edge2idx[edge][1]];
		return V1->P.distance_to(V2->P);
	};
	IC void		EdgeVerts(int e, Vertex** A, Vertex** B)
	{
		*A = v[edge2idx[e][0]];
		*B = v[edge2idx[e][1]];
	}

	float		CalcArea	();
	float		CalcMaxEdge	();
	IC void		CalcCenter	(Fvector &C)
	{
		C.set(v[0]->P);
		C.add(v[1]->P);
		C.add(v[2]->P);
		C.div(3);
	};

	BOOL		RenderEqualTo(Face *F);

	void		Unwarp(int connect_edge, float u1, float v1, float u2, float v2);
	void		OA_Unwarp();

	IC void		AddChannel	(Fvector2 &p1, Fvector2 &p2, Fvector2 &p3) 
	{
		_TCF	TC;
		TC.uv[0] = p1;	TC.uv[1] = p2;	TC.uv[2] = p3;
		tc.push_back(TC);
	}

	Face();
	virtual ~Face();
};

//
typedef poolSS<Vertex,8*1024>	poolVertices;
extern poolVertices				VertexPool;
typedef poolSS<Face,8*1024>		poolFaces;
extern poolFaces				FacePool;

extern bool						g_bUnregister;

