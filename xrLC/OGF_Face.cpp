#include "stdafx.h"
#include "build.h"
#include "ogf_face.h"
#include "std_classes.h"
#include "fs.h"
#include "fmesh.h"
#include "xrOcclusion.h"

void set_status(char* N, int id, int f, int v)
{
	static char status_str[512];

	sprintf(status_str,"Model #%4d [F:%5d, V:%5d]: %s...",id,f,v,N);
	Status	(status_str);
	Log		(status_str);
}

BOOL OGF_Vertex::similar(OGF* ogf, OGF_Vertex& V)
{
	if (!P.similar(V.P)) return FALSE;
	if (!N.similar(V.N)) return FALSE;
	for (DWORD i=0; i<V.UV.size(); i++) {
		OGF_Texture *T = ogf->textures.begin()+i;
		b_texture	*B = T->pSurface;
		float		eu = 1.f/float(B->dwWidth );
		float		ev = 1.f/float(B->dwHeight);
		if (!UV[i].similar(V.UV[i],eu,ev)) return FALSE;
	}
	return TRUE;
}

WORD OGF::_BuildVertex	(OGF_Vertex& V1)
{
	for (itOGF_V it=vertices.begin(); it!=vertices.end(); it++)
	{
		if (it->similar(this,V1)) return WORD(it-vertices.begin());
	}
	vertices.push_back(V1);
	return vertices.size()-1;
}

void OGF::Optimize()
{
	// Real optimization
	set_status("Optimizing UV",treeID,faces.size(),vertices.size());

	// Detect relevant number of UV pairs
	R_ASSERT			(vertices.size());
	dwRelevantUV		= vertices.front().UV.size();
	dwRelevantUVMASK	= 0;
	for (DWORD t=0; t<dwRelevantUV; t++) dwRelevantUVMASK |= 1<<t;

	if (0==stricmp(shader->cName,"water")) return;

	// Optimize texture coordinates
	// 1. Calc bounds
	UVpoint Tdelta[8];
	for (DWORD i=0; i<8; i++)
	{
		if (0==(dwRelevantUVMASK&(1<<i))) continue;	// skip unneeded UV
		UVpoint Tmin,Tmax;
		Tmin.set(flt_max,flt_max);
		Tmax.set(flt_min,flt_min);
		for (DWORD j=0; j<vertices.size(); j++)
		{
			OGF_Vertex& V = vertices[j];
			Tmin.min(V.UV[i]);
			Tmax.max(V.UV[i]);
		}
		Tdelta[i].u = floorf((Tmax.u-Tmin.u)/2+Tmin.u);
		Tdelta[i].v = floorf((Tmax.v-Tmin.v)/2+Tmin.v);
	}

	// 2. Recalc UV mapping
	for (i=0; i<vertices.size(); i++)
	{
		svector<UVpoint,8>& UV = vertices[i].UV;
		for (int j=0; j<8; j++)
		{
			if (dwRelevantUVMASK&(1<<j))	UV[j].sub(Tdelta[j]);
			else							UV[j].set(0,0 );
		}
	}
}

void OGF::CreateOccluder()
{
	ORM_Vertex* pV = ORM_Begin(vertices.size(),faces.size()*3,(WORD*)faces.begin());
	for (DWORD i=0; i<vertices.size(); i++)
	{
		OGF_Vertex& V = vertices[i];
		pV->x = V.P.x;
		pV->y = V.P.y;
		pV->z = V.P.z;
		pV->dummycolor = 0xffffffff;
		pV++;
	}
	ORM_End(C,R);
}

void OGF::MakeProgressive()
{
	if (faces.size()>DWORD(g_params.m_SS_DedicateCached)) 
	{
		set_status("CLODing",treeID,faces.size(),vertices.size());
		vertices_saved	= vertices;
		faces_saved		= faces;

		// Options
		PM_Options(dwRelevantUV,dwRelevantUVMASK,3, 
			g_params.m_pm_uv, g_params.m_pm_pos, g_params.m_pm_curv, 
			g_params.m_pm_borderH_angle, g_params.m_pm_borderH_distance, 
			g_params.m_pm_heuristic);

		// Transfer vertices
		for(itOGF_V iV=vertices.begin();iV!=vertices.end();iV++) {
			PM_CreateVertex(
				iV->P.x,iV->P.y,iV->P.z,
				iV - vertices.begin(),
				(P_UV*)iV->UV.begin()
				);
		}

		// Convert
		PM_Result R;
		I_Current = PM_Convert((WORD*)faces.begin(),faces.size()*3,&R);

		if (I_Current>=0) 
		{
			// Permute vertices
			vecOGF_V temp_list = vertices;
			
			// Perform permutation
			for(DWORD i=0; i<temp_list.size(); i++)
				vertices[R.permutePTR[i]]=temp_list[i];
			
			// Copy results
			pmap_vsplit.resize(R.splitSIZE);
			CopyMemory(pmap_vsplit.begin(),R.splitPTR,R.splitSIZE*sizeof(Vsplit));

			pmap_faces.resize(R.facefixSIZE);
			CopyMemory(pmap_faces.begin(),R.facefixPTR,R.facefixSIZE*sizeof(WORD));

			dwMinVerts = R.minVertices;
		}
	}
}

extern int	RegisterTexture	(string &T);
extern int	RegisterShader	(string &T);

#pragma pack(push,4)
struct DPatch {
	Fvector P;
	Fvector	N;
	float	S;
	DWORD	C;
};
#pragma pack(pop)

void OGF_Patch::Save(CFS_Base &fs)
{
	b_material&	M = pBuild->materials[data[0].dwMaterial];

	// Header
	fs.open_chunk		(OGF_HEADER);
	ogf_header H;
	H.format_version	= xrOGF_FormatVersion;
	H.type				= MT_DETAIL_PATCH;
	H.flags				= 0;
	fs.write			(&H,sizeof(H));
	fs.close_chunk		();

	// Texture & shader
	fs.open_chunk(OGF_TEXTURE_L);
	fs.Wdword(RegisterTexture(string(pBuild->textures[M.surfidx[0]].name)));
	fs.Wdword(RegisterShader(string("std_aop")));
	fs.close_chunk();

	// BBox (already computed)
	fs.open_chunk(OGF_BBOX);
	fs.write(&bbox,sizeof(Fvector)*2);
	fs.close_chunk();

	// Sphere (already computed)
	fs.open_chunk(OGF_BSPHERE);
	fs.write(&C,sizeof(Fvector));
	fs.write(&R,sizeof(float));
	fs.close_chunk();

	// Data
	fs.open_chunk(OGF_DPATCH);
	for (DWORD i=0; i<data.size(); i++)
	{
		DPatch P;
		P.P.set(data[i].P);
		P.N.set(data[i].N);
		P.S = data[i].size;
		P.C = data[i].color;
		fs.write(&P,sizeof(P));
	}
	fs.close_chunk();
}

// Represent a node as HierrarhyVisual
void OGF_Node::Save(CFS_Base &fs)
{
	// Header
	fs.open_chunk		(OGF_HEADER);
	ogf_header H;
	H.format_version	= xrOGF_FormatVersion;
	H.type				= MT_HIERRARHY;
	H.flags				= 0;
	fs.write(&H,sizeof(H));
	fs.close_chunk();

	// BBox (already computed)
	fs.open_chunk(OGF_BBOX);
	fs.write(&bbox,sizeof(Fvector)*2);
	fs.close_chunk();

	// Chields
	fs.open_chunk(OGF_CHIELDS_L);
	fs.Wdword(chields.size());
	fs.write(chields.begin(),chields.size()*sizeof(DWORD));
	fs.close_chunk();
}
