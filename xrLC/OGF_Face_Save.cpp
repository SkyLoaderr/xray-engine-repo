#include "stdafx.h"
#include "build.h"
#include "ogf_face.h"
#include "std_classes.h"
#include "fs.h"
#include "fmesh.h"

extern int	RegisterTexture	(string &T);
extern int	RegisterShader	(string &T);

void OGF::Save(CFS_Base &fs)
{
	// Create header
	ogf_header H;
	H.format_version	= xrOGF_FormatVersion;
	H.flags				= 0;
	if (faces.size()<=DWORD(g_params.m_SS_DedicateCached))	H.type = MT_CACHED;
	else													H.type = (I_Current>=0)?MT_PROGRESSIVE_STRIPS:MT_NORMAL;

	// Texture & shader
	fs.open_chunk		(OGF_TEXTURE_L);
	string Tname;
	for (DWORD i=0; i<textures.size(); i++)
	{
		if (!Tname.empty()) Tname += ',';
		char *fname = textures[i].name;
		if (strchr(fname,'.')) *strchr(fname,'.')=0;
		Tname += fname;
	}
	fs.Wdword			(RegisterTexture(Tname));
	fs.Wdword			(RegisterShader	(string(shader->cName)));
	fs.close_chunk		();

	// BBox (already computed)
	fs.open_chunk		(OGF_BBOX);
	fs.write			(&bbox,sizeof(Fvector)*2);
	fs.close_chunk		();
	
	// Sphere (already computed)
	fs.open_chunk		(OGF_BSPHERE);
	fs.write			(&C,sizeof(Fvector));
	fs.write			(&R,sizeof(float));
	fs.close_chunk		();

	// Vertices
	bool bVertexColors	=	(shader->CL==SH_ShaderDef::clVertex);
	BOOL bNeedLighting	=	(shader->NeedLighting());
	DWORD	FVF			=	D3DFVF_XYZ|(dwRelevantUV<<D3DFVF_TEXCOUNT_SHIFT) |
							(bVertexColors?D3DFVF_DIFFUSE:0) |
							(bNeedLighting?D3DFVF_NORMAL:0);
	
	switch (H.type) 
	{
	case MT_CACHED:			
		Save_Cached		(fs,H,FVF,bVertexColors,bNeedLighting);		
		break;
	case MT_NORMAL:			
	case MT_PROGRESSIVE:
		Save_Normal_PM	(fs,H,FVF,bVertexColors,bNeedLighting);		
		break;
	case MT_PROGRESSIVE_STRIPS:
		Save_Progressive(fs,H,FVF,bVertexColors,bNeedLighting);		
		break;
	}

	// Header
	fs.open_chunk		(OGF_HEADER);
	fs.write			(&H,sizeof(H));
	fs.close_chunk		();
}

void	OGF::Save_Cached		(CFS_Base &fs, ogf_header& H, DWORD FVF, BOOL bColors, BOOL bLighting)
{
	Log				("- saving: cached");
	fs.open_chunk	(OGF_VERTICES);
	fs.Wdword		(FVF);
	fs.Wdword		(vertices.size());
	for (itOGF_V V=vertices.begin(); V!=vertices.end(); V++)
	{
		if (bLighting)		fs.write(V,6*sizeof(float));	// Position & normal
		else				fs.write(V,3*sizeof(float));	// Position only
		if (bColors)		fs.write(&(V->Color),4);
		for (DWORD uv=0; uv<dwRelevantUV; uv++)
			fs.write(V->UV.begin()+uv,2*sizeof(float));
	}
	fs.close_chunk	();
	
	// Faces
	fs.open_chunk(OGF_INDICES);
	fs.Wdword(faces.size()*3);
	for (itOGF_F F=faces.begin(); F!=faces.end(); F++)	fs.write(F,3*sizeof(WORD));
	fs.close_chunk();
}

void	OGF::Save_Normal_PM		(CFS_Base &fs, ogf_header& H, DWORD FVF, BOOL bColors, BOOL bLighting)
{
	Log				("- saving: normal or clod");

	DWORD ID,Start;
	g_VB.Begin(FVF);
	for (itOGF_V V=vertices.begin(); V!=vertices.end(); V++)
	{
		if (bLighting)		g_VB.Add(V,6*sizeof(float));	// Position & normal
		else				g_VB.Add(V,3*sizeof(float));	// Position only
		if (bColors)		g_VB.Add(&(V->Color),4);
		for (DWORD uv=0; uv<dwRelevantUV; uv++)
			g_VB.Add(V->UV.begin()+uv,2*sizeof(float));
	}
	g_VB.End(&ID,&Start);
	
	fs.open_chunk	(OGF_VCONTAINER);
	fs.Wdword		(ID);
	fs.Wdword		(Start);
	fs.Wdword		(vertices.size());
	fs.close_chunk	();
	
	// Faces
	fs.open_chunk	(OGF_INDICES);
	fs.Wdword		(faces.size()*3);
	for (itOGF_F F=faces.begin(); F!=faces.end(); F++)	fs.write(F,3*sizeof(WORD));
	fs.close_chunk	();
	
	// PMap
	if (I_Current>=0) 
	{
		fs.open_chunk(OGF_P_MAP);
		{
			fs.open_chunk	(0x1);
			fs.Wdword		(dwMinVerts);
			fs.Wdword		(I_Current);
			fs.close_chunk	();
		}
		{
			fs.open_chunk	(0x2);
			fs.write		(pmap_vsplit.begin(),pmap_vsplit.size()*sizeof(Vsplit));
			fs.close_chunk	();
		}
		{
			fs.open_chunk	(0x3);
			fs.Wdword		(pmap_faces.size());
			fs.write		(pmap_faces.begin(),pmap_faces.size()*sizeof(WORD));
			fs.close_chunk	();
		}
		fs.close_chunk();
	}
}

extern	void xrStripify(std::vector<WORD> &indices, std::vector<WORD> &perturb, int iCacheSize, int iMinStripLength);

void	OGF::Save_Progressive	(CFS_Base &fs, ogf_header& H, DWORD FVF, BOOL bColors, BOOL bLighting)
{
	Log				("- saving: progressive");

	// Determining the number of samples
	R_ASSERT				(I_Current>=0);
	int verts_for_remove	= vertices.size()-dwMinVerts;
	int samples				= 16;
	int verts_per_sample	= verts_for_remove/samples;
	while (verts_per_sample<4)
	{
		samples--;
		if (samples<=0)		break;
		
		verts_per_sample	= verts_for_remove/samples;
	}
	if (samples<=1)			{
		// Degrading to normal visual
		Log					("- saving: *degrading*");
		H.type				= MT_NORMAL;
		I_Current			= -1;
		vertices			= vertices_saved;
		faces				= faces_saved;
		Stripify			();
		Save_Normal_PM		(fs,H,FVF,bColors,bLighting);
		return;
	}

	// Perform process
	fs.open_chunk(OGF_P_LODS);
	{
		// Init
		DWORD					V_Current,V_Minimal,FIX_Current;
		WORD*					faces_affected	= (WORD*)pmap_faces.begin();
		Vsplit*					vsplit			= (Vsplit*)pmap_vsplit.begin();
		V_Current				= V_Minimal		= dwMinVerts;
		FIX_Current				= 0;
		
		// Cycle on sampes
		for (DWORD dwSample=0; dwSample<DWORD(samples); dwSample++)
		{
			// ***** Perform COLLAPSE or VSPLIT
			DWORD dwCount		= dwMinVerts + verts_per_sample*dwSample;
			if (dwSample==DWORD(samples-1))	dwCount = vertices.size	();
			
			if (V_Current!=dwCount) {
				WORD* Indices	= (WORD*)faces.begin();
				
				// First cycle - try to improve quality
				while (V_Current<dwCount) {
					Vsplit&	S	=	vsplit[V_Current-V_Minimal];
					
					// fixup faces
					DWORD	dwEnd = DWORD(S.numFixFaces)+FIX_Current;
					WORD	V_Cur = WORD(V_Current);
					for (DWORD I=FIX_Current; I<dwEnd; I++) {
						R_ASSERT(Indices[faces_affected[I]]==S.vsplitVert);
						Indices[faces_affected[I]]=V_Cur;
					};
					
					// correct numbers
					I_Current	+=	3*DWORD(S.numNewTriangles);
					V_Current	+=	1;
					FIX_Current	+=	S.numFixFaces;
				};
				
				// Second cycle - try to decrease quality
				while (V_Current>dwCount) {
					V_Current	-=	1;
					
					Vsplit&	S	=	vsplit[V_Current-V_Minimal];
					
					// fixup faces
					WORD V_New		= WORD(S.vsplitVert);
					DWORD dwEnd		= FIX_Current;
					DWORD dwStart	= dwEnd-S.numFixFaces;
					for (DWORD I=dwStart; I<dwEnd; I++) {
						R_ASSERT(Indices[faces_affected[I]]==V_Current);
						Indices[faces_affected[I]]=V_New;
					};
					
					// correct numbers
					I_Current	-=	3*DWORD(S.numNewTriangles);
					FIX_Current	-=	S.numFixFaces;
				};
			}

			// ***** Stripify resulting mesh
			vector<WORD>	strip_indices, strip_permute;
			vecOGF_V		strip_verts;
			try {
				// Stripify
				WORD* strip_F			= (WORD*)faces.begin(); 
				strip_indices.assign	(strip_F, strip_F+I_Current);
				strip_permute.resize	(V_Current);
				xrStripify				(strip_indices,strip_permute,g_params.m_vCacheSize,0);
				
				// Permute vertices
				strip_verts.resize		(V_Current);
				for(DWORD i=0; i<strip_verts.size(); i++)
					strip_verts[i]=vertices[strip_permute[i]];
			} catch (...) {
				Log			("ERROR: Stripifying failed.");
			}

			// ***** SAVE
			try {
				// Fill container
				DWORD ID,Start;
				g_VB.Begin(FVF);
				for (itOGF_V V=strip_verts.begin(); V!=strip_verts.end(); V++)
				{
					if (bLighting)		g_VB.Add(V,6*sizeof(float));	// Position & normal
					else				g_VB.Add(V,3*sizeof(float));	// Position only
					if (bColors)		g_VB.Add(&(V->Color),4);
					for (DWORD uv=0; uv<dwRelevantUV; uv++)
						g_VB.Add(V->UV.begin()+uv,2*sizeof(float));
				}
				g_VB.End(&ID,&Start);

				fs.open_chunk		(dwSample);
				{
					// Vertices
					fs.open_chunk	(OGF_VCONTAINER);
					fs.Wdword		(ID);
					fs.Wdword		(Start);
					fs.Wdword		(strip_verts.size());
					fs.close_chunk	();
					
					// Faces
					fs.open_chunk	(OGF_INDICES);
					fs.Wdword		(strip_indices.size());
					fs.write		(strip_indices.begin(),strip_indices.size()*sizeof(WORD));
					fs.close_chunk	();
				}
				fs.close_chunk		();
			} catch (...) {
				Log			("ERROR: Saving failed.");
			}

			// ***** Close chunk
			Msg("     saved chunk #%d, %d faces, %d verts", 
				dwSample,strip_indices.size()/3, strip_verts.size());
		}
	}
	fs.close_chunk	();
}
