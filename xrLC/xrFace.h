#pragma once

#include "xrShaderDef.h"

#ifndef IC
#define IC __forceinline
#endif

class Vertex;
class Face;
class Material;
class Edge;

// Typedefs
typedef vector<Vertex*>			vecVertex;
typedef vecVertex::iterator		vecVertexIt;
typedef vector<Face*>			vecFace;
typedef vecFace::iterator		vecFaceIt;

extern const int edge2idx[3][2];

#include "xrUVpoint.h"

class Vertex
{
public:
	Fvector		P;
	Fvector		N;
	DWORD		Color;		// only used for Per-Vertex lighting

	vecFace		adjacent;

	IC BOOL	similar(Vertex &V, float eps)
	{
		return P.similar(V.P,eps);
	}
	IC Vertex*	CreateCopy_NOADJ()
	{
		Vertex* V = new Vertex;
		V->P.set(P);
		V->N.set(N);
		V->Color = Color;
		return	V;
	}
	IC	Vertex*	CreateCopy()
	{
		Vertex* V = CreateCopy_NOADJ();
		V->adjacent = adjacent;
		return V;
	}
	IC	void	prep_add(Face* F)
	{
		for (vecFaceIt I=adjacent.begin(); I!=adjacent.end(); I++)
			if (F==(*I)) return;
		adjacent.push_back(F);
	}
	IC	void	prep_remove(Face* F)
	{
		vecFaceIt	I = find(adjacent.begin(),adjacent.end(),F);
		if (I!=adjacent.end())	adjacent.erase(I);
	}

	void	normalFromAdj();

	Vertex();
	~Vertex();
};

struct _TCF {
	UVpoint uv[3];

	IC void	barycentric	(UVpoint &P, float &u, float &v, float &w)
	{
		UVpoint		kV02; kV02.sub(uv[0],uv[2]);
		UVpoint		kV12; kV12.sub(uv[1],uv[2]);
		UVpoint		kPV2; kPV2.sub(P,    uv[2]);
		
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
	IC void	barycentric	(UVpoint &P, Fvector &B)
	{	barycentric(P,B.x,B.y,B.z); }
	IC bool	isInside	(float u, float v, float w)
	{	return (u>=0 && u<=1) && (v>=0 && v<=1) && (w>=0 && w<=1); }
	IC bool	isInside	(Fvector &B)
	{	return	isInside	(B.x,B.y,B.z); }
	IC bool	isInside	(UVpoint &P, Fvector &B) {
		barycentric(P,B);
		return isInside(B);
	}
};

#define TESS_EDGE(E)	(1<<E)
#define TESS_EDGE_0		TESS_EDGE(0)
#define TESS_EDGE_1		TESS_EDGE(1)
#define TESS_EDGE_2		TESS_EDGE(2)
#define TESS_EDGE_ALL	(TESS_EDGE_0 | TESS_EDGE_1 | TESS_EDGE_2)

class Face
{
public:
	//------------------------------//
	Vertex*			v[3];			// vertices
	svector<_TCF,8>	tc;				// TC

	WORD			dwMaterial;		// index of material

	Fvector			N;				// face normal

	void*			pDeflector;		// does the face has LM-UV map?
	union			{
		bool		bSplitted;		//
		bool		bOpaque;		// For ray-tracing speedup
	};

	SH_ShaderDef&	Shader			();
	void			CacheOpacity	();
	//------------------------------//
	IC void Failure	(void)
	{	R_ASSERT(0=="Invalid face"); }

	// Does the face contains this vertex?
	IC bool	VContains(Vertex* pV)
	{
		return VIndex(pV)>=0;
	};

	// Replace ONE vertex by ANOTHER
	IC void	VReplace(Vertex* what, Vertex* to)
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
	IC void CalcNormal2()
	{
		FPU::m64r();
		Dvector			v0,v1,v2,t1,t2,dN;
		v0.set			(v[0]->P);
		v1.set			(v[1]->P);
		v2.set			(v[2]->P);
		t1.sub			(v1,v0);
		t2.sub			(v2,v1);
		dN.crossproduct	(t1,t2);
		double mag		= dN.magnitude();
		if (mag<dbl_zero)
		{
			Dvector Nabs;
			Nabs.abs	(dN);
			
#define SIGN(a) ((a>=0.f)?1.f:-1.f)
			
			if (Nabs.x>Nabs.y && Nabs.x>Nabs.z)			N.set(SIGN(N.x),0.f,0.f);
			else if (Nabs.y>Nabs.x && Nabs.y>Nabs.z)	N.set(0.f,SIGN(N.y),0.f);
			else if (Nabs.z>Nabs.x && Nabs.z>Nabs.y)	N.set(0.f,0.f,SIGN(N.z));
			else {
				Log("* Too small face: can't calc normal");
				N.set(0,1,0); 
			}
#undef SIGN
		} else {
			dN.div	(mag);
			N.set	(dN);
		}
		FPU::m24r();
	}
	IC void	CalcNormal()
	{
		Fvector t1,t2;
		
		Fvector*	v0 = &(v[0]->P);
		Fvector*	v1 = &(v[1]->P);
		Fvector*	v2 = &(v[2]->P);
		t1.sub			(*v1,*v0);
		t2.sub			(*v2,*v1);
		N.crossproduct	(t1,t2);
		float mag		= N.magnitude();
		if (mag<EPS_S)
		{
			CalcNormal2();
		} else {
			N.div(mag);
		}
	};
	IC	void	SetVertex(int idx, Vertex* V)
	{
		v[idx]=V; V->prep_add(this);
	};
	IC void	SetVertices(Vertex *V1, Vertex *V2, Vertex *V3)
	{
		SetVertex(0,V1);
		SetVertex(1,V2);
		SetVertex(2,V3);
	};
	IC BOOL	isDegenerated()
	{
		return (v[0]==v[1] || v[0]==v[2] || v[1]==v[2]);
	};
	IC float	EdgeLen(int edge)
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
	IC void	CalcCenter	(Fvector &C)
	{
		C.set(v[0]->P);
		C.add(v[1]->P);
		C.add(v[2]->P);
		C.div(3);
	};

	BOOL		RenderEqualTo(Face *F);

	void		Unwarp(int connect_edge, float u1, float v1, float u2, float v2);
	void		OA_Unwarp();

	IC void	AddChannel(UVpoint &p1, UVpoint &p2, UVpoint &p3) {
		_TCF TC;
		TC.uv[0] = p1;	TC.uv[1] = p2;	TC.uv[2] = p3;
		tc.push_back(TC);
	}

	Face();
	~Face();
};

#pragma pack(push,1)
class DetailPatch : public b_particle
{
public:
	DWORD	color;
};
#pragma pack(pop)

extern vector<DetailPatch>	g_pathes; 
extern bool					g_bUnregister;