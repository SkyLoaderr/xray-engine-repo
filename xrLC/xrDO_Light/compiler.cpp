#include "stdafx.h"
#include "xrlevel.h"
#include "communicate.h"
#include "xrThread.h"
#include "detailformat.h"
#include "xrhemisphere.h"
#include "cl_intersect.h"
#include "ftimer.h"

#define NUM_THREADS		3

//-----------------------------------------------------------------------------------------------------------------
const int	LIGHT_Count				=	6;

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

Lights					g_lights;
CDB::MODEL				Level;
Fbox					LevelBB;
CVirtualFileStreamRW*	dtFS=0;
DetailHeader			dtH;
DetailSlot*				dtS;
b_params				g_params;
//-----------------------------------------------------------------
// hemi
struct		hemi_data
{
	vector<R_Light>*	dest;
	R_Light				T;
};
void		__stdcall	hemi_callback(float x, float y, float z, float E, LPVOID P)
{
	hemi_data*	H		= (hemi_data*)P;
	H->T.energy			= E * g_params.area_color.magnitude_rgb();
	H->T.direction.set	(x,y,z);
	H->dest->push_back	(H->T);
}

// 
void xrLoad(LPCSTR name)
{
	// Load CFORM
	FILE_NAME			N;
	{
		strconcat			(N,name,"build.cform");
		CVirtualFileStream	FS(N);
		
		hdrCFORM			H;
		FS.Read				(&H,sizeof(hdrCFORM));
		R_ASSERT			(CFORM_CURRENT_VERSION==H.version);
		
		Fvector*	verts	= (Fvector*)FS.Pointer();
		CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
		Level.build			( verts, H.vertcount, tris, H.facecount );
		Msg("* Level CFORM: %dK",Level.memory()/1024);
		
		LevelBB.set			(H.aabb);
	}
	
	// Load .details
	{
		strconcat			(N,name,"level.details");
		dtFS				= new CVirtualFileStreamRW(N);
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
		CStream*	F			= new CFileStream(N);
		F->Read		(&id,8);
		if (0==strcmp(id,ID))	{
			_DELETE		(F);
			F					= new CCompressedStream(N,ID);
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
					RL.energy				=	L.diffuse.magnitude_rgb	();

					g_lights.push_back		(RL);
					// place into layer
//					R_ASSERT	(temp.controller_ID<L_layers.size());
//					L_layers	[temp.controller_ID].lights.push_back	(RL);
				}
				F->Close		();
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
	DB.ray_query	(&Level,P,D,r);
	t_time			+=	CPU::GetCycleCount()-t_start-CPU::cycles_overhead;
	t_count			+=	1;
	
	// 3. Analyze
	if (0==DB.r_count()) {
		return false;
	} else {
		// cache polygon
		CDB::RESULT&	rpinf	= *DB.r_begin();
		CDB::TRI&		T		= Level.get_tris()[rpinf.id];
		L.tri[0].set	(*T.verts[0]);
		L.tri[1].set	(*T.verts[1]);
		L.tri[2].set	(*T.verts[2]);
		return true;
	}
}

float LightPoint(CDB::COLLIDER& DB, Fvector &Pold, Fvector &N, LSelection& SEL)
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
			if (!RayPick(DB,Pnew,Ldir,1000.f,*L))	amount+=D*L->energy;
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
			float R		= sqrtf(sqD);
			if (!RayPick(DB,Pnew,Ldir,R,*L))
				amount += (D*L->energy)/(L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);
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
				BB.grow		(0.01f);

				Fsphere		S;
				BB.getsphere(S.P,S.R);
				
				// Select polygons
				Fvector				bbC,bbD;
				BB.get_CD			(bbC,bbD);
				DB.box_query		(&Level,bbC,bbD);

				box_result.clear	();
				for (CDB::RESULT* I=DB.r_begin(); I!=DB.r_end(); I++) box_result.push_back(I->id);
				if (box_result.empty())	continue;

				CDB::TRI* tris		= Level.get_tris();
				
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
						amount	[pid]	+= LightPoint(DB,P,t_n,Selected);
						count	[pid]	+= 1;
					}
				}
				
				// 
				if ((0==count[0]) || (0==count[1]) || (0==count[2]) || (0==count[3]))
					Msg("* failed to calculate slot X%d:Z%d",_x,_z);
//				Msg("%dx%d [0:%f, 1:%f, 2:%f, 3:%f]",_x,_z,amount[0],amount[1],amount[2],amount[3]);

				// calculation of luminocity
				DetailPalette* dc = (DetailPalette*)&DS.color;	int LL; float	res;
				float amb		= g_params.m_lm_amb_color.magnitude_rgb();
				float f			= g_params.m_lm_amb_fogness;
				float f_inv		= 1.f - f; 
				res				= (amount[0]/float(count[0]))*f_inv + amb*f; LL = iFloor(8.f * res); clamp(LL,0,15); dc->a0	= LL;
				res				= (amount[1]/float(count[1]))*f_inv + amb*f; LL = iFloor(8.f * res); clamp(LL,0,15); dc->a1	= LL;
				res				= (amount[2]/float(count[2]))*f_inv + amb*f; LL = iFloor(8.f * res); clamp(LL,0,15); dc->a2	= LL;
				res				= (amount[3]/float(count[3]))*f_inv + amb*f; LL = iFloor(8.f * res); clamp(LL,0,15); dc->a3	= LL;
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
		CThread*	T		= new LightThread(thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride));
		T->thMessages		= FALSE;
		T->thMonitor		= TRUE;
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

	if (dtFS)	_DELETE(dtFS);
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
