#include "stdafx.h"
#include "compiler.h"
#include "cl_intersect.h"

#include "xrThread.h"

// -------------------------------- Ray pick
typedef Fvector	RayCache[3];

IC bool RayPick(CDB::COLLIDER* DB, Fvector& P, Fvector& D, float r, RayCache& C)
{
	// 1. Check cached polygon
	float _u,_v,range;
	if (CDB::TestRayTri(P,D,&C[0],_u,_v,range,true)) 
	{
		if (range>0 && range<r) return true;
	}

	// 2. Polygon doesn't pick - real database query
	try { DB->ray_query	(&Level,P,D,r); } catch (...) { Msg("* ERROR: Failed to trace ray"); }
	if (0==DB->r_count()) {
		return false;
	} else {
		// cache polygon
		CDB::RESULT&	rp	= *DB->r_begin();
		CDB::TRI&		T	= Level.get_tris()[rp.id];
		C[0].set	(*T.verts[0]);
		C[1].set	(*T.verts[1]);
		C[2].set	(*T.verts[2]);
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

/*
class Marker {
private:
	u32	size;	// in dwords
	u32*	data;	// dword*
public:
	void	m_Create	(u32 dim)
	{
		size	= dim/32+1;
		data	= (u32*) _aligned_malloc(size*4,64);
	}
	void	m_Destroy	()
	{
		_aligned_free(data);
		size	= 0;
		data	= 0;
	}
	void	clear	()
	{
		u32	cache	= size/16;				// in 64b cache lines
		u32	leaf	= (size-cache*16)*4;	// in bytes
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
	IC	void	set1	(u32 id)
	{
		u32	word	= id/32;
		u32	bit		= id%32;
		u32	mask	= 1ul<<bit;
		data	[word]	|= mask;
	}
	IC	void	set0	(u32 id)
	{
		u32	word	= id/32;
		u32	bit		= id%32;
		u32	mask	= 1ul<<bit;
		data	[word]	&= ~mask;
	}
	IC	BOOL	get	(u32 id)
	{
		u32	word	= id/32;
		u32	bit		= id%32;
		u32	mask	= 1ul<<bit;
		return	data[word]&mask;
	}
};
*/

// volumetric query
DEF_VECTOR		(Nearest,u32);

class			Query
{
public:
	Nearest		q_List;
	Nearest		q_Clear;
	Marks		q_Marks;
	Fvector		q_Base;
	
	IC void		Begin	(int count)
	{
		q_List.reserve	(8192);
		q_Clear.reserve	(8192);
		q_Marks.assign	(count,false);
	}

	IC void		Init	(Fvector& P)
	{
		q_Base.set		(P);
		q_List.clear	();
		q_Clear.clear	();
	}

	IC void		Perform	(u32 ID)
	{
		if (ID==InvalidNode)		return;
		if (ID>=q_Marks.size())		return;
		if (q_Marks[ID])			return;
		
		q_Marks[ID]			= true;
		q_Clear.push_back	(ID);

		vertex&	N			= g_nodes[ID];
		if (q_Base.distance_to_sqr(N.Pos)>cover_sqr_dist)	return;
		
		// ok
		q_List.push_back	(ID);
		
		Perform	(N.n1);
		Perform	(N.n2);
		Perform	(N.n3);
		Perform	(N.n4);
	}

	IC void		Clear	()
	{
		for (Nearest_it it=q_Clear.begin(); it!=q_Clear.end();  it++)
			q_Marks[*it]	= false;
	}
};
struct	RC { RayCache	C; };

class	CoverThread : public CThread
{
	u32	Nstart, Nend;
public:
	CoverThread			(u32 ID, u32 _start, u32 _end) : CThread(ID)
	{
		Nstart	= _start;
		Nend	= _end;
	}
	virtual void		Execute()
	{
		CDB::COLLIDER DB;
		DB.ray_options	(CDB::OPT_ONLYFIRST);
		
		xr_vector<RC>		cache;
		{
			RC				rc;	
			rc.C[0].set		(0,0,0); 
			rc.C[1].set		(0,0,0); 
			rc.C[2].set		(0,0,0);
			
			cache.assign	(g_nodes.size()*2,rc);
		}

		FPU::m24r		();
		Query			Q;
		Q.Begin			(g_nodes.size());
		for (u32 N=Nstart; N<Nend; N++)
		{
			// initialize process
			thProgress	= float(N-Nstart)/float(Nend-Nstart);
			vertex&		BaseNode= g_nodes[N];
			Fvector&	BasePos	= BaseNode.Pos;
			Fvector		TestPos = BasePos; TestPos.y+=cover_height;
			
			u32	c_total	[8]	= {0,0,0,0,0,0,0,0};
			u32	c_passed[8]	= {0,0,0,0,0,0,0,0};
			
			// perform volumetric query
			Q.Init			(BasePos);
			Q.Perform		(N);
			
			// main cycle: trace rays and compute counts
			for (Nearest_it it=Q.q_List.begin(); it!=Q.q_List.end();  it++)
			{
				// calc dir & range
				u32		ID			= *it;
				R_ASSERT	(ID<g_nodes.size());
				if			(N==ID)		continue;
				vertex&		N			= g_nodes[ID];
				Fvector&	Pos			= N.Pos;
				Fvector		Dir;
				Dir.sub		(Pos,BasePos);
				float		range		= Dir.magnitude();
				Dir.div		(range);
				
				// raytrace
				int			sector		=	calcSphereSector(Dir);
				c_total		[sector]	+=	1;
				c_passed	[sector]	+=	(RayPick(&DB, TestPos, Dir, range, cache[ID].C)?0:1);
			}
			Q.Clear			();
			
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

#define NUM_THREADS	3
extern	void mem_Optimize();
void	xrCover	()
{
	Status("Calculating...");

	// Start threads, wait, continue --- perform all the work
	u32	start_time		= timeGetTime();
	CThreadManager		Threads;
	u32	stride			= g_nodes.size()/NUM_THREADS;
	u32	last			= g_nodes.size()-stride*(NUM_THREADS-1);
	for (u32 thID=0; thID<NUM_THREADS; thID++)
		Threads.start(xr_new<CoverThread>(thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride)));
	Threads.wait			();
	Msg("%d seconds elapsed.",(timeGetTime()-start_time)/1000);

	// Smooth
	Status			("Smoothing coverage mask...");
	mem_Optimize	();
	Nodes	Old		= g_nodes;
	for (u32 N=0; N<g_nodes.size(); N++)
	{
		vertex&	Base		= Old[N];
		vertex&	Dest		= g_nodes[N];
		
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
