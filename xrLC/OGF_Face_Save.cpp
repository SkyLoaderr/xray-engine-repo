#include "stdafx.h"
#include "build.h"
#include "ogf_face.h"
#include "std_classes.h"
#include "fs.h"
#include "fmesh.h"

extern int	RegisterString		(LPCSTR T);
extern void	geom_batch_average	(u32 verts, u32 faces);

u32						u8_vec4			(Fvector N, u8 A=0)
{
	N.add				(1.f);
	N.mul				(.5f*255.f);
	s32 nx				= iFloor(N.x);	clamp(nx,0,255);
	s32 ny				= iFloor(N.y);	clamp(ny,0,255);
	s32 nz				= iFloor(N.z);	clamp(nz,0,255);
	return				color_rgba(nx,ny,nz,A);
}
s16						s16_tc_base		(float uv)		// [-32 .. +32]
{
	const u32	max_tile	=	32;
	const s32	quant		=	32768/max_tile;

	s32			t			=	iFloor	(uv*float(quant)); clamp(t,-32768,32767);
	return	s16	(t);
}
s16						s16_tc_lmap		(float uv)		// [-1 .. +1]
{
	const u32	max_tile	=	1;
	const s32	quant		=	32768/max_tile;

	s32			t			=	iFloor	(uv*float(quant)); clamp(t,-32768,32767);
	return	s16	(t);
}

D3DVERTEXELEMENT9		r2_decl			[] =	// 36
{
	{0, 0,  D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,	0 },
	{0, 12, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,	0 },
	{0, 16, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TANGENT,	0 },
	{0, 20, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_BINORMAL,	0 },
	{0, 24, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_COLOR,		0 },
	{0, 28, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	0 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9		r1_decl_lmap	[] =	// 12+4+8	= 24
{
	{0, 0,  D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,	0 },
	{0, 12, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,	0 },
	{0, 16, D3DDECLTYPE_SHORT4,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	0 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9		r1_decl_vert	[] =	// 12+4+4+4 = 24
{
	{0, 0,  D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,	0 },
	{0, 12, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,	0 },
	{0, 16, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_COLOR,		0 },
	{0, 20, D3DDECLTYPE_SHORT2,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	0 },
	D3DDECL_END()
};
#pragma pack(push,1)
struct  r1v_lmap	{
	Fvector3	P;
	u32			N;
	u16			tc0x,tc0y;
	u16			tc1x,tc1y;

	r1v_lmap	(Fvector3 _P, Fvector _N, base_color _C, Fvector2 tc_base, Fvector2 tc_lmap )
	{
		_N.normalize	();
		P				= _P;
		N				= u8_vec4		(_N,u8_clr(_C.hemi));
		tc0x			= s16_tc_base	(tc_base.x);
		tc0y			= s16_tc_base	(tc_base.y);
		tc1x			= s16_tc_lmap	(tc_lmap.x);
		tc1y			= s16_tc_lmap	(tc_lmap.y);
	}
};
struct  r1v_vert	{
	Fvector3	P;
	u32			N;
	u32			C;
	u16			tc0x,tc0y;

	r1v_vert	(Fvector3 _P, Fvector _N, base_color _C, Fvector2 tc_base)
	{
		_N.normalize	();
		P				= _P;
		N				= u8_vec4		(_N,u8_clr(_C.hemi));
		C				= color_rgba	(u8_clr(_C.rgb.x),u8_clr(_C.rgb.y),u8_clr(_C.rgb.z),u8_clr(_C.sun));
		tc0x			= s16_tc_base	(tc_base.x);
		tc0y			= s16_tc_base	(tc_base.y);
	}
};
#pragma pack(pop)

void OGF::Save			(IWriter &fs)
{
	OGF_Base::Save		(fs);

	// clMsg			("* %d faces",faces.size());
	geom_batch_average	((u32)vertices.size(),(u32)faces.size());

	// Create header
	ogf_header H;
	H.format_version	= xrOGF_FormatVersion;
	H.flags				= 0;
	H.type				= MT_NORMAL;

	// Texture & shader
	fs.open_chunk		(OGF_TEXTURE_L);
	std::string Tname;
	for (u32 i=0; i<textures.size(); i++)
	{
		if (!Tname.empty()) Tname += ',';
		char *fname = textures[i].name;
		if (strchr(fname,'.')) *strchr(fname,'.')=0;
		Tname += fname;
	}
	fs.w_u32			(RegisterString(Tname.c_str()));
	fs.w_u32			(RegisterString(pBuild->shader_render[pBuild->materials[material].shader].name));
	fs.close_chunk		();

	// Vertices
	Shader_xrLC*	SH	=	pBuild->shaders.Get		(pBuild->materials[material].reserved);
	bool bVertexColors	=	(SH->flags.bLIGHT_Vertex);
	
	switch (H.type) 
	{
	case MT_CACHED:			
		Save_Cached		(fs,H,bVertexColors);		
		break;
	case MT_NORMAL:
	case MT_PROGRESSIVE:
		Save_Normal_PM	(fs,H,bVertexColors);		
		break;
	case MT_PROGRESSIVE_STRIPS:
		Save_Progressive(fs,H,bVertexColors);		
		break;
	}

	// Header
	fs.open_chunk			(OGF_HEADER);
	fs.w					(&H,sizeof(H));
	fs.close_chunk			();
}

void OGF_Reference::Save	(IWriter &fs)
{
	OGF_Base::Save		(fs);

	// geom_batch_average	(vertices.size(),faces.size());	// don't use reference(s) as batch estimate

	// Create header
	ogf_header			H;
	H.format_version	= xrOGF_FormatVersion;
	H.flags				= 0;
	H.type				= MT_TREE;

	// Texture & shader
	fs.open_chunk		(OGF_TEXTURE_L);
	std::string			Tname;
	for (u32 i=0; i<textures.size(); i++)
	{
		if (!Tname.empty()) Tname += ',';
		char *fname = textures[i].name;
		if (strchr(fname,'.')) *strchr(fname,'.')=0;
		Tname += fname;
	}
	fs.w_u32			(RegisterString(Tname.c_str()));
	fs.w_u32			(RegisterString(pBuild->shader_render[pBuild->materials[material].shader].name));
	fs.close_chunk		();

	// Vertices
	fs.open_chunk		(OGF_VCONTAINER);
	fs.w_u32			(vb_id);
	fs.w_u32			(vb_start);
	fs.w_u32			((u32)model->vertices.size());
	fs.close_chunk		();
	
	// Faces
	fs.open_chunk		(OGF_ICONTAINER);
	fs.w_u32			(ib_id);
	fs.w_u32			(ib_start);
	fs.w_u32			((u32)model->faces.size()*3);
	fs.close_chunk		();

	// Special
	fs.open_chunk		(OGF_TREEDEF2);
	fs.w				(&xform,	sizeof(xform));
	fs.w				(&c_scale,	5*sizeof(float));
	fs.w				(&c_bias,	5*sizeof(float));
	fs.close_chunk		();

	// Header
	fs.open_chunk		(OGF_HEADER);
	fs.w				(&H,sizeof(H));
	fs.close_chunk		();
}

void	OGF::Save_Cached		(IWriter &fs, ogf_header& H, BOOL bVertexColored)
{
//	clMsg			("- saving: cached");
	R_ASSERT		(0);

	fs.open_chunk	(OGF_VERTICES);
	fs.w_u32		(0);
	fs.w_u32		((u32)vertices.size());
	for (itOGF_V V=vertices.begin(); V!=vertices.end(); V++)
	{
						fs.w(&*V,6*sizeof(float));	// Position & normal
		if (bVertexColored)	fs.w(&(V->Color),4);
		for (u32 uv=0; uv<dwRelevantUV; uv++)
			fs.w(&*V->UV.begin()+uv,2*sizeof(float));
	}
	fs.close_chunk	();
	
	// Faces
	fs.open_chunk	(OGF_INDICES);
	fs.w_u32		((u32)faces.size()*3);
	for (itOGF_F F=faces.begin(); F!=faces.end(); F++)	fs.w(&*F,3*sizeof(WORD));
	fs.close_chunk	();
}

void	OGF::Save_Normal_PM		(IWriter &fs, ogf_header& H, BOOL bVertexColored)
{
//	clMsg			("- saving: normal or clod");

	// Vertices
	u32 ID,Start;
	if (b_R2)
	{
#pragma todo("R2 saving incorrect")
		VDeclarator		D;
		D.set			(r2_decl);

		u32 t;
		g_VB.Begin		(D);
		for (itOGF_V V=vertices.begin(); V!=vertices.end(); V++)
		{
			g_VB.Add			(&(V->P),3*sizeof(float));		// Position
			t=u8_vec4(V->N);	g_VB.Add(&t,4);					// Normal
			t=u8_vec4(V->T);	g_VB.Add(&t,4);					// Tangent
			t=u8_vec4(V->B);	g_VB.Add(&t,4);					// Binormal
			// t=V->Color;		g_VB.Add(&t,4);					// Color
			g_VB.Add			(V->UV.begin(),2*sizeof(float));// TC
		}
		g_VB.End		(&ID,&Start);
	} else {
		VDeclarator		D;
		if	(bVertexColored)	
		{
			// vertex-colored
			D.set			(r1_decl_vert);
			g_VB.Begin		(D);
			for (itOGF_V V=vertices.begin(); V!=vertices.end(); V++)
			{
				r1v_vert	v	(V->P,V->N,V->Color,V->UV[0]);
				g_VB.Add		(&v,sizeof(v));
			}
			g_VB.End		(&ID,&Start);
		}
		else
		{
			// lmap-colored
			D.set			(r1_decl_lmap);
			g_VB.Begin		(D);
			for (itOGF_V V=vertices.begin(); V!=vertices.end(); V++)
			{
				r1v_lmap	v	(V->P,V->N,V->Color,V->UV[0],V->UV[1]);
				g_VB.Add		(&v,sizeof(v));
			}
			g_VB.End		(&ID,&Start);
		}
	}
	
	fs.open_chunk	(OGF_VCONTAINER);
	fs.w_u32		(ID);
	fs.w_u32		(Start);
	fs.w_u32		((u32)vertices.size());
	fs.close_chunk	();
	
	// Faces
	g_IB.Register	(LPWORD(&*faces.begin()),LPWORD(&*faces.end()),&ID,&Start);
	fs.open_chunk	(OGF_ICONTAINER);
	fs.w_u32		(ID);
	fs.w_u32		(Start);
	fs.w_u32		((u32)faces.size()*3);
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
			fs.w			(&*pmap_vsplit.begin(),(u32)pmap_vsplit.size()*sizeof(Vsplit));
			fs.close_chunk	();
		}
		{
			fs.open_chunk	(0x3);
			fs.w_u32		((u32)pmap_faces.size());
			fs.w			(&*pmap_faces.begin(),(u32)pmap_faces.size()*sizeof(WORD));
			fs.close_chunk	();
		}
		fs.close_chunk();
	}
}

extern	void xrStripify(xr_vector<WORD> &indices, xr_vector<WORD> &perturb, int iCacheSize, int iMinStripLength);

void	OGF::Save_Progressive	(IWriter &fs, ogf_header& H, BOOL bVertexColored)
{
//	clMsg				("- saving: progressive");

	// Determining the number of samples
	R_ASSERT				(I_Current>=0);
	int verts_for_remove	= (u32)vertices.size()-dwMinVerts;
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
		Save_Normal_PM		(fs,H,bVertexColored);
		return;
	}

	// Perform process
	fs.open_chunk(OGF_P_LODS);
	{
		// Init
		u32					V_Current,V_Minimal,FIX_Current;
		WORD*					faces_affected	= (WORD*)&*pmap_faces.begin();
		Vsplit*					vsplit			= (Vsplit*)&*pmap_vsplit.begin();
		V_Current				= V_Minimal		= dwMinVerts;
		FIX_Current				= 0;
		
		// Cycle on sampes
		for (u32 dwSample=0; dwSample<u32(samples); dwSample++)
		{
			// ***** Perform COLLAPSE or VSPLIT
			u32 dwCount		= dwMinVerts + verts_per_sample*dwSample;
			if (dwSample==u32(samples-1))	dwCount = (u32)vertices.size	();
			
			if (V_Current!=dwCount) {
				WORD* Indices	= (WORD*)&*faces.begin();
				
				// First cycle - try to improve quality
				while (V_Current<dwCount) {
					Vsplit&	S	=	vsplit[V_Current-V_Minimal];
					
					// fixup faces
					u32	dwEnd = u32(S.numFixFaces)+FIX_Current;
					WORD	V_Cur = WORD(V_Current);
					for (u32 I=FIX_Current; I<dwEnd; I++) {
						R_ASSERT(Indices[faces_affected[I]]==S.vsplitVert);
						Indices[faces_affected[I]]=V_Cur;
					};
					
					// correct numbers
					I_Current	+=	3*u32(S.numNewTriangles);
					V_Current	+=	1;
					FIX_Current	+=	S.numFixFaces;
				};
				
				// Second cycle - try to decrease quality
				while (V_Current>dwCount) {
					V_Current	-=	1;
					
					Vsplit&	S	=	vsplit[V_Current-V_Minimal];
					
					// fixup faces
					WORD V_New		= WORD(S.vsplitVert);
					u32 dwEnd		= FIX_Current;
					u32 dwStart	= dwEnd-S.numFixFaces;
					for (u32 I=dwStart; I<dwEnd; I++) {
						R_ASSERT(Indices[faces_affected[I]]==V_Current);
						Indices[faces_affected[I]]=V_New;
					};
					
					// correct numbers
					I_Current	-=	3*u32(S.numNewTriangles);
					FIX_Current	-=	S.numFixFaces;
				};
			}

			// ***** Stripify resulting mesh
			xr_vector<WORD>	strip_indices, strip_permute;
			vecOGF_V		strip_verts;
			try {
				// Stripify
				WORD* strip_F			= (WORD*)&*faces.begin(); 
				strip_indices.assign	(strip_F, strip_F+I_Current);
				strip_permute.resize	(V_Current);
				xrStripify				(strip_indices,strip_permute,c_vCacheSize,0);
				
				// Permute vertices
				strip_verts.resize		(V_Current);
				for(u32 i=0; i<strip_verts.size(); i++)
					strip_verts[i]=vertices[strip_permute[i]];
			} catch (...) {
				clMsg			("ERROR: Stripifying failed.");
			}

			// ***** SAVE
			try {
				// Fill container
				u32 ID,Start;
				g_VB.Begin(0);
				for (itOGF_V V=strip_verts.begin(); V!=strip_verts.end(); V++)
				{
					g_VB.Add(&*V,6*sizeof(float));	// Position & normal
					if (bVertexColored)		g_VB.Add(&(V->Color),4);
					for (u32 uv=0; uv<dwRelevantUV; uv++)
						g_VB.Add(V->UV.begin()+uv,2*sizeof(float));
				}
				g_VB.End(&ID,&Start);

				fs.open_chunk		(dwSample);
				{
					// Vertices
					fs.open_chunk	(OGF_VCONTAINER);
					fs.w_u32		(ID);
					fs.w_u32		(Start);
					fs.w_u32		((u32)strip_verts.size());
					fs.close_chunk	();
					
					// Faces
					fs.open_chunk	(OGF_INDICES);
					fs.w_u32		((u32)strip_indices.size());
					fs.w			(&*strip_indices.begin(),(u32)strip_indices.size()*sizeof(WORD));
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
