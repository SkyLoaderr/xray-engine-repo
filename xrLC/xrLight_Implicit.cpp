#include "stdafx.h"
#include "build.h"
#include "tga.h"
#include "xrThread.h"
#include "hash2D.h"

BOOL	hasImplicitLighting(Face* F)
{
	if (0==F)									return FALSE;
	if (!F->Shader().flags.bRendering)			return FALSE;
	b_material& M = pBuild->materials			[F->dwMaterial];
	b_BuildTexture&	T  = pBuild->textures		[M.surfidx];
	return (T.THM.flags.test(STextureParams::flImplicitLighted));
}

class ImplicitDeflector
{
public:
	b_BuildTexture*			texture;
	lm_layer				lmap;
	vecFace					faces;
	
	ImplicitDeflector() : texture(0)
	{
	}
	~ImplicitDeflector()
	{
		Deallocate	();
	}
	
	void			Allocate	()
	{
		lmap.create	(Width(),Height());
	}
	void			Deallocate	()
	{
		lmap.destroy();
	}
	
	u32		Width()	{ return texture->dwWidth; }
	u32		Height(){ return texture->dwHeight; }
	
	u32&		Texel	(u32 x, u32 y)			{ return texture->pSurface[y*Width()+x]; }
	base_color& Lumel	(u32 x, u32 y)			{ return lmap.surface[y*Width()+x];	}
	u8&			Marker	(u32 x, u32 y)			{ return lmap.marker [y*Width()+x];	}
	
	void	Bounds	(u32 ID, Fbox2& dest)
	{
		Face* F		= faces[ID];
		_TCF& TC	= F->tc[0];
		dest.min.set	(TC.uv[0]);
		dest.max.set	(TC.uv[0]);
		dest.modify		(TC.uv[1]);
		dest.modify		(TC.uv[2]);
	}
	void	Bounds_Summary (Fbox2& bounds)
	{
		bounds.invalidate();
		for (u32 I=0; I<faces.size(); I++)
		{
			Fbox2	B;
			Bounds	(I,B);
			bounds.merge(B);
		}
	}
};


DEF_MAP(Implicit,u32,ImplicitDeflector);

typedef hash2D <Face*,384,384>		IHASH;
static IHASH*						ImplicitHash;

class ImplicitThread : public CThread
{
public:
	ImplicitDeflector*	DATA;			// Data for this thread
	u32					y_start,y_end;

	ImplicitThread		(u32 ID, ImplicitDeflector* _DATA, u32 _y_start, u32 _y_end) : CThread (ID)
	{
		DATA			= _DATA;
		y_start			= _y_start;
		y_end			= _y_end;
	}
	virtual void		Execute	()
	{
		R_ASSERT				(DATA);
		ImplicitDeflector&		defl	= *DATA;
		CDB::COLLIDER			DB;
		
		// Setup variables
		Fvector2	dim,half;
		dim.set		(float(defl.Width()),float(defl.Height()));
		half.set	(.5f/dim.x,.5f/dim.y);
		
		// Jitter data
		Fvector2	JS;
		JS.set		(g_params.m_lm_jitter/dim.x, g_params.m_lm_jitter/dim.y);
		u32			Jcount;
		Fvector2*	Jitter;
		Jitter_Select(Jitter, Jcount);
		
		// Lighting itself
		DB.ray_options	(0);
		for (u32 V=y_start; V<y_end; V++)
		{
			for (u32 U=0; U<defl.Width(); U++)
			{
				base_color	C;
				u32			Fcount	= 0;
				
				try {
					for (u32 J=0; J<Jcount; J++) 
					{
						// LUMEL space
						Fvector2				P;
						P.x						= float(U)/dim.x + half.x + Jitter[J].x * JS.x;
						P.y						= float(V)/dim.y + half.y + Jitter[J].y * JS.y;
						xr_vector<Face*>& space	= ImplicitHash->query(P.x,P.y);
						
						// World space
						Fvector wP,wN,B;
						for (vecFaceIt it=space.begin(); it!=space.end(); it++)
						{
							Face	*F	= *it;
							_TCF&	tc	= F->tc[0];
							if (tc.isInside(P,B)) 
							{
								// We found triangle and have barycentric coords
								Vertex	*V1 = F->v[0];
								Vertex	*V2 = F->v[1];
								Vertex	*V3 = F->v[2];
								wP.from_bary(V1->P,V2->P,V3->P,B);
								wN.from_bary(V1->N,V2->N,V3->N,B);
								wN.normalize();
								LightPoint	(&DB, RCAST_Model, C, wP, wN, pBuild->L_static, LP_dont_hemi, F);
								Fcount		++;
							}
						}
					} 
				} catch (...)
				{
					clMsg("* THREAD #%d: Access violation. Possibly recovered.",thID);
				}
				if (Fcount) {
					// Calculate lighting amount
					C.scale				(Fcount);
					defl.Lumel(U,V)		= C;
					defl.Marker(U,V)	= 255;
				} else {
					defl.Marker	(U,V)	= 0;
				}
			}
			thProgress	= float(V - y_start) / float(y_end-y_start);
		}
	}
};

//#pragma optimize( "g", off )

#define	NUM_THREADS	8
void CBuild::ImplicitLighting()
{
	Implicit		calculator;
	ImplicitHash	= xr_new<IHASH>	();
	
	// Sorting
	Status("Sorting faces...");
	for (vecFaceIt I=g_faces.begin(); I!=g_faces.end(); I++)
	{
		Face* F = *I;
		if (F->pDeflector)	continue;
		if (!hasImplicitLighting(F)) continue;
		
		Progress (float(I-g_faces.begin())/float(g_faces.size()));
		b_material&		M	= materials		[F->dwMaterial];
		u32				Tid = M.surfidx;
		b_BuildTexture*	T	= &(textures[Tid]);
		
		Implicit_it		it	= calculator.find(Tid);
		if (it==calculator.end()) 
		{
			ImplicitDeflector	ImpD;
			ImpD.texture		= T;
			ImpD.faces.push_back(F);
			calculator.insert	(mk_pair(Tid,ImpD));
		} else {
			ImplicitDeflector&	ImpD = it->second;
			ImpD.faces.push_back(F);
		}
	}
	
	// Lighing
	for (Implicit_it imp=calculator.begin(); imp!=calculator.end(); imp++)
	{
		ImplicitDeflector& defl = imp->second;
		Status			("Lighting implicit map '%s'...",defl.texture->name);
		Progress		(0);
		defl.Allocate	();
		
		// Setup cache
		Progress					(0);
		Fbox2 bounds;
		defl.Bounds_Summary			(bounds);
		ImplicitHash->initialize	(bounds,defl.faces.size());
		for (u32 fid=0; fid<defl.faces.size(); fid++)
		{
			Face* F				= defl.faces[fid];
			defl.Bounds			(fid,bounds);
			ImplicitHash->add	(bounds,F);
		}

		// Start threads
		CThreadManager			tmanager;
		u32	stride				= defl.Height()/NUM_THREADS;
		for (u32 thID=0; thID<NUM_THREADS; thID++)
			tmanager.start		(xr_new<ImplicitThread> (thID,&defl,thID*stride,thID*stride+stride));
		tmanager.wait			();

		// Expand
		Status	("Processing lightmap...");
		for (u32 ref=254; ref>0; ref--)	if (!ApplyBorders(defl.lmap,ref)) break;

		Status	("Mixing lighting with texture...");
		u32*	markup = (u32*)(xr_malloc(defl.Height()*defl.Width()*4));
		{
			for (u32 V=0; V<defl.Height(); V++)
			{
				for (u32 U=0; U<defl.Width(); U++)
				{
					// Retreive Texel
					u32 &OLD	= defl.Texel(U,V);
					Fcolor		cccT;
					cccT.set	(OLD);

					// Retreive Lumel
					base_color&	L = defl.Lumel(U,V);
					
					// Modulate & save
					cccT.r		*= L.color.x;
					cccT.g		*= L.color.y;
					cccT.b		*= L.color.z;
					OLD			= cccT.get	();

					// Marker table
					markup[V*defl.Width()+U]	= D3DCOLOR_XRGB(L.marker,L.marker,L.marker);
				}
			}
		}
		
		Status	("Saving...");
		// Save local copy of the map
		{
			char	name[256];
			sscanf(strstr(GetCommandLine(),"-f")+2,"%s",name);
			b_BuildTexture& TEX		= *defl.texture;
			char	out_name[256];	strconcat(out_name,"gamedata\\levels\\",name,"\\",TEX.name,".dds");
			clMsg		("Saving texture '%s'...",out_name);
			VerifyPath				(out_name);
			BYTE*	raw_data		= LPBYTE(TEX.pSurface);
			u32	w					= TEX.dwWidth;
			u32	h					= TEX.dwHeight;
			u32	pitch				= w*4;
			STextureParams* fmt		= &(TEX.THM);
			DXTCompress				(out_name,raw_data,w,h,pitch,fmt,4);
		}
		/*
		// Save markup
		{
			char	name[256];
			sscanf(strstr(GetCommandLine(),"-f")+2,"%s",name);
			b_BuildTexture& TEX		= *defl.texture;
			char	out_name[256];	strconcat(out_name,"\\",TEX.name,"_m");
			clMsg		("Saving markup '%s'...",out_name);

			TGAdesc			p;
			p.format		= IMG_24B;
			p.scanlenght	= TEX.dwWidth*4;
			p.width			= TEX.dwWidth;
			p.height		= TEX.dwHeight;
			p.data			= markup;
			p.maketga		(out_name);
		}
		xr_free(markup)
		*/
		xr_free						(markup);
		defl.Deallocate				();
	}

	xr_delete			(ImplicitHash);
	calculator.clear	();
}
