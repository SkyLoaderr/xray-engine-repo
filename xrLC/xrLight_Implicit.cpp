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
	return (T.THM.flag&STextureParams::flImplicitLighted);
}

class ImplicitLumel
{
public:
	Fvector	color;
	DWORD	marker;
};

class ImplicitDeflector
{
public:
	b_BuildTexture*			texture;
	ImplicitLumel*			lmap;
	ImplicitLumel*			temp;
	vecFace					faces;
	
	ImplicitDeflector() : texture(0),lmap(0)
	{
	}
	
	void			Allocate()
	{
		DWORD size	= Width()*Height()*sizeof(ImplicitLumel);
		lmap		= (ImplicitLumel*)xr_malloc	(size);	R_ASSERT(lmap);
		temp		= (ImplicitLumel*)xr_malloc	(size);	R_ASSERT(temp);
		ZeroMemory	(lmap,size);
		ZeroMemory	(temp,size);
	}
	void			Deallocate()
	{
		_FREE		(temp);
		_FREE		(lmap);
	}
	
	DWORD	Width()	{ return texture->dwWidth; }
	DWORD	Height(){ return texture->dwHeight; }
	
	DWORD&	Texel	(DWORD x, DWORD y)	
	{ 
		return texture->pSurface[y*Width()+x];
	}
	
	ImplicitLumel& Lumel(DWORD x, DWORD y)	
	{ 
		return lmap[y*Width()+x];
	}
	IC void GET(int x, int y, DWORD ref,ImplicitLumel& mix)
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
	
	BOOL ApplyBorders(DWORD ref) 
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
	
	void	Bounds	(DWORD ID, Fbox2& dest)
	{
		Face* F		= faces[ID];
		_TCF& TC	= F->tc[0];
		dest.min.set	(TC.uv[0].conv());
		dest.max.set	(TC.uv[0].conv());
		dest.modify		(TC.uv[1].conv());
		dest.modify		(TC.uv[2].conv());
	}
	void	Bounds_Summary (Fbox2& bounds)
	{
		bounds.invalidate();
		for (DWORD I=0; I<faces.size(); I++)
		{
			Fbox2	B;
			Bounds	(I,B);
			bounds.merge(B);
		}
	}
};


DEF_MAP(Implicit,DWORD,ImplicitDeflector);

static hash2D <Face*,384,384>	ImplicitHash;

class ImplicitThread : public CThread
{
public:
	ImplicitDeflector*	DATA;			// Data for this thread
	DWORD				y_start,y_end;

	ImplicitThread		(DWORD ID, ImplicitDeflector* _DATA, DWORD _y_start, DWORD _y_end) : CThread (ID)
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
		UVpoint		dim,half;
		dim.set		(float(defl.Width()),float(defl.Height()));
		half.set	(.5f/dim.u,.5f/dim.v);
		
		// Jitter data
		UVpoint		JS;
		JS.set		(g_params.m_lm_jitter/dim.u, g_params.m_lm_jitter/dim.v);
		DWORD		Jcount;
		UVpoint*	Jitter;
		Jitter_Select(Jitter, Jcount);
		
		// Lighting itself
		DB.ray_options	(0);
		Fcolor			C[9];
		for (DWORD J=0; J<9; J++)	C[J].set(0,0,0,0);
		for (DWORD V=y_start; V<y_end; V++)
		{
			for (DWORD U=0; U<defl.Width(); U++)
			{
				DWORD		Fcount	= 0;
				
				try {
					for (J=0; J<Jcount; J++) 
					{
						// LUMEL space
						UVpoint P;
						P.u = float(U)/dim.u + half.u + Jitter[J].u * JS.u;
						P.v	= float(V)/dim.v + half.v + Jitter[J].v * JS.v;
						vector<Face*>&	space	= ImplicitHash.query(P.u,P.v);
						
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
								LightPoint	(&DB, C[J], wP, wN, Lights.begin(), Lights.end(), F);
								Fcount		++;
							}
						}
					} 
				} catch (...)
				{
					Msg("* THREAD #%d: Access violation. Possibly recovered.",thID);
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
		memcpy( tmp, path, i );
		tmp[i] = 0;
		CreateDirectory( tmp, 0 );
	}
}

#define	NUM_THREADS	8
void CBuild::ImplicitLighting()
{
	Implicit	calculator;
	
	// Sorting
	Status("Sorting faces...");
	for (vecFaceIt I=g_faces.begin(); I!=g_faces.end(); I++)
	{
		Face* F = *I;
		if (F->pDeflector)	continue;
		if (!hasImplicitLighting(F)) continue;
		
		Progress (float(I-g_faces.begin())/float(g_faces.size()));
		b_material&		M	= materials		[F->dwMaterial];
		DWORD			Tid = M.surfidx;
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
		ImplicitHash.initialize		(bounds);
		for (DWORD fid=0; fid<defl.faces.size(); fid++)
		{
			Face* F				= defl.faces[fid];
			defl.Bounds			(fid,bounds);
			ImplicitHash.add	(bounds,F);
		}

		// Start threads
		CThreadManager			tmanager;
		DWORD	stride			= defl.Height()/NUM_THREADS;
		for (DWORD thID=0; thID<NUM_THREADS; thID++)
			tmanager.start		(new ImplicitThread(thID,&defl,thID*stride,thID*stride+stride));
		tmanager.wait			();

		// Expand
		Status	("Processing lightmap...");
		for (DWORD ref=254; ref>0; ref--)	if (!defl.ApplyBorders(ref)) break;

		Status	("Mixing lighting with texture...");
		DWORD*	markup = LPDWORD(xr_malloc(defl.Height()*defl.Width()*4));
		{
			for (DWORD V=0; V<defl.Height(); V++)
			{
				for (DWORD U=0; U<defl.Width(); U++)
				{
					// Retreive Texel
					DWORD &OLD	= defl.Texel(U,V);
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
			Msg		("Saving texture '%s'...",out_name);
			VerifyPath				(out_name);
			BYTE*	raw_data		= LPBYTE(TEX.pSurface);
			DWORD	w				= TEX.dwWidth;
			DWORD	h				= TEX.dwHeight;
			DWORD	pitch			= w*4;
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
			Msg		("Saving markup '%s'...",out_name);

			TGAdesc			p;
			p.format		= IMG_24B;
			p.scanlenght	= TEX.dwWidth*4;
			p.width			= TEX.dwWidth;
			p.height		= TEX.dwHeight;
			p.data			= markup;
			p.maketga		(out_name);
		}
		_FREE(markup)
		*/
	}
}
