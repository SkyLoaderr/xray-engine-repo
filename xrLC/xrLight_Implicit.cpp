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

class ImplicitLumel
{
public:
	Fvector	color;
	u32	marker;
};

class ImplicitDeflector
{
public:
	b_BuildTexture*			texture;
	ImplicitLumel*			lmap;
	ImplicitLumel*			temp;
	vecFace					faces;
	
	ImplicitDeflector() : texture(0),lmap(0),temp(0)
	{
	}
	~ImplicitDeflector()
	{
		Deallocate	();
	}
	
	void			Allocate()
	{
		u32 size	= Width()*Height()*sizeof(ImplicitLumel);
		lmap		= (ImplicitLumel*)xr_malloc	(size);	R_ASSERT(lmap);
		temp		= (ImplicitLumel*)xr_malloc	(size);	R_ASSERT(temp);
		ZeroMemory	(lmap,size);
		ZeroMemory	(temp,size);
	}
	void			Deallocate()
	{
		xr_free		(temp);
		xr_free		(lmap);
	}
	
	u32		Width()	{ return texture->dwWidth; }
	u32		Height(){ return texture->dwHeight; }
	
	u32&	Texel	(u32 x, u32 y)	
	{ 
		return texture->pSurface[y*Width()+x];
	}
	
	ImplicitLumel& Lumel(u32 x, u32 y)	
	{ 
		return lmap[y*Width()+x];
	}
	IC void GET(int x, int y, u32 ref,ImplicitLumel& mix)
	{
		// wrap pixels
		if (x<0) x+=(int)Width();
		else if (x>=(int)Width())	x-=(int)Width();
		if (y<0) y+=(int)Height();
		else if (y>=(int)Height())	y-=(int)Height();
		
		// summarize
		ImplicitLumel&	L = Lumel	(x,y);
		if (L.marker<=ref)			return;
		
		mix.color.add	(L.color);
		mix.marker		+= 1;
	}
	
	BOOL ApplyBorders(u32 ref) 
	{
		BOOL bNeedContinue = FALSE;
		
		CopyMemory(temp,lmap,Width()*Height()*sizeof(ImplicitLumel));
		for (int y=0; y<(int)Height(); y++) {
			for (int x=0; x<(int)Width(); x++)
			{
				ImplicitLumel&	L	= Lumel(x,y);
				if (L.marker==0)
				{
					ImplicitLumel	mix;
					mix.color.set	(0,0,0);
					mix.marker		= 0;
					GET(x-1,y-1,ref,mix);
					GET(x  ,y-1,ref,mix);
					GET(x+1,y-1,ref,mix);
					
					GET(x-1,y  ,ref,mix);
					GET(x+1,y  ,ref,mix);
					
					GET(x-1,y+1,ref,mix);
					GET(x  ,y+1,ref,mix);
					GET(x+1,y+1,ref,mix);
					
					if (mix.marker) 
					{
						mix.color.div		(float(mix.marker));
						mix.marker			= ref;
						temp[y*Width()+x]	= mix;
						bNeedContinue		= TRUE;
					}
				}
			}
		}
		CopyMemory(lmap,temp,Width()*Height()*sizeof(ImplicitLumel));
		return bNeedContinue;
	}
	
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
		ImplicitDeflector& defl = *DATA;
		vector<R_Light>			Lights	= pBuild->L_layers.front().lights;
		CDB::COLLIDER			DB;
		
		// Setup variables
		Fvector2	dim,half;
		dim.set		(float(defl.Width()),float(defl.Height()));
		half.set	(.5f/dim.x,.5f/dim.y);
		
		// Jitter data
		Fvector2	JS;
		JS.set		(g_params.m_lm_jitter/dim.x, g_params.m_lm_jitter/dim.y);
		u32		Jcount;
		Fvector2*	Jitter;
		Jitter_Select(Jitter, Jcount);
		
		// Lighting itself
		DB.ray_options	(0);
		Fcolor			C[9];
		for (u32 J=0; J<9; J++)	C[J].set(0,0,0,0);
		for (u32 V=y_start; V<y_end; V++)
		{
			for (u32 U=0; U<defl.Width(); U++)
			{
				u32		Fcount	= 0;
				
				try {
					for (J=0; J<Jcount; J++) 
					{
						// LUMEL space
						Fvector2 P;
						P.x = float(U)/dim.x + half.x + Jitter[J].x * JS.x;
						P.y	= float(V)/dim.y + half.y + Jitter[J].y * JS.y;
						vector<Face*>&	space	= ImplicitHash->query(P.x,P.y);
						
						// World space
						Fvector wP,wN,B;
						C[J].set(0,0,0,0);
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
								LightPoint	(&DB, RCAST_Model, C[J], wP, wN, Lights.begin(), Lights.end(), F);
								Fcount		++;
							}
						}
					} 
				} catch (...)
				{
					clMsg("* THREAD #%d: Access violation. Possibly recovered.",thID);
				}
				/*
				C[0].set(1,1,1,1);
				C[1].set(1,1,1,1);
				C[2].set(1,1,1,1);
				C[3].set(1,1,1,1);
				C[4].set(1,1,1,1);
				C[5].set(1,1,1,1);
				C[6].set(1,1,1,1);
				C[7].set(1,1,1,1);
				C[8].set(1,1,1,1);
				Fcount		= 1;
				*/
				if (Fcount) {
					// Calculate lighting amount
					Fcolor		Lumel,R;
					float cnt	= float(Fcount);
					R.r =		(C[0].r + C[1].r + C[2].r + C[3].r + C[4].r + C[5].r + C[6].r + C[7].r + C[8].r)/cnt;
					R.g =		(C[0].g + C[1].g + C[2].g + C[3].g + C[4].g + C[5].g + C[6].g + C[7].g + C[8].g)/cnt;
					R.b	=		(C[0].b + C[1].b + C[2].b + C[3].b + C[4].b + C[5].b + C[6].b + C[7].b + C[8].b)/cnt;
					Lumel.lerp	(R,g_params.m_lm_amb_color,g_params.m_lm_amb_fogness);
					Lumel.a		= 1.f;
					
					ImplicitLumel& L = defl.Lumel(U,V);
					L.color.x	= Lumel.r;
					L.color.y	= Lumel.g;
					L.color.z	= Lumel.b;
					L.marker	= 255;
				} else {
					defl.Lumel	(U,V).marker=0;
				}
			}
			thProgress	= float(V - y_start) / float(y_end-y_start);
		}
	}
};

//#pragma optimize( "g", off )

void VerifyPath	(LPCSTR path)
{
	char tmp[MAX_PATH];
	for(int i=0;path[i];i++){
		if( path[i]!='\\' || i==0 )
			continue;
		CopyMemory		( tmp, path, i );
		tmp[i] = 0;
		CreateDirectory	( tmp, 0 );
	}
}

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
			calculator.insert	(make_pair(Tid,ImpD));
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
		u32	stride			= defl.Height()/NUM_THREADS;
		for (u32 thID=0; thID<NUM_THREADS; thID++)
			tmanager.start		(xr_new<ImplicitThread> (thID,&defl,thID*stride,thID*stride+stride));
		tmanager.wait			();

		// Expand
		Status	("Processing lightmap...");
		for (u32 ref=254; ref>0; ref--)	if (!defl.ApplyBorders(ref)) break;

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
					ImplicitLumel&	L = defl.Lumel(U,V);
					
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
