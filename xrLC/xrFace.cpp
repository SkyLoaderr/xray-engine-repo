#include "stdafx.h"
#include "build.h"

const int	edge2idx	[3][2]	= { {0,1},		{1,2},		{2,0}	};
const int	edge2idx3	[3][3]	= { {0,1,2},	{1,2,0},	{2,0,1}	};
const int	idx2edge	[3][3]  = {
	{-1,  0,  2},
	{ 0, -1,  1},
	{ 2,  1, -1}
};

DWORD			dwInvalidFaces	= 0;

extern CBuild*	pBuild;

bool			g_bUnregister = true;

vecVertex		g_vertices;
vecFace			g_faces;

poolVertices	VertexPool;
poolFaces		FacePool;

Vertex::Vertex()
{
	g_vertices.push_back(this);
	adjacent.reserve	(4);
}
Vertex::~Vertex()
{
	if (g_bUnregister) {
		vecVertexIt F = find(g_vertices.begin(), g_vertices.end(), this);
		if (F!=g_vertices.end()) g_vertices.erase(F);
		else Log("Unregistered VERTEX destroyed");
	}
}
Vertex*	Vertex::CreateCopy_NOADJ()
{
	Vertex* V	= VertexPool.create();
	V->P.set	(P);
	V->N.set	(N);
	V->Color	= Color;
	return		V;
}

void	Vertex::normalFromAdj()
{
	N.set(0,0,0);
	for (vecFaceIt ad = adjacent.begin(); ad!=adjacent.end(); ad++)
		N.add( (*ad)->N );
	N.normalize_safe();
}

Face::Face()
{
	pDeflector	= 0;
	bSplitted	= FALSE;
	g_faces.push_back(this);
}
Face::~Face()
{
	if (g_bUnregister) {
		vecFaceIt F = find(g_faces.begin(), g_faces.end(), this);
		if (F!=g_faces.end()) g_faces.erase(F);
		else Log("* ERROR: Unregistered FACE destroyed");
	}
	// Remove 'this' from adjacency info in vertices
	for (int i=0; i<3; i++)
		v[i]->prep_remove(this);
}
Shader_xrLC&	Face::Shader()
{
	DWORD shader_id = pBuild->materials[dwMaterial].reserved;
	return *(pBuild->shaders.Get(shader_id));
}

#define VPUSH(a) a.x,a.y,a.z
void	Face::Failure		()
{
	dwInvalidFaces			++;

	Msg		("* ERROR: Invalid face. (A=%f,e0=%f,e1=%f,e2=%f)",
		CalcArea(),
		v[0]->P.distance_to(v[1]->P),
		v[0]->P.distance_to(v[2]->P),
		v[1]->P.distance_to(v[2]->P)
		);
	Msg		("*        v0[%f,%f,%f], v1[%f,%f,%f], v2[%f,%f,%f]",
		VPUSH(v[0]->P),
		VPUSH(v[1]->P),
		VPUSH(v[2]->P)
		);
	pBuild->err_invalid.Wvector	(v[0]->P);
	pBuild->err_invalid.Wvector	(v[1]->P);
	pBuild->err_invalid.Wvector	(v[2]->P);
}

void	Face::Verify		()
{
	// 1st area
	if	(CalcArea()<EPS)	{ Failure(); return; }

	// 2nd possibility to calc normal
	CalcNormal				();
}

void	Face::CalcNormal	()
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
		CalcNormal2	();
	} else {
		N.div(mag);
	}
};

void	Face::CalcNormal2	()
{
	FPU::m64r		();
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
		Failure		();
		Dvector Nabs;
		Nabs.abs	(dN);

#define SIGN(a) ((a>=0.f)?1.f:-1.f)
		if (Nabs.x>Nabs.y && Nabs.x>Nabs.z)			N.set(SIGN(N.x),0.f,0.f);
		else if (Nabs.y>Nabs.x && Nabs.y>Nabs.z)	N.set(0.f,SIGN(N.y),0.f);
		else if (Nabs.z>Nabs.x && Nabs.z>Nabs.y)	N.set(0.f,0.f,SIGN(N.z));
		else {
			N.set	(0,1,0); 
		}
#undef SIGN
	} else {
		dN.div	(mag);
		N.set	(dN);
	}
}

float Face::CalcArea()
{
	float	e1 = v[0]->P.distance_to(v[1]->P);
	float	e2 = v[0]->P.distance_to(v[2]->P);
	float	e3 = v[1]->P.distance_to(v[2]->P);

	float	p  = (e1+e2+e3)/2.f;
	return	sqrtf( p*(p-e1)*(p-e2)*(p-e3) );
}
float Face::CalcMaxEdge()
{
	float	e1 = v[0]->P.distance_to(v[1]->P);
	float	e2 = v[0]->P.distance_to(v[2]->P);
	float	e3 = v[1]->P.distance_to(v[2]->P);
	
	if (e1>e2 && e1>e3) return e1;
	if (e2>e1 && e2>e3) return e2;
	return e3;
}

BOOL Face::RenderEqualTo(Face *F)
{
	if (F->dwMaterial	!= dwMaterial		)	return FALSE;
	if (F->tc.size()	!= F->tc.size()		)	return FALSE;	// redundant???
	return TRUE;
}

void Face::CacheOpacity()
{
	bOpaque	= false;
	
	b_material& M		= pBuild->materials		[dwMaterial];
	b_texture&	T		= pBuild->textures		[M.surfidx];
	if (T.bHasAlpha)	bOpaque = FALSE;
	else				bOpaque = TRUE;
}
