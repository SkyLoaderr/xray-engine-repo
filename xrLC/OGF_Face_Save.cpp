#include "stdafx.h"
#include "build.h"
#include "ogf_face.h"
#include "std_classes.h"
#include "fs.h"
#include "fmesh.h"

extern int	RegisterString		(string &T);
extern void	geom_batch_average	(u32 verts, u32 faces);

void OGF::Save			(IWriter &fs)
{
	OGF_Base::Save		(fs);

	// clMsg			("* %d faces",faces.size());
	geom_batch_average	(vertices.size(),faces.size());

	// Create header
	ogf_header H;
	H.format_version	= xrOGF_FormatVersion;
	H.flags				= 0;
	H.type				= MT_NORMAL;

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
	fs.w_u32			(RegisterString(Tname));
	fs.w_u32			(RegisterString(string(pBuild->shader_render[pBuild->materials[material].shader].name)));
	fs.close_chunk		();

	// Vertices
	Shader_xrLC*	SH	=	pBuild->shaders.Get		(pBuild->materials[material].reserved);
	bool bVertexColors	=	(SH->flags.bLIGHT_Vertex);
	bool bNeedNormals	=	(SH->flags.bSaveNormals);
	DWORD	FVF			=	D3DFVF_XYZ|(dwRelevantUV<<D3DFVF_TEXCOUNT_SHIFT) |
							(bVertexColors?D3DFVF_DIFFUSE:0) |
							(bNeedNormals?D3DFVF_NORMAL:0);
	
	switch (H.type) 
	{
	case MT_CACHED:			
		Save_Cached		(fs,H,FVF,bVertexColors,bNeedNormals);		
		break;
	case MT_NORMAL:			
	case MT_PROGRESSIVE:
		Save_Normal_PM	(fs,H,FVF,bVertexColors,bNeedNormals);		
		break;
	case MT_PROGRESSIVE_STRIPS:
		Save_Progressive(fs,H,FVF,bVertexColors,bNeedNormals);		
		break;
	}

	// Header
	fs.open_chunk		(OGF_HEADER);
	fs.w				(&H,sizeof(H));
	fs.close_chunk		();
}

void OGF_Reference::Save	(IWriter &fs)
{
	OGF_Base::Save		(fs);

	// clMsg			("* %d faces",faces.size());
	// geom_batch_average	(vertices.size(),faces.size());	// don't use reference(s) as batch estimate

	// Create header
	ogf_header			H;
	H.format_version	= xrOGF_FormatVersion;
	H.flags				= 0;
	H.type				= MT_TREE;

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
	fs.w_u32			(RegisterString(Tname));
	fs.w_u32			(RegisterString(string(pBuild->shader_render[pBuild->materials[material].shader].name)));
	fs.close_chunk		();

	// Vertices
	fs.open_chunk		(OGF_VCONTAINER);
	fs.w_u32			(vb_id);
	fs.w_u32			(vb_start);
	fs.w_u32			(model->vertices.size());
	fs.close_chunk		();
	
	// Faces
	fs.open_chunk		(OGF_ICONTAINER);
	fs.w_u32			(ib_id);
	fs.w_u32			(ib_start);
	fs.w_u32			(model->faces.size()*3);
	fs.close_chunk		();

	// Special
	fs.open_chunk		(OGF_TREEDEF);
	fs.write			(&xform,sizeof(xform));
	fs.write			(&c_scale,sizeof(c_scale));
	fs.write			(&c_bias,sizeof(c_bias));
	fs.close_chunk		();

	// Header
	fs.open_chunk		(OGF_HEADER);
	fs.write			(&H,sizeof(H));
	fs.close_chunk		();
}

void	OGF::Save_Cached		(IWriter &fs, ogf_header& H, DWORD FVF, BOOL bColors, BOOL bNeedNormals)
{
//	clMsg			("- saving: cached");
	fs.open_chunk	(OGF_VERTICES);
	fs.w_u32		(FVF);
	fs.w_u32		(vertices.size());
	for (itOGF_V V=vertices.begin(); V!=vertices.end(); V++)
	{
		if (bNeedNormals)	fs.write(V,6*sizeof(float));	// Position & normal
		else				fs.write(V,3*sizeof(float));	// Position only
		if (bColors)		fs.write(&(V->Color),4);
		for (DWORD uv=0; uv<dwRelevantUV; uv++)
			fs.write(V->UV.begin()+uv,2*sizeof(float));
	}
	fs.close_chunk	();
	
	// Faces
	fs.open_chunk(OGF_INDICES);
	fs.w_u32	(faces.size()*3);
	for (itOGF_F F=faces.begin(); F!=faces.end(); F++)	fs.write(F,3*sizeof(WORD));
	fs.close_chunk();
}

void	OGF::Save_Normal_PM		(IWriter &fs, ogf_header& H, DWORD FVF, BOOL bColors, BOOL bNeedNormals)
{
//	clMsg			("- saving: normal or clod");

	// Vertices
	u32 ID,Start;
	g_VB.Begin		(FVF);
	for (itOGF_V V=vertices.begin(); V!=vertices.end(); V++)
	{
		if (bNeedNormals)	g_VB.Add(V,6*sizeof(float));	// Position & normal
		else				g_VB.Add(V,3*sizeof(float));	// Position only
		if (bColors)		g_VB.Add(&(V->Color),4);
		for (DWORD uv=0; uv<dwRelevantUV; uv++)
			g_VB.Add(V->UV.begin()+uv,2*sizeof(float));
	}
	g_VB.End		(&ID,&Start);
	
	fs.open_chunk	(OGF_VCONTAINER);
	fs.w_u32		(ID);
	fs.w_u32		(Start);
	fs.w_u32		(vertices.size());
	fs.close_chunk	();
	
	// Faces
	g_IB.Register	(LPWORD(faces.begin()),LPWORD(faces.end()),&ID,&Start);
	fs.open_chunk	(OGF_ICONTAINER);
	fs.w_u32		(ID);
	fs.w_u32		(Start);
	fs.w_u32		(faces.size()*3);
	fs.close_chunk	();
	
	// PMap
	if (I_Current>=0) 
	{
		fs.open_chunk(OGF_P_MAP);
		{
			fs.open_chunk	(0x1);
			fs.w_u32		(dwMinVerts);
			fs.w_u32		(I_Current);
			fs.close_chunk	();
		}
		{
			fs.open_chunk	(0x2);
			fs.write		(pmap_vsplit.begin(),pmap_vsplit.size()*sizeof(Vsplit));
			fs.close_chunk	();
		}
		{
			fs.open_chunk	(0x3);
			fs.w_u32		(pmap_faces.size());
			fs.write		(pmap_faces.begin(),pmap_faces.size()*sizeof(WORD));
			fs.close_chunk	();
		}
		fs.close_chunk();
	}
}

extern	void xrStripify(std::vector<WORD> &indices, std::vector<WORD> &perturb, int iCacheSize, int iMinStripLength);

void	OGF::Save_Progressive	(IWriter &fs, ogf_header& H, DWORD FVF, BOOL bColors, BOOL bNeedNormals)
{
//	clMsg				("- saving: progressive");

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
//		clMsg					("- saving: *degrading*");
		H.type				= MT_NORMAL;
		I_Current			= -1;
		vertices			= vertices_saved;
		faces				= faces_saved;
		Stripify			();
		Save_Normal_PM		(fs,H,FVF,bColors,bNeedNormals);
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
				clMsg			("ERROR: Stripifying failed.");
			}

			// ***** SAVE
			try {
				// Fill container
				u32 ID,Start;
				g_VB.Begin(FVF);
				for (itOGF_V V=strip_verts.begin(); V!=strip_verts.end(); V++)
				{
					if (bNeedNormals)	g_VB.Add(V,6*sizeof(float));	// Position & normal
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
					fs.w_u32		(ID);
					fs.w_u32		(Start);
					fs.w_u32		(strip_verts.size());
					fs.close_chunk	();
					
					// Faces
					fs.open_chunk	(OGF_INDICES);
					fs.w_u32		(strip_indices.size());
					fs.write		(strip_indices.begin(),strip_indices.size()*sizeof(WORD));
					fs.close_chunk	();
				}
				fs.close_chunk		();
			} catch (...) {
				clMsg			("ERROR: Saving failed.");
			}

			// ***** Close chunk
			clMsg("     saved chunk #%d, %d faces, %d verts", 
				dwSample,strip_indices.size()/3, strip_verts.size());
		}
	}
	fs.close_chunk	();
}
