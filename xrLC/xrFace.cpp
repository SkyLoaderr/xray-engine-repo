#include "stdafx.h"
#include "build.h"

extern CBuild*	pBuild;

bool			g_bUnregister = true;

vecVertex		g_vertices;
vecFace			g_faces;

Vertex::Vertex()
{
	g_vertices.push_back(this);
}
Vertex::~Vertex()
{
	if (g_bUnregister) {
		vecVertexIt F = find(g_vertices.begin(), g_vertices.end(), this);
		if (F!=g_vertices.end()) g_vertices.erase(F);
		else Log("Unregistered VERTEX destroyed");
	}
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
SH_ShaderDef&	Face::Shader()
{
	DWORD shader_id = pBuild->materials[dwMaterial].reserved;
	return pBuild->shader_defs[shader_id];
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
	bOpaque	= true;

	SH_ShaderDef&	SH							= Shader();
	if (!SH.C.bCastShadow)						return;
	if (0==SH.Passes_Count)						return;
	
	SH_PassDef&		Pass						= SH.Passes[0];
	if (!Pass.Flags.bABlend)					return;
	if (0==Pass.Stages_Count)					return;
	
	SH_StageDef&	Stage						= Pass.Stages[0];
	if (Stage.tcs!=SH_StageDef::tcsGeometry)	return;
	//if (Stage.tcm!=0)							return;				// Incorrect for trees
	
	b_material& M = pBuild->materials			[dwMaterial];
	if (0==M.dwTexCount)						return;
	
	b_texture&	T  = pBuild->textures			[M.surfidx[0]];
	if (!T.bHasAlpha)							return;
	
	if (strcmp(Stage.Tname,"$lmap")==0)
	{
		// first stage - lightmap - use second stage if available
		if (Pass.Stages_Count<2)					return;
		
		SH_StageDef&	StageX						= Pass.Stages[1];
		if (StageX.tcs!=SH_StageDef::tcsGeometry)	return;
		// if (StageX.tcm!=0)						return;			// Incorrect for trees
		if (strcmp(StageX.Tname,"$base0")!=0)		return;
		if (strcmp(StageX.Gname,"$base0")!=0)		return;
	} else {
		if (strcmp(Stage.Tname,"$base0")!=0)		return;
		if (strcmp(Stage.Gname,"$base0")!=0)		return;
	}

	bOpaque	= false;
}
