#include "stdafx.h"
#include "xrlevel.h"
#include "communicate.h"
#include "xrThread.h"
#include "detailformat.h"
#include "xrhemisphere.h"
#include "cl_intersect.h"
#include "ftimer.h"

#define NUM_THREADS		3
#define NUM_SUBDIVS		128

//-----------------------------------------------------------------------------------------------------------------
const int	LIGHT_Count				=	6;
const int	LIGHT_Total				=	(2*LIGHT_Count+1)*(2*LIGHT_Count+1);

//-----------------------------------------------------------------
#define LT_DIRECT		0
#define LT_POINT		1
#define LT_SECONDARY	2

struct R_Light
{
    DWORD           type;				// Type of light source		
    float			amount;				// Diffuse color of light	
    Fvector         position;			// Position in world space	
    Fvector         direction;			// Direction in world space	
    float		    range;				// Cutoff range
	float			range2;				// ^2
    float	        attenuation0;		// Constant attenuation		
    float	        attenuation1;		// Linear attenuation		
    float	        attenuation2;		// Quadratic attenuation	
	
	Fvector			tri[3];				// Cached triangle for ray-testing
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
b_transfer				Header;

__declspec(thread)		u64			t_start	= 0;
__declspec(thread)		u64			t_time	= 0;
__declspec(thread)		u64			t_count	= 0;

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
	H->T.amount			= E * Header.params.area_color.magnitude_rgb();
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

		CVirtualFileStream	FS(N);
		void*				data = FS.Pointer();
		
		Header				= *((b_transfer *) data);
		R_ASSERT(XRCL_CURRENT_VERSION==Header._version);
		Header.lights			= (b_light*)	(DWORD(data)+DWORD(Header.lights));

		vector<R_Light>*	dest = &g_lights;
		for (DWORD l=0; l<Header.light_count; l++) 
		{
			b_light* L = &Header.lights[l];
			if (L->flags.bAffectStatic) 
			{
				// generic properties
				R_Light						RL;
				RL.amount =					L->diffuse.magnitude_rgb();
				RL.position.set				(L->position);
				RL.direction.normalize_safe	(L->direction);
				RL.range				=	L->range*1.1f;
				RL.range2				=	RL.range*RL.range;
				RL.attenuation0			=	L->attenuation0;
				RL.attenuation1			=	L->attenuation1;
				RL.attenuation2			=	L->attenuation2;
				RL.tri[0].set			(0,0,0);
				RL.tri[1].set			(0,0,0);
				RL.tri[2].set			(0,0,0);
				
				if (L->type==D3DLIGHT_DIRECTIONAL) 
				{
					RL.type				= LT_DIRECT;
					R_Light	T			= RL;
					Fmatrix				rot_y;
					
					Fvector				v_top,v_right,v_dir;
					v_top.set			(0,1,0);
					v_dir.set			(RL.direction);
					v_right.crossproduct(v_top,v_dir);
					v_right.normalize	();
					
					// Build jittered light
					T.amount			= L->diffuse.magnitude_rgb()/14.f;
					float angle			= deg2rad(Header.params.area_dispersion);
					{
						//*** center
						dest->push_back	(T);
						
						//*** left
						rot_y.rotateY			(3*angle/4);
						rot_y.transform_dir		(T.direction,RL.direction);
						dest->push_back	(T);
						
						rot_y.rotateY			(2*angle/4);
						rot_y.transform_dir		(T.direction,RL.direction);
						dest->push_back	(T);
						
						rot_y.rotateY			(1*angle/4);
						rot_y.transform_dir		(T.direction,RL.direction);
						dest->push_back	(T);
						
						//*** right
						rot_y.rotateY			(-1*angle/4);
						rot_y.transform_dir		(T.direction,RL.direction);
						dest->push_back	(T);
						
						rot_y.rotateY			(-2*angle/4);
						rot_y.transform_dir		(T.direction,RL.direction);
						dest->push_back	(T);
						
						rot_y.rotateY			(-3*angle/4);
						rot_y.transform_dir		(T.direction,RL.direction);
						dest->push_back	(T);
						
						//*** top 
						rot_y.rotation			(v_right, 3*angle/4);
						rot_y.transform_dir		(T.direction,RL.direction);
						dest->push_back	(T);
						
						rot_y.rotation			(v_right, 2*angle/4);
						rot_y.transform_dir		(T.direction,RL.direction);
						dest->push_back	(T);
						
						rot_y.rotation			(v_right, 1*angle/4);
						rot_y.transform_dir		(T.direction,RL.direction);
						dest->push_back	(T);
						
						//*** bottom
						rot_y.rotation			(v_right,-1*angle/4);
						rot_y.transform_dir		(T.direction,RL.direction);
						dest->push_back	(T);
						
						rot_y.rotation			(v_right,-2*angle/4);
						rot_y.transform_dir		(T.direction,RL.direction);
						dest->push_back	(T);
						
						rot_y.rotation			(v_right,-3*angle/4);
						rot_y.transform_dir		(T.direction,RL.direction);
						dest->push_back	(T);
					}
					
					// Build area-lights
					if (Header.params.area_quality)	
					{
						hemi_data				h_data;
						h_data.dest				= dest;
						h_data.T				= RL;
						h_data.T.amount			= 0;
						xrHemisphereBuild		(Header.params.area_quality,FALSE,0.5f,Header.params.area_energy_summary,hemi_callback,&h_data);
					}
				} else {
					RL.type			= LT_POINT;
					dest->push_back (RL);
				}
			}
		}
	}
}

IC bool RayPick(CDB::COLLIDER& DB, Fvector& P, Fvector& D, float r, R_Light& L)
{
	/*
	// 1. Check cached polygon
	float _u,_v,range;
	bool res = CDB::TestRayTri(P,D,L.tri,_u,_v,range,true);
	if (res) {
		if (range>0 && range<r) return true;
	}
	*/

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
			if (!RayPick(DB,Pnew,Ldir,1000.f,*L))	amount+=D*L->amount;
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
				amount += (D*L->amount)/(L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);
		}
	}
	return amount;
}

class	LightThread : public CThread
{
	DWORD	Nstart, Nend;
public:
	LightThread			(DWORD ID, DWORD _start, DWORD _end) : CThread(ID)
	{
		Nstart	= _start;
		Nend	= _end;
	}
	virtual void		Execute()
	{
		CDB::COLLIDER	DB;
		DB.ray_options	(CDB::OPT_ONLYNEAREST | CDB::OPT_CULL);

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
				DB.box_options		(0);
				DB.box_query		(&Level,bbC,bbD);
				DWORD	triCount	= DB.r_count	();
				if (0==triCount)	continue;

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
				float coeff		= 0.5f*DETAIL_SLOT_SIZE/float(LIGHT_Count);
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
						for (DWORD tid=0; tid<triCount; tid++)
						{
							CDB::TRI&	T	= tris	[DB.r_begin()[tid].id];
							if (CDB::TestRayTri(start,dir,T.verts,r_u,r_v,r_range,TRUE))
							{
								if (r_range>=0.f)	{
									float y_test	= start.y - r_range;
									if (y_test>P.y)	{
										P.y = y_test;
										t_n.mknormal(*T.verts[0],*T.verts[1],*T.verts[2]);
									}
								}
							}
						}
						if (P.y<BB.min.y)	continue;
						
						// select part of slot
						int pid = 0;
						if (z<0)
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
//				if ((0==count[0]) || (0==count[1]) || (0==count[2]) || (0==count[3]))
//					Msg("* failed to calculate slot X%d:Z%d",_x,_z);

				// calculation of luminocity
				DetailPalette* dc = (DetailPalette*)&DS.color;	int LL; float	res;
				float amb		= Header.params.m_lm_amb_color.magnitude_rgb();
				float f			= Header.params.m_lm_amb_fogness;
				float f_inv		= 1.f - f; 
				res				= (amount[0]/float(count[0]))*f_inv + amb*f; LL = iFloor(15.f * res); clamp(LL,0,15); dc->a0	= LL;
				res				= (amount[1]/float(count[1]))*f_inv + amb*f; LL = iFloor(15.f * res); clamp(LL,0,15); dc->a1	= LL;
				res				= (amount[2]/float(count[2]))*f_inv + amb*f; LL = iFloor(15.f * res); clamp(LL,0,15); dc->a2	= LL;
				res				= (amount[3]/float(count[3]))*f_inv + amb*f; LL = iFloor(15.f * res); clamp(LL,0,15); dc->a3	= LL;
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
