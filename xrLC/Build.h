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
#include "xrMU_Model.h"

#pragma comment(lib,"x:\\dxt.lib")
extern "C" __declspec(dllimport) bool __stdcall DXTCompress(LPCSTR out_name, BYTE* raw_data, DWORD w, DWORD h, DWORD pitch, STextureParams* fmt, DWORD depth);

struct b_BuildTexture : public b_texture
{
	STextureParams	THM;
	
	u32&	Texel	(DWORD x, DWORD y)
	{
		return pSurface[y*dwWidth+x];
	}
	void	Vflip		()
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
	CFS_Memory				err_invalid;
	CFS_Memory				err_tjunction;
	CFS_Memory				err_multiedge;
	void					err_save	();

	Fbox					scene_bb;
	
	vector<b_material>		materials;
	vector<b_shader>		shader_render;
	vector<b_shader>		shader_compile;
	vector<b_BuildTexture>	textures;

	vector<b_glow>			glows;
	vector<b_portal>		portals;
	vector<b_lod>			lods;

	vector<R_Layer>			L_layers;
	vector<b_light_dynamic>	L_dynamic;
	
	vector<BYTE>			L_control_data;
	
	vector<xrMU_Model*>		mu_models;
	vector<xrMU_Reference*>	mu_refs;

	Shader_xrLC_LIB			shaders;
	string					path;

	void	mem_Compact				();
	void	mem_CompactSubdivs		();
	vector<R_Layer>*				LLayer_by_name	(LPCSTR N);
public:
	void	Load					(const b_params& P, const CStream&  FS);
	void	Run						(string& path);

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

	CBuild	();
	~CBuild	();
};

extern CBuild* pBuild;

#endif // !defined(AFX_BUILD_H__C7D43EBA_1C15_4FF4_A3ED_E89F6D759F58__INCLUDED_)
