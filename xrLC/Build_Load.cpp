#include "stdafx.h"

b_params				g_params;
extern u32				dwInvalidFaces;

template <class T>
void transfer(const char *name, vector<T> &dest, CStream& F, u32 chunk)
{
	CStream*	O		= F.OpenChunk(chunk);
	u32		count	= O?(O->Length()/sizeof(T)):0;
	clMsg			("* %16s: %d",name,count);
	if (count)  
	{
		dest.reserve(count);
		dest.insert	(dest.begin(), (T*)O->Pointer(), (T*)O->Pointer() + count);
	}
	if (O)		O->Close	();
}

extern u32*		Surface_Load	(char* name, u32& w, u32& h);
extern void		Surface_Init	();

/*
vector<R_Layer>*	CBuild::LLayer_by_name	(LPCSTR N)
{
for (u32 L=0; L<L_layers.size(); L++)
{
if (0==stricmp(L_layers[L].control.name,N))	return &(L_layers[L].lights);
}
return 0;
}
*/

void CBuild::Load	(const b_params& Params, const CStream& _in_FS)
{
	CStream&	FS	= const_cast<CStream&>(_in_FS);
	// HANDLE		hLargeHeap	= HeapCreate(0,64*1024*1024,0);
	// clMsg		("* <LargeHeap> handle: %X",hLargeHeap);

	u32			i			= 0;

	float			p_total		= 0;
	float			p_cost		= 1.f/3.f;
	
	CStream*		F			= 0;

	// 
	shaders.Load				("gamedata\\shaders_xrlc.xr");
	
	//*******
	Status					("Vertices...");
	{
		F = FS.OpenChunk		(EB_Vertices);
		u32 v_count			=	F->Length()/sizeof(b_vertex);
		g_vertices.reserve		(3*v_count/2);
		scene_bb.invalidate		();
		for (i=0; i<v_count; i++)
		{
			Vertex*	pV			= VertexPool.create();
			F->Rvector			(pV->P);
			pV->N.set			(0,0,0);
			pV->Color.set		(0,0,0,0);
			scene_bb.modify		(pV->P);
		}
		Progress			(p_total+=p_cost);
		clMsg				("* %16s: %d","vertices",g_vertices.size());
		F->Close			();
	}

	//*******
	Status					("Faces...");
	{
		F = FS.OpenChunk		(EB_Faces);
		R_ASSERT				(F);
		u32 f_count			=	F->Length()/sizeof(b_face);
		g_faces.reserve			(f_count);
		for (i=0; i<f_count; i++)
		{
			try 
			{
				Face*	_F			= FacePool.create();
				b_face	B;
				F->Read				(&B,sizeof(B));

				_F->dwMaterial		= WORD(B.dwMaterial);
				_F->dwMaterialGame	= B.dwMaterialGame;

				// Vertices and adjacement info
				for (u32 it=0; it<3; it++)
				{
					int id			= B.v[it];
					R_ASSERT		(id<(int)g_vertices.size());
					_F->SetVertex	(it,g_vertices[id]);
				}

				// transfer TC
				Fvector2				uv1,uv2,uv3;
				uv1.set				(B.t[0].x,B.t[0].y);
				uv2.set				(B.t[1].x,B.t[1].y);
				uv3.set				(B.t[2].x,B.t[2].y);
				_F->AddChannel		( uv1, uv2, uv3 );
			} catch (...)
			{
				err_save		();
				Debug.fatal		("* ERROR: Can't process face #%d",i);
			}
		}
		Progress			(p_total+=p_cost);
		clMsg				("* %16s: %d","faces",g_faces.size());
		F->Close			();

		if (dwInvalidFaces)	
		{
			err_save		();
			Debug.fatal		("* FATAL: %d invalid faces. Compilation aborted",dwInvalidFaces);
		}
	}

	//*******
	Status	("Models and References");
	F = FS.OpenChunk		(EB_MU_models);
	if (F)
	{
		while (!F->Eof())
		{
			mu_models.push_back				(xr_new<xrMU_Model>());
			mu_models.back()->Load			(*F);
		}
		F->Close				();
	}
	F = FS.OpenChunk		(EB_MU_refs);
	if (F)
	{
		while (!F->Eof())
		{
			mu_refs.push_back				(xr_new<xrMU_Reference>());
			mu_refs.back()->Load			(*F);
		}		
		F->Close				();
	}

	//*******
	Status	("Other transfer...");
	transfer("materials",	materials,			FS,		EB_Materials);
	transfer("shaders",		shader_render,		FS,		EB_Shaders_Render);
	transfer("shaders_xrlc",shader_compile,		FS,		EB_Shaders_Compile);
	transfer("glows",		glows,				FS,		EB_Glows);
	transfer("portals",		portals,			FS,		EB_Portals);
	transfer("LODs",		lods,				FS,		EB_LOD_models);

	// Load lights
	Status	("Loading lights...");
	{
		// Controlles/Layers
		{
			F = FS.OpenChunk		(EB_Light_control);
			L_control_data.assign	(LPBYTE(F->Pointer()),LPBYTE(F->Pointer())+F->Length());

			R_Layer			temp;
			
			while (!F->Eof())
			{
				F->Read				(temp.control.name,sizeof(temp.control.name));
				u32 cnt				= F->Rdword();
				temp.control.data.resize(cnt);
				F->Read				(temp.control.data.begin(),cnt*sizeof(u32));

				L_layers.push_back	(temp);
			}

			F->Close		();
		}
		// Static
		{
			F = FS.OpenChunk(EB_Light_static);
			b_light_static	temp;
			u32 cnt			= F->Length()/sizeof(temp);
			for				(i=0; i<cnt; i++)
			{
				R_Light		RL;
				F->Read		(&temp,sizeof(temp));
				Flight&		L = temp.data;

				// type
				if			(L.type == D3DLIGHT_DIRECTIONAL)	RL.type	= LT_DIRECT;
				else											RL.type = LT_POINT;

				// generic properties
				RL.diffuse.normalize_rgb	(L.diffuse);
				RL.position.set				(L.position);
				RL.direction.normalize_safe	(L.direction);
				RL.range				=	L.range*1.1f;
				RL.range2				=	RL.range*RL.range;
				RL.attenuation0			=	L.attenuation0;
				RL.attenuation1			=	L.attenuation1;
				RL.attenuation2			=	L.attenuation2;
				RL.energy				=	L.diffuse.magnitude_rgb	();

				// place into layer
				R_ASSERT	(temp.controller_ID<L_layers.size());
				L_layers	[temp.controller_ID].lights.push_back	(RL);
			}
			F->Close		();
		}

		// ***Search HEMI***
		for (u32 LH=0; LH<L_layers.size(); LH++)
		{
			R_Layer&	TEST	= L_layers[LH];
			if (0=stricmp(TEST.control.name,LCONTROL_HEMI))
			{
				// Hemi found
				L_hemi	= TEST.lights;
			}
		}
		clMsg	("HEMI: %d lights",L_hemi.size());

		// ***Hack*** to merge all layers into one
		for (u32 H=1; H<L_layers.size(); H++)
			L_layers[0].lights.insert(L_layers[0].lights.end(),L_layers[H].lights.begin(),L_layers[H].lights.end());
		L_layers.erase	(L_layers.begin()+1,L_layers.end());

		// Dynamic
		transfer("d-lights",	L_dynamic,			FS,		EB_Light_dynamic);
	}
	
	// process textures
	Status			("Processing textures...");
	{
		Surface_Init		();
		F = FS.OpenChunk	(EB_Textures);
		u32 tex_count	= F->Length()/sizeof(b_texture);
		for (u32 t=0; t<tex_count; t++)
		{
			Progress		(float(t)/float(tex_count));

			b_texture		TEX;
			F->Read			(&TEX,sizeof(TEX));

			b_BuildTexture	BT;
			CopyMemory		(&BT,&TEX,sizeof(TEX));

			// load thumbnail
			LPSTR N			= BT.name;
			if (strchr(N,'.')) *(strchr(N,'.')) = 0;
			strlwr			(N);
			char th_name[256]; strconcat(th_name,"\\\\x-ray\\stalkerdata$\\textures\\",N,".thm");
			CCompressedStream THM	(th_name,THM_SIGN);

			// analyze thumbnail information
			R_ASSERT		(THM.ReadChunk(THM_CHUNK_TEXTUREPARAM,&BT.THM));
			BOOL			bLOD=FALSE;
			if (N[0]=='l' && N[1]=='o' && N[2]=='d' && N[3]=='\\') bLOD = TRUE;

			// load surface if it has an alpha channel or has "implicit lighting" flag
			BT.dwWidth	= BT.THM.width;
			BT.dwHeight	= BT.THM.height;
			BT.bHasAlpha= BT.THM.HasAlphaChannel();
			if (!bLOD) 
			{
				if (BT.bHasAlpha || BT.THM.flags.test(STextureParams::flImplicitLighted))
				{
					clMsg		("- loading: %s",N);
					u32			w=0, h=0;
					BT.pSurface = Surface_Load(N,w,h);
					BT.Vflip	();
				} else {
					// Free surface memory
				}
			}

			// save all the stuff we've created
			textures.push_back	(BT);
		}
	}

	// post-process materials
	Status	("Post-process materials...");
	for (u32 m=0; m<materials.size(); m++)
	{
		b_material &M	= materials[m];

		if (65535==M.shader_xrlc)	{
			// No compiler shader
			M.reserved	= WORD(-1);
			clMsg	(" *  %20s",shader_render[M.shader].name);
		} else {
			clMsg	(" *  %20s / %-20s",shader_render[M.shader].name, shader_compile[M.shader_xrlc].name);
			int id = shaders.GetID(shader_compile[M.shader_xrlc].name);
			if (id<0) {
				clMsg	("ERROR: Shader '%s' not found in library",shader_compile[M.shader].name);
				R_ASSERT(id>=0);
			}
			M.reserved = WORD(id);
		}
	}
	Progress(p_total+=p_cost);

	// Parameter block
	CopyMemory(&g_params,&Params,sizeof(b_params));
	g_params.m_SS_DedicateCached	= 8;
	g_params.m_bConvertProgressive	= FALSE;

	// 
	clMsg	("* sizes: V(%d),F(%d)",sizeof(Vertex),sizeof(Face));
}

