#include "stdafx.h"
#include "build.h"
#include "ogf_face.h"
#include "std_classes.h"
#include "fs.h"
#include "fmesh.h"

using namespace std;

extern u16	RegisterShader		(LPCSTR T);
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
u32						u8_vec4			(base_basis N, u8 A=0)
{
	return				color_rgba		(N.x,N.y,N.z,A);
}
s16						s16_tc_lmap		(float uv)		// [-1 .. +1]
{
	const u32	max_tile	=	1;
	const s32	quant		=	32768/max_tile;

	s32			t			=	iFloor	(uv*float(quant)); clamp(t,-32768,32767);
	return	s16	(t);
}

D3DVERTEXELEMENT9		r1_decl_lmap	[] =	// 12+4+4+4+8	= 24 / 28
{
	{0, 0,  D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,	0 },
	{0, 12, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,	0 },
	{0, 16, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TANGENT,	0 },
	{0, 20, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_BINORMAL,	0 },
	{0, 24, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	0 },
	{0, 32, D3DDECLTYPE_SHORT2,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	1 },
	D3DDECL_END()
};
D3DVERTEXELEMENT9		r1_decl_vert	[] =	// 12+4+4+4 = 24 / 28
{
	{0, 0,  D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_POSITION,	0 },
	{0, 12, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_NORMAL,	0 },
	{0, 16, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TANGENT,	0 },
	{0, 20, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_BINORMAL,	0 },
	{0, 24, D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_COLOR,		0 },
	{0, 28, D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT, 	D3DDECLUSAGE_TEXCOORD,	0 },
	D3DDECL_END()
};
#pragma pack(push,1)
struct  r1v_lmap	{
	Fvector3	P;
	u32			N;
	u32			T;
	u32			B;
	float		tc0x,tc0y;
	s16			tc1x,tc1y;

	r1v_lmap	(Fvector3 _P, Fvector _N, base_basis _T, base_basis _B, base_color _CC, Fvector2 tc_base, Fvector2 tc_lmap )
	{
		base_color_c	_C;	_CC._get	(_C);
		_N.normalize	();
		P				= _P;
		N				= u8_vec4		(_N,u8_clr(_C.hemi));
		T				= u8_vec4		(_T);	//.
		B				= u8_vec4		(_B);
		tc0x			= tc_base.x;
		tc0y			= tc_base.y;
		tc1x			= s16_tc_lmap	(tc_lmap.x);
		tc1y			= s16_tc_lmap	(tc_lmap.y);
	}
};
struct  r1v_vert	{
	Fvector3	P;
	u32			N;
	u32			T;
	u32			B;
	u32			C;
	float		tc0x,tc0y;

	r1v_vert	(Fvector3 _P, Fvector _N, base_basis _T, base_basis _B, base_color _CC, Fvector2 tc_base)
	{
		base_color_c	_C;	_CC._get	(_C);
		_N.normalize	();
		P				= _P;
		N				= u8_vec4		(_N,u8_clr(_C.hemi));
		T				= u8_vec4		(_T);	//.
		B				= u8_vec4		(_B);
		C				= color_rgba	(u8_clr(_C.rgb.x),u8_clr(_C.rgb.y),u8_clr(_C.rgb.z),u8_clr(_C.sun));
		tc0x			= tc_base.x;
		tc0y			= tc_base.y;
	}
};
#pragma pack(pop)

void OGF::Save			(IWriter &fs)
{
	OGF_Base::Save		(fs);

	// clMsg			("* %d faces",faces.size());
	geom_batch_average	((u32)vertices.size(),(u32)faces.size());

	// Texture & shader
	std::string			Tname;
	for (u32 i=0; i<textures.size(); i++)	{
		if (!Tname.empty()) Tname += ',';
		string256		t;
		strcpy			(t,*textures[i].name);
		if (strchr(t,'.')) *strchr(t,'.')=0;
		Tname			+= t;
	}
	string1024			sid;
	strconcat			(sid,
		pBuild->shader_render[pBuild->materials[material].shader].name,
		"/",
		Tname.c_str()
		);

	// Create header
	ogf_header			H;
	H.format_version	= xrOGF_FormatVersion;
	H.type				= MT_NORMAL;
	H.shader_id			= RegisterShader			(sid);
	H.bb.min			= bbox.min;
	H.bb.max			= bbox.max;
	H.bs.c				= C;
	H.bs.r				= R;

	// Vertices
	Shader_xrLC*	SH	=	pBuild->shaders.Get		(pBuild->materials[material].reserved);
	bool bVertexColors	=	(SH->flags.bLIGHT_Vertex);
	
	switch (H.type) 
	{
	case MT_NORMAL:
	case MT_PROGRESSIVE:
		Save_Normal_PM	(fs,H,bVertexColors);		
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

	// Texture & shader
	std::string			Tname;
	for (u32 i=0; i<textures.size(); i++)
	{
		if (!Tname.empty()) Tname += ',';
		string256		t;
		strcpy			(t,*textures[i].name);
		if (strchr(t,'.')) *strchr(t,'.')=0;
		Tname			+= t;
	}
	string1024			sid;
	strconcat			(sid,
		pBuild->shader_render[pBuild->materials[material].shader].name,
		"/",
		Tname.c_str()
		);

	// Create header
	ogf_header			H;
	H.format_version	= xrOGF_FormatVersion;
	H.type				= MT_TREE;
	H.shader_id			= RegisterShader	(sid);
	H.bb.min			= bbox.min;
	H.bb.max			= bbox.max;
	H.bs.c				= C;
	H.bs.r				= R;

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
	for (itOGF_F F=faces.begin(); F!=faces.end(); F++)	fs.w(&*F,3*sizeof(u16));
	fs.close_chunk	();
}

void	OGF::PreSave			()
{
	Shader_xrLC*	SH	=	pBuild->shaders.Get		(pBuild->materials[material].reserved);
	bool bVertexColored	=	(SH->flags.bLIGHT_Vertex);

	// Vertices
	VDeclarator		D;
	if	(bVertexColored){
		// vertex-colored
		D.set			(r1_decl_vert);
		g_VB.Begin		(D);
		for (itOGF_V V=vertices.begin(); V!=vertices.end(); V++)
		{
			r1v_vert	v	(V->P,V->N,V->T,V->B,V->Color,V->UV[0]);
			g_VB.Add		(&v,sizeof(v));
		}
		g_VB.End		(&vb_id,&vb_start);
	}else{
		// lmap-colored
		D.set			(r1_decl_lmap);
		g_VB.Begin		(D);
		for (itOGF_V V=vertices.begin(); V!=vertices.end(); V++)
		{
			r1v_lmap	v	(V->P,V->N,V->T,V->B,V->Color,V->UV[0],V->UV[1]);
			g_VB.Add		(&v,sizeof(v));
		}
		g_VB.End		(&vb_id,&vb_start);
	}

	// Faces
	g_IB.Register		(LPWORD(&*faces.begin()),LPWORD(&*faces.end()),&ib_id,&ib_start);
}

void	OGF::Save_Normal_PM		(IWriter &fs, ogf_header& H, BOOL bVertexColored)
{
//	clMsg			("- saving: normal or clod");

	// Vertices
	fs.open_chunk	(OGF_VCONTAINER);
	fs.w_u32		(vb_id);
	fs.w_u32		(vb_start);
	fs.w_u32		((u32)vertices.size());
	fs.close_chunk	();
	
	// Faces
	fs.open_chunk	(OGF_ICONTAINER);
	fs.w_u32		(ib_id);
	fs.w_u32		(ib_start);
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
			fs.w			(&*pmap_faces.begin(),(u32)pmap_faces.size()*sizeof(u16));
			fs.close_chunk	();
		}
		fs.close_chunk();
	}
}

extern	void xrStripify(xr_vector<u16> &indices, xr_vector<u16> &perturb, int iCacheSize, int iMinStripLength);

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
		u32						V_Current,V_Minimal,FIX_Current;
		u16*					faces_affected	= (u16*)&*pmap_faces.begin();
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
				u16* Indices	= (u16*)&*faces.begin();
				
				// First cycle - try to improve quality
				while (V_Current<dwCount) {
					Vsplit&	S	=	vsplit[V_Current-V_Minimal];
					
					// fixup faces
					u32	dwEnd = u32(S.numFixFaces)+FIX_Current;
					u16	V_Cur = u16(V_Current);
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
					u16 V_New		= u16(S.vsplitVert);
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
			xr_vector<u16>	strip_indices, strip_permute;
			vecOGF_V		strip_verts;
			try {
				// Stripify
				u16* strip_F			= (u16*)&*faces.begin(); 
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
					fs.w			(&*strip_indices.begin(),(u32)strip_indices.size()*sizeof(u16));
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
