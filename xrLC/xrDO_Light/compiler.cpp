#include "stdafx.h"
#include "xr_func.h"
#include "xrlevel.h"
#include "communicate.h"
#include "cl_rapid.h"
#include "cl_intersect.h"
#include "xrThread.h"
#include "detailformat.h"


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

DEF_VECTOR		(Lights,R_Light);
//-----------------------------------------------------------------

Lights					g_lights;
RAPID::Model			Level;
Fbox					LevelBB;
CVirtualFileStreamRW*	dtFS=0;
DetailHeader			dtH;

//-----------------------------------------------------------------
#define HEMI1_LIGHTS	26
#define HEMI2_LIGHTS	91

const double hemi_1[HEMI1_LIGHTS][3] = 
{
	{0.00000,	1.00000,	0.00000	},
	{0.52573,	0.85065,	0.00000	},
	{0.16246,	0.85065,	0.50000	},
	{-0.42533,	0.85065,	0.30902	},
	{-0.42533,	0.85065,	-0.30902},
	{0.16246,	0.85065,	-0.50000},
	{0.89443,	0.44721,	0.00000	},
	{0.27639,	0.44721,	0.85065	},
	{-0.72361,	0.44721,	0.52573	},
	{-0.72361,	0.44721,	-0.52573},
	{0.27639,	0.44721,	-0.85065},
	{0.68819,	0.52573,	0.50000	},
	{-0.26287,	0.52573,	0.80902	},
	{-0.85065,	0.52573,	-0.00000},
	{-0.26287,	0.52573,	-0.80902},
	{0.68819,	0.52573,	-0.50000},
	{0.95106,	0.00000,	0.30902	},
	{0.58779,	0.00000,	0.80902	},
	{-0.00000,	0.00000,	1.00000	},
	{-0.58779,	0.00000,	0.80902	},
	{-0.95106,	0.00000,	0.30902	},
	{-0.95106,	0.00000,	-0.30902},
	{-0.58779,	0.00000,	-0.80902},
	{0.00000,	0.00000,	-1.00000},
	{0.58779,	0.00000,	-0.80902},
	{0.95106,	0.00000,	-0.30902}
};

void xrLoad(LPCSTR name)
{
	// Load CFORM
	FILE_NAME			N;
	{
		strconcat			(N,name,"level.");
		CVirtualFileStream	FS(N);
		
		CStream* fs			= FS.OpenChunk(fsL_CFORM);
		R_ASSERT			(fs);
		
		hdrCFORM			H;
		fs->Read			(&H,sizeof(hdrCFORM));
		R_ASSERT			(CFORM_CURRENT_VERSION==H.version);
		
		Fvector*	verts	= (Fvector*)fs->Pointer();
		RAPID::tri*	tris	= (RAPID::tri*)(verts+H.vertcount);
		Level.BuildModel	( verts, H.vertcount, tris, H.facecount );
		Msg("* Level CFORM: %dK",Level.MemoryUsage()/1024);
		fs->Close			();
		
		LevelBB.set			(H.aabb);
	}
	
	// Load .details
	{
		strconcat			(N,name,"level.details");
		dtFS				= new CVirtualFileStreamRW(N);
		dtFS->ReadChunk		(0,&dtH);
		R_ASSERT			(dtH.version==DETAIL_VERSION);
	}
	
	// Load lights
	{
		strconcat			(N,name,"build.prj");

		CVirtualFileStream	FS(N);
		void*				data = FS.Pointer();
		
		b_transfer	Header		= *((b_transfer *) data);
		R_ASSERT(XRCL_CURRENT_VERSION==Header._version);
		Header.lights			= (b_light*)	(DWORD(data)+DWORD(Header.lights));

		// Hemi setup
		int			h_count, h_table[3];
		const double (*hemi)[3] = 0;
		h_count		= HEMI1_LIGHTS;
		h_table[0]	= 0;
		h_table[1]	= 1;
		h_table[2]	= 2;
		hemi		= hemi_1;
		
		// Cycle thru
		for (DWORD l=0; l<Header.light_count; l++) 
		{
			b_light R = Header.lights[l];
			if (R.flags & XRLIGHT_LMAPS) 
			{
				R_Light	RL;
				if (R.type==D3DLIGHT_DIRECTIONAL) {
					RL.type				= LT_DIRECT;
				} else {
					RL.type				= LT_POINT;
				}
				RL.amount				=	R.diffuse.magnitude_rgb();
				RL.position.set				(R.position);
				RL.direction.normalize_safe	(R.direction);
				RL.range				=	R.range*1.2f;
				RL.range2				=	RL.range*RL.range;
				RL.attenuation0			=	R.attenuation0;
				RL.attenuation1			=	R.attenuation1;
				RL.attenuation2			=	R.attenuation2;
				RL.tri[0].set			(0,0,0);
				RL.tri[1].set			(0,0,0);
				RL.tri[2].set			(0,0,0);
				g_lights.push_back		(RL);

				if (RL.type==LT_DIRECT)	
				{
					R_Light	T			=	RL;
					T.amount			=	Header.params.areaDark.magnitude_rgb()*(Header.params.area_energy_summary)/float(h_count);
					for (int i=0; i<h_count; i++)
					{
						T.direction.set			(float(hemi[i][0]),float(hemi[i][1]),float(hemi[i][2]));
						T.direction.invert		();
						T.direction.normalize	();
						g_lights.push_back		(T);
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------------------------
const int	LIGHT_Count			=2;
const int	LIGHT_Total			=(2*LIGHT_Count+1)*(2*LIGHT_Count+1);

typedef	svector<R_Light*,128>	LSelection;

IC bool RayPick(RAPID::XRCollide& DB, Fvector& P, Fvector& D, float r, R_Light& L)
{
	// 1. Check cached polygon
	float _u,_v,range;
	bool res = RAPID::TestRayTri(P,D,L.tri,_u,_v,range,true);
	if (res) {
		if (range>0 && range<r) return true;
	}

	// 2. Polygon doesn't pick - real database query
	DB.RayPick(0,&Level,P,D,r);
	if (0==DB.GetRayContactCount()) {
		return false;
	} else {
		// cache polygon
		RAPID::raypick_info&	rpinf	= DB.RayContact[0];
		L.tri[0].set	(rpinf.p[0]);
		L.tri[1].set	(rpinf.p[1]);
		L.tri[2].set	(rpinf.p[2]);
		return true;
	}
}

float LightPoint(RAPID::XRCollide& DB, Fvector &P, Fvector &N, LSelection& SEL)
{
	Fvector		Ldir,Pnew;
	Pnew.direct(P,N,0.05f);

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
			float sqD	= P.distance_to_sqr(L->position);
			if (sqD > L->range2) continue;
			
			// Dir
			Ldir.sub	(L->position,P);
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
		RAPID::XRCollide DB;
		DB.RayMode		(RAY_ONLYFIRST|RAY_CULL);

		vector<R_Light>	Lights = g_lights;

		Fvector			P,D,PLP;
		D.set			(0,1,0);
		float coeff		= 0.5f*DETAIL_SLOT_SIZE/float(LIGHT_Count);
		
		LSelection		Selected;
		float			LperN	= float(g_lights.size());
		for (DWORD i=Nstart; i<Nend; i++)
		{
			Node& N = g_nodes[i];
			
			// select lights
			Selected.clear();
			for (DWORD L=0; L<Lights.size(); L++)
			{
				R_Light&	R = g_lights[L];
				if (R.type==LT_DIRECT)	Selected.push_back(&R);
				else {
					float dist = N.Pos.distance_to(R.position);
					if (dist-g_params.fPatchSize < R.range)
						Selected.push_back(&R);
				}
			}
			LperN = 0.9f*LperN + 0.1f*float(Selected.size());
			
			// lighting itself
			float amount=0;
			for (int x=-LIGHT_Count; x<=LIGHT_Count; x++) 
			{
				P.x = N.Pos.x + coeff*float(x);
				for (int z=-LIGHT_Count; z<=LIGHT_Count; z++) 
				{
					// compute position
					P.z = N.Pos.z + coeff*float(z);
					P.y = N.Pos.y;
					N.Plane.intersectRayPoint(P,D,PLP);	// "project" position
					P.y = PLP.y;
					
					// light point
					amount += LightPoint(DB,P,N.Plane.n,Selected);
				}
			}
			
			// calculation of luminocity
			N.LightLevel	= amount/float(LIGHT_Total);
			
			thProgress		= float(i-Nstart)/float(Nend-Nstart);
		}
	}
};

#define NUM_THREADS	8
void	xrLight			()
{
	// Start threads, wait, continue --- perform all the work
	DWORD	start_time		= timeGetTime();
	CThreadManager			Threads;
	DWORD	stride			= g_nodes.size()/NUM_THREADS;
	DWORD	last			= g_nodes.size()-stride*(NUM_THREADS-1);
	for (DWORD thID=0; thID<NUM_THREADS; thID++)
		Threads.start(new LightThread(thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride)));
	Threads.wait			();
	Msg("%d seconds elapsed.",(timeGetTime()-start_time)/1000);

	// Smooth
	Status("Smoothing lighting...");
	for (int pass=0; pass<3; pass++) {
		Nodes	Old = g_nodes;
		for (DWORD N=0; N<g_nodes.size(); N++)
		{
			Node&	Base		= Old[N];
			Node&	Dest		= g_nodes[N];
			
			float	val			= 2*Base.LightLevel;
			float	cnt			= 2;
			
			for (int nid=0; nid<4; nid++) {
				if (Base.n[nid]!=InvalidNode) {
					val		+=  Old[Base.n[nid]].LightLevel;
					cnt		+=	1.f;
				}
			}
			Dest.LightLevel		=  val/cnt;
			clamp(Dest.LightLevel,0.f,1.f);
		}
	}
}

void xrCompiler(LPCSTR name)
{
	Phase("Loading level...");
	xrLoad		(name);

	Phase("Lighting nodes...");
	xrLight		();
}
