// Build.h: interface for the CBuild class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUILD_H__C7D43EBA_1C15_4FF4_A3ED_E89F6D759F58__INCLUDED_)
#define AFX_BUILD_H__C7D43EBA_1C15_4FF4_A3ED_E89F6D759F58__INCLUDED_
#pragma once

#include "communicate.h"
#include "b_globals.h"
#include "fs.h"
#include "xrLevel.h"
#include "Etextureparams.h"
#include "shader_xrlc.h"

#pragma comment(lib,"x:\\dxt.lib")
extern "C" __declspec(dllimport) bool __cdecl DXTCompress(LPCSTR out_name, BYTE* raw_data, DWORD w, DWORD h, DWORD pitch, STextureParams* fmt, DWORD depth);

struct b_BuildTexture : public b_texture
{
	STextureParams	THM;
	
	DWORD&	Texel	(DWORD x, DWORD y)
	{
		return pSurface[y*dwWidth+x];
	}
	void Vflip		()
	{
		R_ASSERT(pSurface);
		for (DWORD y=0; y<dwHeight/2; y++)
		{
			DWORD y2 = dwHeight-y-1;
			for (DWORD x=0; x<dwWidth; x++) 
			{
				DWORD		t	= Texel(x,y);
				Texel	(x,y)	= Texel(x,y2);
				Texel	(x,y2)	= t;
			}
		}
	}
};

struct R_Control
{
	string64				name;
	vector<DWORD>			data;
};
struct R_Layer
{
	R_Control				control;
	vector<R_Light>			lights;
};

class CBuild  
{
public:
	Fbox					scene_bb;
	
	vector<b_material>		materials;
	vector<b_shader>		shader_render;
	vector<b_shader>		shader_compile;
	vector<b_BuildTexture>	textures;

	vector<b_glow>			glows;
	vector<b_portal>		portals;

	vector<R_Layer>			L_layers;
	vector<b_light_dynamic>	L_dynamic;

	Shader_xrLC_LIB			shaders;

	DWORD	mem_Usage				();
	void	mem_Compact				();
	void	mem_CompactSubdivs		();
public:
	void	Run						(LPCSTR path);

	void	PreOptimize				();
	void	CorrectTJunctions		();
	void	CalcNormals				();
	void	Tesselate				();

	void	BuildCForm				(CFS_Base &fs);
	void	BuildPortals			(CFS_Base &fs);
	void	BuildRapid				();
		
	void	IsolateVertices			();
	void	xrPhase_ResolveMaterials();
	void	xrPhase_UVmap			();
	void	xrPhase_Subdivide		();
	void	SoftenLights			();
	void	ImplicitLighting		();
	void	Light					();
	void	LightVertex				();
	void	xrPhase_MergeLM			();
	void	xrPhase_MergeGeometry	();

	void	Flex2OGF				();
	void	BuildSectors			();

	void	SaveLights				(CFS_Base &fs);
	void	SaveTREE				(CFS_Base &fs);
	void	SaveSectors				(CFS_Base &fs);

	CBuild	(b_params& P, CStream&  FS);
	~CBuild	();
};

extern CBuild* pBuild;

#endif // !defined(AFX_BUILD_H__C7D43EBA_1C15_4FF4_A3ED_E89F6D759F58__INCLUDED_)
