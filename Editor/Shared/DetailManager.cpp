// DetailManager.cpp: implementation of the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DetailManager.h"
#include "fstaticrender.h"
#include "xr_creator.h"
#include "collide\cl_intersect.h"

const DWORD	vs_size				= 3000;
const float slot_size			= 4.f;
const float slot_radius			= 2.83f;

const float dbgOffset			= 0.f;
const int	dbgItems			= 128;

//--------------------------------------------------- Decompression
static int magic4x4[4][4] =  
{
 	 0, 14,  3, 13,
	11,  5,  8,  6,
	12,  2, 15,  1,
	 7,  9,  4, 10
};

void bwdithermap	(int levels, int magic[16][16] )
{
	/* Get size of each step */
    float N = 255.0f / (levels - 1);    
	
	/*
	* Expand 4x4 dither pattern to 16x16.  4x4 leaves obvious patterning,
	* and doesn't give us full intensity range (only 17 sublevels).
	* 
	* magicfact is (N - 1)/16 so that we get numbers in the matrix from 0 to
	* N - 1: mod N gives numbers in 0 to N - 1, don't ever want all
	* pixels incremented to the next level (this is reserved for the
	* pixel value with mod N == 0 at the next level).
	*/
	
    float	magicfact = (N - 1) / 16;
    for ( int i = 0; i < 4; i++ )
		for ( int j = 0; j < 4; j++ )
			for ( int k = 0; k < 4; k++ )
				for ( int l = 0; l < 4; l++ )
					magic[4*k+i][4*l+j] =
					(int)(0.5 + magic4x4[i][j] * magicfact +
					(magic4x4[k][l] / 16.) * magicfact);
}
//--------------------------------------------------- Decompression
void CDetail::Load		(CStream* S)
{
	// Shader
	FILE_NAME		fnT,fnS;
	S->RstringZ		(fnS);
	S->RstringZ		(fnT);
	shader			= Device.Shader.Create(fnS,fnT);

	// Params
	flags			= S->Rdword	();
	number_vertices	= S->Rdword	();
	R_ASSERT		(0==(number_vertices%3));

	// Vertices
	DWORD			size_vertices		= number_vertices*sizeof(fvfVertexIn); 
	vertices		= (CDetail::fvfVertexIn *)_aligned_malloc	(size_vertices,64);
	S->Read			(vertices,size_vertices);

	// Calc BB & SphereRadius
	Fbox bb;
	bb.invalidate	();
	for (DWORD i=0; i<number_vertices; i++)
		bb.modify	(vertices[i].P);
	radius			= bb.getradius();
}

void CDetail::Unload	()
{
	if (vertices)		{ _aligned_free(vertices); vertices=0; }
	Device.Shader.Delete(shader);
}

void CDetail::Transfer	(Fmatrix& mXform, fvfVertexOut* vD, DWORD C)
{
	// Transfer vertices
	{
		fvfVertexIn		*srcIt = vertices, *srcEnd = vertices+number_vertices;
		fvfVertexOut	*dstIt = vD;
		for	(; srcIt!=srcEnd; srcIt++, dstIt++)
		{
			mXform.transform_tiny	(dstIt->P,srcIt->P);
			dstIt->C	= C;
			dstIt->u	= srcIt->u;
			dstIt->v	= srcIt->v;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDetailManager::CDetailManager	()
{
	VS	= Device.Streams.Create	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, vs_size);
}

CDetailManager::~CDetailManager	()
{

}

void CDetailManager::Load		()
{
	{
		CFileStream fs("x:\\dbg\\flower_list.do");
		CDetail		dt;
		dt.Load		(&fs);
		objects.push_back(dt);
	}
	{
		CFileStream fs("x:\\dbg\\flower_yellow.do");
		CDetail		dt;
		dt.Load		(&fs);
		objects.push_back(dt);
	}
	/*
	{
		CFileStream fs("x:\\dbg\\gr_bush.do");
		CDetail		dt;
		dt.Load		(&fs);
		objects.push_back(dt);
	}
	*/
	{
		CFileStream fs("x:\\dbg\\flower_podor.do");
		CDetail		dt;
		dt.Load		(&fs);
		objects.push_back(dt);
	}
	{
		CFileStream fs("x:\\dbg\\flower_violet.do");
		CDetail		dt;
		dt.Load		(&fs);
		objects.push_back(dt);
	}
	{
		CFileStream fs("x:\\dbg\\grass.do");
		CDetail		dt;
		dt.Load		(&fs);
		objects.push_back(dt);
	}
	ZeroMemory(&visible,sizeof(visible));	visible.resize	(dm_max_objects);
	ZeroMemory(&cache,sizeof(cache));		cache.resize	(dm_cache_size);	
	for (DWORD s=0; s<cache.size(); s++)	cache[s].type	= stInvalid;

	// Make dither matrix
	bwdithermap		(2,dither);
}

void CDetailManager::Unload		()
{
	for (DWORD it=0; it<objects.size(); it++)
		objects[it].Unload();
	objects.clear	();
	cache.clear		();
	visible.clear	();
}

extern float g_fSCREEN;
IC	float	CalcSSA(float& distSQ, Fvector& C, FBasicVisual* V)
{
	float R	= V->bv_Radius;
	distSQ	= Device.vCameraPosition.distance_to_sqr(C);
	return	g_fSCREEN*R*R/distSQ;
}

void CDetailManager::Render		(Fvector& EYE)
{
	int s_x	= iFloor			(EYE.x/slot_size+.5f);
	int s_z	= iFloor			(EYE.z/slot_size+.5f);

	UpdateCache					(1);

	float fade_limit	= 13.5f;fade_limit=fade_limit*fade_limit;
	float fade_start	= 8.f;	fade_start=fade_start*fade_start;

	// Collect objects for rendering
	for (int _z=s_z-dm_size; _z<=(s_z+dm_size); _z++)
	{
		for (int _x=s_x-dm_size; _x<=(s_x+dm_size); _x++)
		{
			// Query for slot
			Slot&	S		= Query(_x,_z);
			S.dwFrameUsed	= Device.dwFrame;

			// Transfer visibile and partially visible slot contents
			BYTE mask		= 0xff;
			switch (::Render.ViewBase.testAABB(S.BB.min,S.BB.max,mask))
			{
			case fcvNone:		// nothing to do
				break;
			case fcvPartial:	// addition with TEST
			case fcvFully:		// addition
				{
					for (int sp_id=0; sp_id<dm_obj_in_slot; sp_id++)
					{
						SlotPart&			sp	= S.G		[sp_id];
						if (sp.id==0xff)	continue;
						CList<SlotItem*>&	vis = visible	[sp.id];
						float				R   = objects	[sp.id].radius;
						
						SlotItem			*siIT=sp.items.begin(), *siEND=sp.items.end();
						for (; siIT!=siEND; siIT++) 
						{
							SlotItem& Item	= *siIT;

							float	dist_sq = Device.vCameraPosition.distance_to_sqr(Item.P);
							if (dist_sq>fade_limit)	continue;

							float	alpha	= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/(fade_limit-fade_start);
							float	scale	= Item.scale*(1-alpha);
							float	radius	= R*scale;

							if (::Render.ViewBase.testSphereDirty(siIT->P,R*scale))	
							{
								Item.scale_calculated = scale;
								vis.push_back(siIT);
							}
						}
					}
				}
				break;
/*
				{
					for (int sp_id=0; sp_id<3; sp_id++)
					{
						SlotPart&			sp	= S.G		[sp_id];
						CList<SlotItem>&	vis = visible	[sp.id];
						float				R   = objects	[sp.id].radius;
						
						SlotItem			*siIT=sp.items.begin(), *siEND=sp.items.end();
						for (; siIT!=siEND; siIT++)	vis.push_back(*siIT);
					}
				}
				break;
*/
			}
		}
	}

	HW.pDevice->SetTransform(D3DTS_WORLD,precalc_identity.d3d());

	// Render itself
	DWORD	vOffset;
	float	fPhaseRange	= PI/16;
	float	fPhaseX		= sinf(Device.fTimeGlobal*0.1f)	*fPhaseRange;
	float	fPhaseZ		= sinf(Device.fTimeGlobal*0.11f)*fPhaseRange;

	for (DWORD O=0; O<dm_max_objects; O++)
	{
		CList<SlotItem>&	vis = visible	[O];
		if (vis.empty())	continue;

		CDetail&	Object		= objects[O];
		DWORD	vCount_Object	= Object.number_vertices;
		DWORD	vCount_Total	= vis.size()*vCount_Object;

		// calculate lock count needed
		DWORD	lock_count		= vCount_Total/vs_size;
		if	(vCount_Total>(lock_count*vs_size))	lock_count++;

		// calculate objects per lock
		DWORD	o_total			= vis.size();
		DWORD	o_per_lock		= o_total/lock_count;
		if  (o_total > (o_per_lock*lock_count))	o_per_lock++;

		// Fill VB (and flush it as nesessary)
		Device.Shader.Set		(Object.shader);
		Fmatrix		mXform,mScale,mRot,mRotY,mRotXZ;
		for (DWORD L_ID=0; L_ID<lock_count; L_ID++)
		{
			// Calculate params
			DWORD	item_start	= L_ID*o_per_lock;
			DWORD	item_end	= item_start+o_per_lock;
			if (item_end>o_total)	item_end = o_total;
			if (item_end<=item_start)	break;
			DWORD	item_range	= item_end-item_start;
			DWORD	vCount_Lock	= item_range*vCount_Object;
	
			// Fill VB
			CDetail::fvfVertexOut* vDest = (CDetail::fvfVertexOut*)VS->Lock(vCount_Lock,vOffset);
			for (DWORD item=item_start; item<item_end; item++)
			{
				SlotItem&	Instance	= vis[item];
				float	scale			= Instance.scale_calculated; 

				// Build matrix and xform vertices
				mScale.scale			(scale,scale,scale);
				mRotY.rotateY			(Instance.yaw);
				mRotXZ.setXYZ			(Instance.phase_x+fPhaseX,0,Instance.phase_z+fPhaseZ);
				mRot.mul_43				(mRotXZ,mRotY);
				mXform.mul_43			(mRot,mScale);
				mXform.translate_over	(Instance.P);
				Object.Transfer			(mXform, vDest, Instance.C);
				vDest					+=	vCount_Object;
			}
			VS->Unlock	(vCount_Lock);

			// Render
			Device.Primitive.Draw	(VS,vCount_Lock/3,vOffset);
		}

		// Clean up
		vis.clear	();
	}
}

CDetailManager::Slot&	CDetailManager::Query	(int sx, int sz)
{
	// Search cache
	DWORD oldest	= 0;
	for (DWORD I=0; I<cache.size(); I++)
	{
		Slot&	S = cache[I];
		if ((S.sx==sx)&&(S.sz==sz))	return S;
		else {
			if (S.dwFrameUsed < cache[oldest].dwFrameUsed)	oldest = I;
		}
	}

	// Cache MISS or cache not filled at all
	if (cache.size()<dm_cache_size)	{
		// Create new entry
		cache.push_back(Slot());
		Decompress(sx,sz,cache.back());
		return cache.back();
	} else {
		// We should discard oldest cache entry on LRU basis
		Slot& S = cache[oldest];
		Decompress(sx,sz,S);
		return S;
	}
}

//--------------------------------------------------- Decompression
IC bool InterpolateAndDither(float* alpha255, DWORD x, DWORD y, DWORD size, int dither[16][16] )
{
	float	f	= float(size);
	float	fx	= float(x)/f; float ifx = 1.f-fx;
	float	fy	= float(y)/f; float ify = 1.f-fy;
	
	float	c01	= alpha255[0]*ifx+alpha255[1]*fx;
	float	c23	= alpha255[2]*ifx+alpha255[3]*fx;
	float	c02	= alpha255[0]*ify+alpha255[2]*fy;
	float	c13	= alpha255[1]*ify+alpha255[3]*fy;
	
	float	cx	= ify*c01 + fy*c23;
	float	cy	= ifx*c02 + fx*c13;
	
	int		c	= iFloor((cx+cy)/2+.5f);
	
	DWORD	row	= y % 16; 
	DWORD	col	= x % 16;
 	return	c	> dither[col][row];
}

void 	CDetailManager::Decompress		(int sx, int sz, Slot& D)
{
	DetailSlot&	DS			= QueryDB(sx,sz);

	// Unpacking
	D.type					= stPending;
	D.sx					= sx;
	D.sz					= sz;

	D.BB.min.set			(sx*slot_size,			DS.y_min,	sz*slot_size);
	D.BB.max.set			(D.BB.min.x+slot_size,	DS.y_max,	D.BB.min.z+slot_size);
	D.BB.grow				(EPS_L);

	for (int i=0; i<dm_obj_in_slot; i++)
	{
		D.G[i].id		= DS.items[i].id;
		D.G[i].items.clear	();
	}
}

const	float phase_range = PI/16;
void CDetailManager::UpdateCache	(int limit)
{
	for (DWORD entry=0; limit && (entry<cache.size()); entry++)
	{
		if (cache[entry].type != stPending)	continue;
		
		// Gain access to data
		Slot&		D	= cache[entry];
		DetailSlot&	DS	= QueryDB(D.sx,D.sz);
		D.type			= stReady;
		
		// Select polygons
		XRC.BBoxMode		(0); // BBOX_TRITEST
		XRC.BBoxCollide		(precalc_identity,pCreator->ObjectSpace.GetStaticModel(),precalc_identity,D.BB);
		DWORD	triCount	= XRC.GetBBoxContactCount();
		if (0==triCount)	continue;
		RAPID::tri* tris	= pCreator->ObjectSpace.GetStaticTris();

		// Build shading table
		float		alpha255	[dm_obj_in_slot][4];
		for (int i=0; i<dm_obj_in_slot; i++)
		{
			alpha255[i][0]	= 255.f*float(DS.items[i].palette.a0)/15.f;
			alpha255[i][1]	= 255.f*float(DS.items[i].palette.a1)/15.f;
			alpha255[i][2]	= 255.f*float(DS.items[i].palette.a2)/15.f;
			alpha255[i][3]	= 255.f*float(DS.items[i].palette.a3)/15.f;
			D.G[i].id		= DS.items[i].id;
		}
		
		// Prepare to selection
		float		density		= 0.1f;
		float		jitter		= density/2.f;
		DWORD		d_size		= iCeil	(slot_size/density);
		svector<int,dm_obj_in_slot>		selected;
		
		CRandom				r_selection	(0x12071980);
		CRandom				r_jitter	(0x12071980);
		CRandom				r_yaw		(DS.r_yaw);
		CRandom				r_scale		(DS.r_scale);
		
		// Decompressing itself
		for (DWORD z=0; z<d_size; z++)
		{
			for (DWORD x=0; x<d_size; x++)
			{
				// Iterpolate and dither palette
				selected.clear();
				if ((DS.items[0].id!=0xff)&& InterpolateAndDither(alpha255[0],x,z,d_size,dither))	selected.push_back(0);
				if ((DS.items[1].id!=0xff)&& InterpolateAndDither(alpha255[1],x,z,d_size,dither))	selected.push_back(1);
				if ((DS.items[2].id!=0xff)&& InterpolateAndDither(alpha255[2],x,z,d_size,dither))	selected.push_back(2);
				if ((DS.items[3].id!=0xff)&& InterpolateAndDither(alpha255[3],x,z,d_size,dither))	selected.push_back(3);
				
				// Select 
				if (selected.empty())	continue;
				DWORD ID,index;
				if (selected.size()==1)	{ index = selected[0]; ID = DS.items[index].id; }
				else					{ index = selected[r_selection.randI(selected.size())]; ID = DS.items[index].id; }
				
				
				SlotItem	Item;
				
				// Position (XZ)
				float		rx = (float(x)/float(d_size))*slot_size + D.BB.min.x;
				float		rz = (float(z)/float(d_size))*slot_size + D.BB.min.z;
				Item.P.set	(rx + r_jitter.randFs(jitter), D.BB.max.y, rz + r_jitter.randFs(jitter));

				// Position (Y)
				float y		= D.BB.min.y-1;
				Fvector	dir; dir.set(0,-1,0);

				float		r_u,r_v,r_range;
				for (DWORD tid=0; tid<triCount; tid++)
				{
					RAPID::tri&	T		= tris[XRC.BBoxContact[tid].id];
					if (RAPID::TestRayTri(Item.P,dir,T.verts,r_u,r_v,r_range,TRUE))
					{
						if (r_range>=0)	{
							float y_test	= Item.P.y - r_range;
							if (y_test>y)	y = y_test;
						}
					}
				}
				if (y<D.BB.min.y)	continue;
				Item.P.y	= y;

				
				// Angles and scale
				Item.yaw	= r_yaw.randF		(0,PI_MUL_2);
				Item.scale	= r_scale.randF		(0.3f,1.8f);
				Item.phase_x= ::Random.randFs	(phase_range);
				Item.phase_z= ::Random.randF	(phase_range);
				
				// Color
				Item.C		= 0xffffffff;
				
				// Save it
				D.G[index].items.push_back(Item);
			}
		}

		// Check for number of decompressions
		limit--;
	}
}

DetailSlot&	CDetailManager::QueryDB(int sx, int sz)
{
	static DetailSlot	DS;

	// Debug purposes
	DS.y_min				= 0;
	DS.y_max				= .3f;
	DS.r_yaw				= 0xaaaaaaaa;
	DS.r_scale				= 0xaaaaaaaa;

	DS.items[0].id			= 1;
	DS.items[0].palette.a0	= 15;
	DS.items[0].palette.a1	= 15;
	DS.items[0].palette.a2	= 15;
	DS.items[0].palette.a3	= 15;

	DS.items[1].id			= 0xff;
	DS.items[2].id			= 0xff;
	DS.items[3].id			= 0xff;

	return DS;
}
