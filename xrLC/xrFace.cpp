#include "stdafx.h"
#include "build.h"

const int	edge2idx	[3][2]	= { {0,1},		{1,2},		{2,0}	};
const int	edge2idx3	[3][3]	= { {0,1,2},	{1,2,0},	{2,0,1}	};
const int	idx2edge	[3][3]  = {
	{-1,  0,  2},
	{ 0, -1,  1},
	{ 2,  1, -1}
};

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
		else Log("Unregistered FACE destroyed");
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
