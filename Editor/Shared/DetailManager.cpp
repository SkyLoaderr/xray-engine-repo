// DetailManager.cpp: implementation of the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DetailManager.h"
#include "fstaticrender.h"
#include "detailformat.h"

const DWORD	vs_size				= 3000;
const float slot_size			= 4.f;
const float slot_radius			= 2.83f;

const float dbgOffset			= 0.f;
const int	dbgItems			= 128;

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
}

void CDetailManager::Unload		()
{
	for (DWORD it=0; it<objects.size(); it++)
		objects[it].Unload();
	objects.clear	();
	cache.clear		();
	visible.clear	();
}

void CDetailManager::Render		(Fvector& EYE)
{
	int s_x	= iFloor			(EYE.x/slot_size+.5f);
	int s_z	= iFloor			(EYE.z/slot_size+.5f);

	float fade_limit	= 11.5f;fade_limit=fade_limit*fade_limit;
	float fade_start	= 6.f;	fade_start=fade_start*fade_start;

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
					for (int sp_id=0; sp_id<3; sp_id++)
					{
						SlotPart&			sp	= S.G		[sp_id];
						CList<SlotItem>&	vis = visible	[sp.id];
						float				R   = objects	[sp.id].radius;
						
						SlotItem			*siIT=sp.items.begin(), *siEND=sp.items.end();
						for (; siIT!=siEND; siIT++) 
						{
							if (Device.vCameraPosition.distance_to_sqr(siIT->P)>fade_limit)	continue;
							if (::Render.ViewBase.testSphereDirty(siIT->P,R*siIT->scale))	vis.push_back	(*siIT);
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
			DWORD	item_range	= item_end-item_start;
			DWORD	vCount_Lock	= item_range*vCount_Object;
	
			// Fill VB
			CDetail::fvfVertexOut* vDest = (CDetail::fvfVertexOut*)VS->Lock(vCount_Lock,vOffset);
			for (DWORD item=item_start; item<item_end; item++)
			{
				SlotItem&	Instance	= vis[item];

				float	dist			= Device.vCameraPosition.distance_to_sqr(Instance.P);
				float	alpha			= (dist<fade_start)?0.f:(dist-fade_start)/(fade_limit-fade_start);
				float	scale			= Instance.scale*(1-alpha); 

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

const	float phase_range = PI/16;
void 	CDetailManager::Decompress		(int sx, int sz, Slot& D)
{
	// Debug purposes
	DetailSlot	DS;
	DS.y_min	= 0;
	DS.y_max	= .5f;
	DS.r_yaw	= 0xaaaaaaaa;
	DS.r_scale	= 0xaaaaaaaa;

	DS.items[0].id			= 0;
	DS.items[0].palette.a0	= 15;
	DS.items[0].palette.a1	= 0;
	DS.items[0].palette.a2	= 15;
	DS.items[0].palette.a3	= 0;

	DS.items[1].id			= 0xff;
	DS.items[2].id			= 0xff;
	DS.items[3].id			= 0xff;

	// Unpacking
	D.sx	= sx;
	D.sz	= sz;
	
	D.BB.min.set(sx*slot_size,			DS.y_min,	sz*slot_size);
	D.BB.max.set(D.BB.min.x+slot_size,	DS.y_max,	D.BB.min.z+slot_size);

	float density = 0.1f;
	for (int i=0; i<dm_obj_in_slot; i++)
	{
		SlotPart& SP	= D.G[i];
		SP.id			= DS.items[i].id;
		SP.items.clear	();
		if (SP.id==0xff)	continue;
		
		/*
		float	pal0	= float(DS.items[i].palette.a0)/15.f;
		float	pal1	= float(DS.items[i].palette.a1)/15.f;
		float	pal2	= float(DS.items[i].palette.a2)/15.f;
		float	pal3	= float(DS.items[i].palette.a3)/15.f;

		for (float _z=0; _z<1; _z+=density)
		{
			for (float _x=0; _x<1; _x+=density)
			{
				
			}
		}
		*/

		for (int j=0; j<dbgItems; j++)
		{
			SlotItem	Item;

			// Position
			float		rx = ::Random.randF	(D.BB.min.x,D.BB.max.x);
			float		rz = ::Random.randF	(D.BB.min.z,D.BB.max.z);
			Item.P.set	(rx,0,rz);

			// Angles and scale
			Item.yaw	= ::Random.randF	(0,PI_MUL_2);
			Item.phase_x= ::Random.randFs	(phase_range);
			Item.phase_z= ::Random.randF	(phase_range);
			Item.scale	= ::Random.randF	(0.3f,1.8f);

			// Color
			Item.C		= 0xffffffff;
			
			// Save it
			SP.items.push_back(Item);
		}
	}
}

/*

*/