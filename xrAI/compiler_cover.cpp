#include "stdafx.h"
#include "compiler.h"

#include "cl_defs.h"
#include "cl_intersect.h"
#include "cl_rapid.h"

#include "xrThread.h"

// -------------------------------- Ray pick
typedef Fvector	RayCache[3];

IC bool RayPick(RAPID::XRCollide* DB, Fvector& P, Fvector& D, float r, RayCache& C)
{
	// 1. Check cached polygon
	float _u,_v,range;
	if (RAPID::TestRayTri(P,D,&C[0],_u,_v,range,true)) 
	{
		if (range>0 && range<r) return true;
	}

	// 2. Polygon doesn't pick - real database query
	DB->RayPick(0,&LevelLight,P,D,r);
	if (0==DB->GetRayContactCount()) {
		return false;
	} else {
		// cache polygon
		RAPID::raypick_info&	rpinf	= DB->RayContact[0];
		C[0].set	(rpinf.p[0]);
		C[1].set	(rpinf.p[1]);
		C[2].set	(rpinf.p[2]);
		return true;
	}
}

IC int	calcSphereSector(Fvector& dir)
{
	Fvector2			flat;

	// flatten
	flat.set			(dir.x,dir.z);
	flat.norm			();

	// analyze
	if (_abs(flat.x)>_abs(flat.y))	{
		// sector 0,7,3,4
		if (flat.x<0)	{
			// sector 3,4
			if (flat.y>0)	return 3;
			else			return 4;
		} else {
			// sector 0,7
			if (flat.y>0)	return 0;
			else			return 7;
		}
	} else {
		// sector 1,2,6,5
		if (flat.x<0)	{
			// sector 2,5
			if (flat.y>0)	return 2;
			else			return 5;
		} else {
			// sector 1,6
			if (flat.y>0)	return 1;
			else			return 6;
		}
	}
}

class Marker {
private:
	DWORD	size;	// in dwords
	DWORD*	data;	// dword*
public:
	void	m_Create	(DWORD dim)
	{
		size	= dim/32+1;
		data	= (DWORD*) _aligned_malloc(size*4,64);
	}
	void	m_Destroy	()
	{
		_aligned_free(data);
		size	= 0;
		data	= 0;
	}
	void	clear	()
	{
		DWORD	cache	= size/16;				// in 64b cache lines
		DWORD	leaf	= (size-cache*16)*4;	// in bytes
		void	*dest	= LPVOID(data);
		__asm {
			femms
			mov			ecx,cache;
			mov			eax,dest;
			pxor		mm0,mm0
c_line:
			prefetchw	[eax+64*3];
			movq		[eax+0 ],mm0
			movq		[eax+8 ],mm0
			movq		[eax+16],mm0
			movq		[eax+24],mm0
			movq		[eax+32],mm0
			movq		[eax+40],mm0
			movq		[eax+48],mm0
			movq		[eax+56],mm0
			dec			ecx
			cmp			ecx,0
			jnz			c_line
			femms
		}
		ZeroMemory(LPBYTE(data)+size*4-leaf,leaf);
	}
	void	clear_x	()
	{
		ZeroMemory(data,size*4);
	}
	IC	void	set1	(DWORD id)
	{
		DWORD	word	= id/32;
		DWORD	bit		= id%32;
		DWORD	mask	= 1ul<<bit;
		data	[word]	|= mask;
	}
	IC	void	set0	(DWORD id)
	{
		DWORD	word	= id/32;
		DWORD	bit		= id%32;
		DWORD	mask	= 1ul<<bit;
		data	[word]	&= ~mask;
	}
	IC	BOOL	get	(DWORD id)
	{
		DWORD	word	= id/32;
		DWORD	bit		= id%32;
		DWORD	mask	= 1ul<<bit;
		return	data[word]&mask;
	}
};


// volumetric query
DEF_VECTOR		(Nearest,DWORD);

struct			Query
{
	Nearest		q_List;
	Marks		q_Marks;
	Fvector		q_Base;
	
	IC void		Perform	(DWORD ID)
	{
		if (ID==InvalidNode)		return;
		if (ID>=q_Marks.size())		return;
		if (q_Marks[ID])			return;
		
		q_Marks[ID]	= true;
		Node&	N	= g_nodes[ID];
		if (q_Base.distance_to_sqr(N.Pos)>cover_sqr_dist)	return;
		
		// ok
		q_List.push_back	(ID);
		
		Perform	(N.n1);
		Perform	(N.n2);
		Perform	(N.n3);
		Perform	(N.n4);
	}
};
struct	RC { RayCache	C; };

class	CoverThread : public CThread
{
	DWORD	Nstart, Nend;
public:
	CoverThread			(DWORD ID, DWORD _start, DWORD _end) : CThread(ID)
	{
		Nstart	= _start;
		Nend	= _end;
	}
	virtual void		Execute()
	{
		RAPID::XRCollide DB;
		DB.RayMode		(RAY_ONLYFIRST|RAY_CULL);
		
		vector<RC>		cache;
		{
			RC				rc;	
			rc.C[0].set		(0,0,0); 
			rc.C[1].set		(0,0,0); 
			rc.C[2].set		(0,0,0);
			
			cache.assign	(g_nodes.size()*2,rc);
		}

		Query			Q;
		for (DWORD N=Nstart; N<Nend; N++)
		{
			FPU::m24r	();
			
			// initialize process
			thProgress	= float(N-Nstart)/float(Nend-Nstart);
			Node&		BaseNode= g_nodes[N];
			Fvector&	BasePos	= BaseNode.Pos;
			Fvector		TestPos = BasePos; TestPos.y+=cover_height;
			
			DWORD	c_total	[8]	= {0,0,0,0,0,0,0,0};
			DWORD	c_passed[8]	= {0,0,0,0,0,0,0,0};
			
			// perform volumetric query
			Q.q_Marks.assign(g_nodes.size()+2,false);
			Q.q_List.clear	();
			Q.q_List.reserve(4096);
			Q.q_Base.set	(BasePos);
			Q.Perform		(N);
			
			// main cycle: trace rays and compute counts
			for (Nearest_it it=Q.q_List.begin()+1; it!=Q.q_List.end();  it++)
			{
				// calc dir & range
				DWORD		ID	= *it;
				R_ASSERT	(ID<g_nodes.size());
				Node&		N	= g_nodes[ID];
				Fvector&	Pos = N.Pos;
				Fvector		Dir;
				Dir.sub		(Pos,BasePos);
				float		range		= Dir.magnitude();
				Dir.div		(range);
				
				// raytrace
				int			sector		=	calcSphereSector(Dir);
				c_total		[sector]	+=	1;
				c_passed	[sector]	+=	(RayPick(&DB, TestPos, Dir, range, cache[ID].C)?0:1);
			}
			
			// analyze probabilities
			float	value	[8];
			for (int dirs=0; dirs<8; dirs++) 
			{
				R_ASSERT(c_passed[dirs]<=c_total[dirs]);
				if (c_total[dirs]==0)	value[dirs] = 0;
				else					value[dirs]	= float(c_passed[dirs])/float(c_total[dirs]);
				clamp(value[dirs],0.f,1.f);
			}
			
			BaseNode.cover	[0]	= (value[2]+value[3]+value[4]+value[5])/4.f; clamp(BaseNode.cover[0],0.f,1.f);	// left
			BaseNode.cover	[1]	= (value[0]+value[1]+value[2]+value[3])/4.f; clamp(BaseNode.cover[1],0.f,1.f);	// forward
			BaseNode.cover	[2]	= (value[6]+value[7]+value[0]+value[1])/4.f; clamp(BaseNode.cover[2],0.f,1.f);	// right
			BaseNode.cover	[3]	= (value[4]+value[5]+value[6]+value[7])/4.f; clamp(BaseNode.cover[3],0.f,1.f);	// back
		}
	}
};

#define NUM_THREADS	4
void	xrCover	()
{
	Status("Calculating...");

	// Start threads, wait, continue --- perform all the work
	DWORD	start_time		= timeGetTime();
	CThreadManager			Threads;
	DWORD	stride			= g_nodes.size()/NUM_THREADS;
	DWORD	last			= g_nodes.size()-stride*(NUM_THREADS-1);
	for (DWORD thID=0; thID<NUM_THREADS; thID++)
		Threads.start(new CoverThread(thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride)));
	Threads.wait			();
	Msg("%d seconds elapsed.",(timeGetTime()-start_time)/1000);

	// Smooth
	Status	("Smoothing coverage mask...");
	Nodes	Old = g_nodes;
	for (DWORD N=0; N<g_nodes.size(); N++)
	{
		Node&	Base		= Old[N];
		Node&	Dest		= g_nodes[N];
		
		for (int dir=0; dir<4; dir++)
		{
			float val		= 2*Base.cover[dir];
			float cnt		= 2;
			
			for (int nid=0; nid<4; nid++) {
				if (Base.n[nid]!=InvalidNode) {
					val		+=  Old[Base.n[nid]].cover[dir];
					cnt		+=	1.f;
				}
			}
			Dest.cover[dir]	=  val/cnt;
		}
	}
}
