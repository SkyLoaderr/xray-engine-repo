#include "stdafx.h"
#include "xrlevel.h"
#include "shader_xrlc.h"
#include "communicate.h"
#include "xrThread.h"
#include "detailformat.h"
#include "xrhemisphere.h"
#include "cl_intersect.h"
#include "ftimer.h"
#include "Etextureparams.h"

#define NUM_THREADS		3

float	color_intensity	(Fcolor& c)
{
	float	ntsc		= c.r * 0.2125f + c.g * 0.7154f + c.b * 0.0721f;
	float	absolute	= c.magnitude_rgb() / 1.7320508075688772935274463415059f;
	return	ntsc*0.2f + absolute*0.8f;
}

//-----------------------------------------------------------------------------------------------------------------
const int	LIGHT_Count				=	7;

//-----------------------------------------------------------------
__declspec(thread)		u64			t_start	= 0;
__declspec(thread)		u64			t_time	= 0;
__declspec(thread)		u64			t_count	= 0;

//-----------------------------------------------------------------
#define LT_DIRECT		0
#define LT_POINT		1
#define LT_SECONDARY	2

struct R_Light
{
    DWORD           type;				// Type of light source		
    Fcolor          diffuse;			// Diffuse color of light	
    Fvector         position;			// Position in world space	
    Fvector         direction;			// Direction in world space	
    float		    range;				// Cutoff range
	float			range2;				// ^2
    float	        attenuation0;		// Constant attenuation		
    float	        attenuation1;		// Linear attenuation		
    float	        attenuation2;		// Quadratic attenuation	
	float			energy;				// For radiosity ONLY
	
	Fvector			tri[3];

	R_Light()		{
		tri[0].set	(0,0,0);
		tri[1].set	(0,0,EPS_S);
		tri[2].set	(EPS_S,0,0);
	}
};
typedef	svector<R_Light*,4*1024>	LSelection;

DEF_VECTOR		(Lights,R_Light);
//-----------------------------------------------------------------

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

//-----------------------------------------------------------------
Lights					g_lights;
CDB::MODEL				RCAST_Model;
Fbox					LevelBB;
CVirtualFileStreamRW*	dtFS=0;
DetailHeader			dtH;
DetailSlot*				dtS;

Shader_xrLC_LIB			g_shaders_xrlc;

b_params				g_params;

vector<b_material>		g_materials;
vector<b_shader>		g_shader_render;
vector<b_shader>		g_shader_compile;
vector<b_BuildTexture>	g_textures;
vector<b_rc_face>		g_rc_faces;

//-----------------------------------------------------------------
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

// 
void xrLoad(LPCSTR name)
{
	g_shaders_xrlc.Load			("gamedata\\shaders_xrlc.xr");
	// Load CFORM
	string256			N;
	{
		strconcat			(N,name,"build.cform");
		CVirtualFileStream	FS(N);
		
		R_ASSERT(FS.FindChunk(0));
		hdrCFORM			H;
		FS.Read				(&H,sizeof(hdrCFORM));
		R_ASSERT			(CFORM_CURRENT_VERSION==H.version);
		
		Fvector*	verts	= (Fvector*)FS.Pointer();
		CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
		RCAST_Model.build	( verts, H.vertcount, tris, H.facecount );
		Msg("* Level CFORM: %dK",RCAST_Model.memory()/1024);

		g_rc_faces.resize	(H.facecount);
		R_ASSERT(FS.FindChunk(1));
		FS.Read				(g_rc_faces.begin(),g_rc_faces.size()*sizeof(b_rc_face));

		LevelBB.set			(H.aabb);
	}
	
	// Load .details
	{
		strconcat			(N,name,"level.details");
		dtFS				= xr_new<CVirtualFileStreamRW> (N);
		dtFS->ReadChunk		(0,&dtH);
		R_ASSERT			(dtH.version==DETAIL_VERSION);

		dtFS->FindChunk		(2);
		dtS					= (DetailSlot*)dtFS->Pointer();
	}
	
	// Load lights
	{
		strconcat			(N,name,"build.prj");

		string32	ID			= BUILD_PROJECT_MARK;
		string32	id;
		CStream*	F			= xr_new<CFileStream> (N);
		F->Read		(&id,8);
		if (0==strcmp(id,ID))	{
			xr_delete			(F);
			F					= xr_new<CCompressedStream> (N,ID);
		}
		CStream&				FS	= *F;

		// Version
		DWORD version;
		FS.ReadChunk			(EB_Version,&version);
		R_ASSERT(XRCL_CURRENT_VERSION==version);

		// Header
		FS.ReadChunk			(EB_Parameters,&g_params);


		// Load lights
		Status	("Loading lights...");
		{
			// Static
			{
				F = FS.OpenChunk(EB_Light_static);
				b_light_static	temp;
				DWORD cnt		= F->Length()/sizeof(temp);
				for				(DWORD i=0; i<cnt; i++)
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
					RL.energy				=	color_intensity(L.diffuse);

					g_lights.push_back		(RL);
					// place into layer
//					R_ASSERT	(temp.controller_ID<L_layers.size());
//					L_layers	[temp.controller_ID].lights.push_back	(RL);
				}
				F->Close		();
			}
		}
		transfer("materials",	g_materials,			FS,		EB_Materials);
		transfer("shaders_xrlc",g_shader_compile,		FS,		EB_Shaders_Compile);
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
				BT.pSurface	= 0;
				if (!bLOD) 
				{
					if (BT.bHasAlpha || BT.THM.flags.test(STextureParams::flImplicitLighted))
					{
						clMsg		("- loading: %s",N);
						u32			w=0, h=0;
						BT.pSurface = Surface_Load(N,w,h); 
						R_ASSERT2	(BT.pSurface,"Can't load surface");
						BT.Vflip	();
					} else {
						// Free surface memory
					}
				}

				// save all the stuff we've created
				g_textures.push_back	(BT);
			}
		}
	}
}

IC bool RayPick(CDB::COLLIDER& DB, Fvector& P, Fvector& D, float r, R_Light& L)
{
	// 1. Check cached polygon
	float _u,_v,range;
	bool res = CDB::TestRayTri(P,D,L.tri,_u,_v,range,true);
	if (res) {
		if (range>0 && range<r) return true;
	}

	// 2. Polygon doesn't pick - real database query
	t_start			= CPU::GetCycleCount();
	DB.ray_query	(&RCAST_Model,P,D,r);
	t_time			+=	CPU::GetCycleCount()-t_start-CPU::cycles_overhead;
	t_count			+=	1;
	
	// 3. Analyze
	if (0==DB.r_count()) {
		return false;
	} else {
		// cache polygon
		CDB::RESULT&	rpinf	= *DB.r_begin();
		CDB::TRI&		T		= RCAST_Model.get_tris()[rpinf.id];
		L.tri[0].set	(*T.verts[0]);
		L.tri[1].set	(*T.verts[1]);
		L.tri[2].set	(*T.verts[2]);
		return true;
	}
}

float getLastRP_Scale(CDB::COLLIDER* DB, R_Light& L)//, Face* skip)
{
	u32	tris_count		= DB->r_count();
	float	scale		= 1.f;
	Fvector B;

//	X_TRY 
	{
		for (u32 I=0; I<tris_count; I++)
		{
			CDB::RESULT& rpinf = DB->r_begin()[I];
			// Access to texture
			CDB::TRI& clT								= RCAST_Model.get_tris()[rpinf.id];
			b_rc_face& F								= g_rc_faces[clT.dummy];
//			if (0==F)									continue;
//			if (skip==F)								continue;

			b_material& M	= g_materials				[F.dwMaterial];
			b_texture&	T	= g_textures				[M.surfidx];
			Shader_xrLC& SH	= *g_shaders_xrlc.Get		(M.shader_xrlc);
			if (!SH.flags.bLIGHT_CastShadow)			continue;

			if (!T.bHasAlpha)		
			{
				// Opaque poly - cache it
				L.tri[0].set	(*clT.verts[0]);
				L.tri[1].set	(*clT.verts[1]);
				L.tri[2].set	(*clT.verts[2]);
				return 0;
			}
			R_ASSERT2(T.pSurface,"Empty surface.");

			// barycentric coords
			// note: W,U,V order
			B.set	(1.0f - rpinf.u - rpinf.v,rpinf.u,rpinf.v);

			// calc UV
			Fvector2*	cuv = F.t;
			Fvector2	uv;
			uv.x = cuv[0].x*B.x + cuv[1].x*B.y + cuv[2].x*B.z;
			uv.y = cuv[0].y*B.x + cuv[1].y*B.y + cuv[2].y*B.z;

			int U = iFloor(uv.x*float(T.dwWidth) + .5f);
			int V = iFloor(uv.y*float(T.dwHeight)+ .5f);
			U %= T.dwWidth;		if (U<0) U+=T.dwWidth;
			V %= T.dwHeight;	if (V<0) V+=T.dwHeight;

			u32 pixel		= T.pSurface[V*T.dwWidth+U];
			u32 pixel_a		= color_get_A(pixel);
			float opac		= 1.f - float(pixel_a)/255.f;
			scale			*= opac;
		}
	} 
//	X_CATCH
//	{
//		clMsg("* ERROR: getLastRP_Scale");
//	}

	return scale;
}

float rayTrace	(CDB::COLLIDER* DB, R_Light& L, Fvector& P, Fvector& D, float R)//, Face* skip)
{
	R_ASSERT	(DB);

	// 1. Check cached polygon
	float _u,_v,range;
	bool res = CDB::TestRayTri(P,D,L.tri,_u,_v,range,false);
	if (res) {
		if (range>0 && range<R) return 0;
	}

	// 2. Polygon doesn't pick - real database query
	DB->ray_query	(&RCAST_Model,P,D,R);

	// 3. Analyze polygons and cache nearest if possible
	if (0==DB->r_count()) {
		return 1;
	} else {
		return getLastRP_Scale(DB,L);//,skip);
	}
	return 0;
}

float LightPoint(CDB::COLLIDER* DB, Fvector &Pold, Fvector &N, LSelection& SEL)
{
	Fvector		Ldir,Pnew;
	Pnew.mad	(Pold,N,0.1f);

	R_Light	**IT = SEL.begin(), **E = SEL.end();

	float	amount = 0;
	for (; IT!=E; IT++)
	{
		R_Light* L = *IT;
		if (L->type==LT_DIRECT) 
		{
			// Cos
			Ldir.invert	(L->direction);
			float D		= Ldir.dotproduct( N );
			if( D <=0 ) continue;

			// Raypick
//O			if (!RayPick(DB,Pnew,Ldir,1000.f,*L))	amount+=D*L->energy;
			// Trace Light
			amount += D*L->energy*rayTrace(DB,*L,Pnew,Ldir,1000.f);//,skip);
		} else {
			// Distance
			float sqD	= Pnew.distance_to_sqr(L->position);
			if (sqD > L->range2) continue;
			
			// Dir
			Ldir.sub	(L->position,Pnew);
			Ldir.normalize_safe();
			float D		= Ldir.dotproduct( N );
			if( D <=0 ) continue;
			
			// Raypick
//O			float R		= _sqrt(sqD);
//O			if (!RayPick(DB,Pnew,Ldir,R,*L))
//O				amount += (D*L->energy)/(L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);
			// Trace Light
			float R		= _sqrt(sqD);
			float scale = D*L->energy*rayTrace(DB,*L,Pnew,Ldir,R);//,skip);
			amount		+= scale / (L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);
		}
	}
	return amount;
}

DEFINE_VECTOR(DWORD,DWORDVec,DWORDIt);
class	LightThread : public CThread
{
	DWORD		Nstart, Nend;
	DWORDVec	box_result;
public:
	LightThread			(DWORD ID, DWORD _start, DWORD _end) : CThread(ID)
	{
		Nstart	= _start;
		Nend	= _end;
	}
	IC float			fromSlotX		(int x)		
	{
		return (x-dtH.offs_x)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;
	}
	IC float			fromSlotZ		(int z)		
	{
		return (z-dtH.offs_z)*DETAIL_SLOT_SIZE+DETAIL_SLOT_SIZE_2;
	}
	void				GetSlotRect		(Frect& rect, int sx, int sz)
	{
		float x 		= fromSlotX(sx);
		float z 		= fromSlotZ(sz);
		rect.x1			= x-DETAIL_SLOT_SIZE_2+EPS_L;
		rect.y1			= z-DETAIL_SLOT_SIZE_2+EPS_L;
		rect.x2			= x+DETAIL_SLOT_SIZE_2-EPS_L;
		rect.y2			= z+DETAIL_SLOT_SIZE_2-EPS_L;
	}
	virtual void		Execute()
	{
		CDB::COLLIDER	DB;
		DB.ray_options	(CDB::OPT_ONLYNEAREST | CDB::OPT_CULL		);
		DB.box_options	(0);//CDB::OPT_FULL_TEST							);

		vector<R_Light>	Lights = g_lights;

		LSelection		Selected;
		for (DWORD _z=Nstart; _z<Nend; _z++)
		{
			for (DWORD _x=0; _x<dtH.size_x; _x++)
			{
				DetailSlot&	DS = dtS[_z*dtH.size_x+_x];

				if ((DS.items[0].id==0xff)&&(DS.items[1].id==0xff)&&(DS.items[2].id==0xff)&&(DS.items[3].id==0xff))
					continue;

				// Build slot BB & sphere
				int slt_z = int(_z)-int(dtH.offs_z);
				int slt_x = int(_x)-int(dtH.offs_x);
				
				Fbox		BB;
				BB.min.set	(slt_x*DETAIL_SLOT_SIZE,	DS.y_min,	slt_z*DETAIL_SLOT_SIZE);
				BB.max.set	(BB.min.x+DETAIL_SLOT_SIZE,	DS.y_max,	BB.min.z+DETAIL_SLOT_SIZE);
				BB.grow		(0.05f);

				Fsphere		S;
				BB.getsphere(S.P,S.R);
				
				// Select polygons
				Fvector				bbC,bbD;
				BB.get_CD			(bbC,bbD);	bbD.add(0.01f);
				DB.box_query		(&RCAST_Model,bbC,bbD);

				box_result.clear	();
				for (CDB::RESULT* I=DB.r_begin(); I!=DB.r_end(); I++) box_result.push_back(I->id);
				if (box_result.empty())	continue;

				CDB::TRI* tris		= RCAST_Model.get_tris();
				
				// select lights
				Selected.clear();
				for (DWORD L=0; L<Lights.size(); L++)
				{
					R_Light&	R = g_lights[L];
					if (R.type==LT_DIRECT)	Selected.push_back(&R);
					else {
						float dist = S.P.distance_to(R.position);
						if ((dist - S.R) < R.range)	Selected.push_back(&R);
					}
				}
				
				// lighting itself
				float amount[4]	= {0,0,0,0};
				DWORD count[4]	= {0,0,0,0};
				float coeff		= DETAIL_SLOT_SIZE_2/float(LIGHT_Count);
				FPU::m64r		();
				for (int x=-LIGHT_Count; x<=LIGHT_Count; x++) 
				{
					Fvector		P;
					P.x			= bbC.x + coeff*float(x);

					for (int z=-LIGHT_Count; z<=LIGHT_Count; z++) 
					{
						// compute position
						Fvector t_n;  t_n.set(0,1,0);
						P.z			= bbC.z + coeff*float(z);
						P.y			= BB.min.y-5;
						Fvector	dir;	dir.set		(0,-1,0);
						Fvector start;	start.set	(P.x,BB.max.y+EPS,P.z);
						
						float		r_u,r_v,r_range;
						for (DWORDIt tit=box_result.begin(); tit!=box_result.end(); tit++)
						{
							CDB::TRI&	T	= tris	[*tit];
							if (CDB::TestRayTri(start,dir,T.verts,r_u,r_v,r_range,TRUE))
							{
								if (r_range>=0.f)	{
									float y_test	= start.y - r_range;
									if (y_test>P.y)	{
										P.y			= y_test+EPS;
										t_n.mknormal(*T.verts[0],*T.verts[1],*T.verts[2]);
									}
								}
							}
						}
						if (P.y<BB.min.y) continue;
						
						// select part of slot
						int pid = 0;
						if (z>0)
						{
							if (x<0)	pid = 0;
							else		pid = 1;
						} else {
							if (x<0)	pid = 2;
							else		pid = 3;
						}

						// light point
						amount	[pid]	+= LightPoint(&DB,P,t_n,Selected);
						count	[pid]	+= 1;
					}
				}
				
				// 
				// if ((0==count[0]) || (0==count[1]) || (0==count[2]) || (0==count[3]))
					// Msg("* failed to calculate slot X%d:Z%d",_x,_z);
//				Msg("%dx%d [0:%f, 1:%f, 2:%f, 3:%f]",_x,_z,amount[0],amount[1],amount[2],amount[3]);

				// calculation of luminocity
				DetailPalette* dc = (DetailPalette*)&DS.color;	int LL; float	res;
				float amb		= color_intensity	(g_params.m_lm_amb_color);
				float f			= g_params.m_lm_amb_fogness;
				float f_inv		= 1.f - f; 
				res				= (amount[0]/float(count[0]))*f_inv + amb*f; LL = iFloor(9.f * res); clamp(LL,0,15); dc->a0	= LL;
				res				= (amount[1]/float(count[1]))*f_inv + amb*f; LL = iFloor(9.f * res); clamp(LL,0,15); dc->a1	= LL;
				res				= (amount[2]/float(count[2]))*f_inv + amb*f; LL = iFloor(9.f * res); clamp(LL,0,15); dc->a2	= LL;
				res				= (amount[3]/float(count[3]))*f_inv + amb*f; LL = iFloor(9.f * res); clamp(LL,0,15); dc->a3	= LL;
				thProgress		= float(_z-Nstart)/float(Nend-Nstart);
				thPerformance	= float(double(t_count)/double(t_time*CPU::cycles2seconds))/1000.f;
			}
		}
	}
};

void	xrLight			()
{
	DWORD	range			= dtH.size_z;

	// Start threads, wait, continue --- perform all the work
	CThreadManager			Threads;
	DWORD	start_time		= timeGetTime();
	DWORD	stride			= range/NUM_THREADS;
	DWORD	last			= range-stride*(NUM_THREADS-1);
	for (DWORD thID=0; thID<NUM_THREADS; thID++)
	{
		CThread*	T		= xr_new<LightThread> (thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride));
		T->thMessages		= FALSE;
		T->thMonitor		= FALSE;
		Threads.start		(T);
	}
	Threads.wait			();
	Msg("%d seconds elapsed.",(timeGetTime()-start_time)/1000);
}

void xrCompiler(LPCSTR name)
{
	Phase("Loading level...");
	xrLoad		(name);

	Phase("Lighting nodes...");
	xrLight		();

	if (dtFS)	xr_delete(dtFS);
}
						/*
						Frect rect;
						GetSlotRect	(rect,_x,_z);
						BB.min.set	(rect.x1, DS.y_min, rect.y1);
						BB.max.set	(rect.x2, DS.y_max, rect.y2);
						BB.grow		(0.01f);
						Msg			("BB1:[%3.2f,%3.2f,%3.2f]-[%3.2f,%3.2f,%3.2f]",
						BB.min.x,BB.min.y,BB.min.z,
						BB.max.x,BB.max.y,BB.max.z
						);
						*/
