#include "stdafx.h"
#include "build.h"
#include "xr_func.h"
#include "tga.h"
#include "xrThread.h"

BOOL	hasImplicitLighting(Face* F)
{
	if (0==F)									return FALSE;
	
	SH_ShaderDef&	SH							= F->Shader();
	if (0==SH.Passes_Count)						return FALSE;
	
	SH_PassDef&		Pass						= SH.Passes		[0];
	if (0==Pass.Stages_Count)					return FALSE;
	
	SH_StageDef&	Stage						= Pass.Stages	[0];
	if (Stage.tcs!=SH_StageDef::tcsGeometry)	return FALSE;
	if (Stage.tcm!=0)							return FALSE;
	
	b_material& M = pBuild->materials			[F->dwMaterial];
	if (0==M.dwTexCount)						return FALSE;
	
	b_BuildTexture&	T  = pBuild->textures		[M.surfidx[0]];
	return (T.THM.flag & EF_IMPLICIT_LIGHTED);
}

template <class T, DWORD s_X, DWORD s_Y>
class hash2D {
	vector<T>	table[s_Y][s_X];
	Fbox2		bounds;
	Fvector2	size;
public:
	void		initialize	(Fbox2& R)
	{
		bounds.set	(R);
		size.set	(R.max.x-R.min.x,R.max.y-R.min.y);
		for (DWORD y=0; y<s_Y; y++)
			for (DWORD x=0; x<s_Y; x++)
				table[y][x].clear();
	};
	void		add			(Fbox2& R, T& value)
	{
		int x1 = iFloor(float(s_X)*(R.min.x-bounds.min.x)/size.x); clamp(x1,0,int(s_X-1));
		int x2 = iCeil (float(s_X)*(R.max.x-bounds.min.x)/size.x); clamp(x2,0,int(s_X-1));
		int y1 = iFloor(float(s_Y)*(R.min.y-bounds.min.y)/size.y); clamp(y1,0,int(s_Y-1));
		int y2 = iCeil (float(s_Y)*(R.max.y-bounds.min.y)/size.y); clamp(y2,0,int(s_Y-1));
		
		for (int y=y1; y<=y2; y++)
			for (int x=x1; x<=x2; x++)
				table[y][x].push_back(value);
	};
	vector<T>&	query		(float x, float y)
	{
		int _x	= iFloor(float(s_X)*(x-bounds.min.x)/size.x); clamp(_x,0,int(s_X-1));
		int _y	= iFloor(float(s_Y)*(y-bounds.min.y)/size.y); clamp(_y,0,int(s_Y-1));
		return table[_y][_x];
	};
};

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
		DWORD size	=Width()*Height();
		lmap		= new ImplicitLumel[size];
		temp		= new ImplicitLumel[size];
		size		*= sizeof(ImplicitLumel);
		ZeroMemory	(lmap,size);
		ZeroMemory	(temp,size);
	}
	void			Deallocate()
	{
		_DELETE		(temp);
		_DELETE		(lmap);
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


#pragma comment(lib,"x:\\dxt.lib")
extern "C" __declspec(dllimport) bool __cdecl DXTCompress(LPCSTR out_name, BYTE* raw_data, DWORD w, DWORD h, DWORD pitch, STextureParams* fmt, DWORD depth);
extern void LightPoint(RAPID::XRCollide* DB, Fcolor &C, Fvector &P, Fvector &N, R_Light* begin, R_Light* end);

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
		Start			();
	}
	virtual void		Execute	()
	{
		R_ASSERT				(DATA);
		ImplicitDeflector& defl = *DATA;
		vector<R_Light>	Lights	= pBuild->lights_soften;
		RAPID::XRCollide		DB;
		
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
		DB.RayMode	(0);
		Fcolor		C[9];
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
						vecFace&	space	= ImplicitHash.query(P.u,P.v);
						
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
								LightPoint	(&DB, C[J], wP, wN, Lights.begin(), Lights.end());
								Fcount		++;
							}
						}
					} 
				} catch (...)
				{
					Msg("* THREAD #%d: Access violation. Possibly recovered.",thID);
				}
				
				FPU::m24r	();
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
					L.color.x	= R.r;
					L.color.y	= R.g;
					L.color.z	= R.b;
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
		DWORD			Tid = M.surfidx[0];
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
		Status("Lighting implicit map '%s'...",defl.texture->name);
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
		ImplicitThread*	THP	[NUM_THREADS];
		DWORD	stride		= defl.Height()/NUM_THREADS;
		for (DWORD thID=0; thID<NUM_THREADS; thID++)
			THP[thID]	= new ImplicitThread(thID,&defl,thID*stride,thID*stride+stride);

		// Wait for completition
		for (;;)
		{
			Sleep(500);

			float	sumProgress=0;
			DWORD	sumComplete=0;
			for (DWORD ID=0; ID<NUM_THREADS; ID++)
			{
				sumProgress += THP[ID]->thProgress;
				sumComplete	+= THP[ID]->thCompleted?1:0;
			}

			Progress(sumProgress/float(NUM_THREADS));
			if (sumComplete == NUM_THREADS)	break;
		}
		
		// Delete threads
		for (thID=0; thID<NUM_THREADS; thID++)
			_DELETE(THP[thID]);

		// Expand
		Status	("Processing lightmap...");
		for (DWORD ref=254; ref>0; ref--)	if (!defl.ApplyBorders(ref)) break;

		Status	("Mixing lighting with texture...");
		DWORD*	markup = LPDWORD(malloc(defl.Height()*defl.Width()*4));
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
			char	out_name[256];	strconcat(out_name,"x:\\game\\data\\levels\\",name,"\\",TEX.name,".dds");
			BYTE*	raw_data		= LPBYTE(TEX.pSurface);
			DWORD	w				= TEX.dwWidth;
			DWORD	h				= TEX.dwHeight;
			DWORD	pitch			= w*4;
			STextureParams* fmt		= &(TEX.THM);
			DXTCompress				(out_name,raw_data,w,h,pitch,fmt,4);
		}
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
	}
}
