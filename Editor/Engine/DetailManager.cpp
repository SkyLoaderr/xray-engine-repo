// DetailManager.cpp: implementation of the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DetailManager.h"
#include "fstaticrender.h"
#include "xr_creator.h"
#include "collide\cl_intersect.h"

float		psDetailDensity		= 0.15f;

const DWORD	vs_size				= 3000;
const float slot_size			= DETAIL_SLOT_SIZE;
const float slot_radius			= DETAIL_SLOT_SIZE*0.70710678118654752440084436210485f; // (slot_size/2)*sqrtf(2)

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
	s_min			= S->Rfloat	();
	s_max			= S->Rfloat	();
	number_vertices	= S->Rdword	();
	number_indices	= S->Rdword	();
	R_ASSERT		(0==(number_indices%3));

	// Vertices
	DWORD			size_vertices		= number_vertices*sizeof(fvfVertexIn); 
	vertices		= (CDetail::fvfVertexIn *)	_aligned_malloc	(size_vertices,64);
	S->Read			(vertices,size_vertices);

	// Indices
	DWORD			size_indices		= number_indices*sizeof(WORD);
	indices			= (WORD*)					_aligned_malloc	(size_indices,64);
	S->Read			(indices,size_indices);

	// Calc BB & SphereRadius
	Fbox bb;
	bb.invalidate	();
	for (DWORD i=0; i<number_vertices; i++)
		bb.modify	(vertices[i].P);
	radius			= bb.getradius()*2;
}

void CDetail::Unload	()
{
	if (vertices)		{ _aligned_free(vertices);	vertices=0; }
	if (indices)		{ _aligned_free(indices);	indices=0;	}
	Device.Shader.Delete(shader);
}

void CDetail::Transfer	(Fmatrix& mXform, fvfVertexOut* vDest, DWORD C, WORD* iDest, DWORD iOffset)
{
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
/*
*/
void CDetailManager::Load		()
{
	// Open file stream
	FILE_NAME	fn;
	if (!Engine.FS.Exist(fn,Path.Current,"level.details"))
	{
		dtFS	= NULL;
		return;
	}
	dtFS = new CVirtualFileStream(fn);

	// Header
	dtFS->ReadChunk	(0,&dtH);
	R_ASSERT		(dtH.version == DETAIL_VERSION);
	DWORD m_count	= dtH.object_count;

	// Models
	CStream* m_fs	= dtFS->OpenChunk(1);
	for (DWORD m_id = 0; m_id < m_count; m_id++)
	{
		CDetail				dt;
		CStream* S			= m_fs->OpenChunk(m_id);
		dt.Load				(S);
		objects.push_back	(dt);
		S->Close			();
	}
	m_fs->Close		();

	// Get pointer to database (slots)
	CStream* m_slots= dtFS->OpenChunk(2);
	dtSlots			= (DetailSlot*)m_slots->Pointer();
	m_slots->Close	();

	// Initialize 'vis' and 'cache'
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
	_DELETE			(dtFS);
}

extern float g_fSCREEN;
extern float ssaLIMIT;

void CDetailManager::Render		(Fvector& EYE)
{
	if (0==dtFS)	return;

	int s_x	= iFloor			(EYE.x/slot_size+.5f);
	int s_z	= iFloor			(EYE.z/slot_size+.5f);

	UpdateCache					(4);

	float fade_limit			= 14.5f;fade_limit=fade_limit*fade_limit;
	float fade_start			= 1.f;	fade_start=fade_start*fade_start;
	float fade_range			= fade_limit-fade_start;

	// Collect objects for rendering
	Device.Statistic.RenderDUMP_DT_VIS.Begin	();
	for (int _z=s_z-dm_size; _z<=(s_z+dm_size); _z++)
	{
		for (int _x=s_x-dm_size; _x<=(s_x+dm_size); _x++)
		{
			// Query for slot
			Device.Statistic.RenderDUMP_DT_Cache.Begin	();
			Slot&	S		= Query(_x,_z);
			S.dwFrameUsed	= Device.dwFrame;
			Device.Statistic.RenderDUMP_DT_Cache.End	();

			// Transfer visibile and partially visible slot contents
			BYTE mask		= 0xff;
			switch (::Render.ViewBase.testAABB(S.BB.min,S.BB.max,mask))
			{
			case fcvNone:		// nothing to do
				break;
			case fcvPartial:	// addition with TEST
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

							float	dist_sq = EYE.distance_to_sqr(Item.P);
							if (dist_sq>fade_limit)	continue;
							
							if (::Render.ViewBase.testSphereDirty(siIT->P,R*Item.scale))	
							{
								float	alpha	= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
								float	scale	= Item.scale*(1-alpha);
								float	radius	= R*scale;
								
								if (g_fSCREEN*radius*radius/dist_sq < ssaLIMIT) continue;

								Item.scale_calculated = scale; //alpha;
								vis.push_back	(siIT);
							}
						}
					}
				}
				break;
			case fcvFully:		// addition
				{
					for (int sp_id=0; sp_id<3; sp_id++)
					{
						SlotPart&			sp	= S.G		[sp_id];
						if (sp.id==0xff)	continue;
						CList<SlotItem*>&	vis = visible	[sp.id];
						float				R   = objects	[sp.id].radius;
						
						SlotItem			*siIT=sp.items.begin(), *siEND=sp.items.end();
						for (; siIT!=siEND; siIT++) 
						{
							SlotItem& Item	= *siIT;

							float	dist_sq = EYE.distance_to_sqr(Item.P);
							if (dist_sq>fade_limit)	continue;
							
							float	alpha	= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
							float	scale	= Item.scale*(1-alpha);
							float	radius	= R*scale;

							if (g_fSCREEN*radius*radius/dist_sq < ssaLIMIT) continue;

							Item.scale_calculated = scale; //alpha;
							vis.push_back	(siIT);
						}
					}
				}
				break;
			}
		}
	}
	Device.Statistic.RenderDUMP_DT_VIS.End	();

	Device.Statistic.RenderDUMP_DT_Render.Begin	();
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE));

	// Render itself
	float	fPhaseRange	= PI/16;
	float	fPhaseX		= sinf(Device.fTimeGlobal*0.1f)	*fPhaseRange;
	float	fPhaseZ		= sinf(Device.fTimeGlobal*0.11f)*fPhaseRange;

	// Get index-stream
	CIndexStream*	IS	= Device.Streams.Get_IB();

	for (DWORD O=0; O<dm_max_objects; O++)
	{
		CList<SlotItem*>&	vis = visible	[O];
		if (vis.empty())	continue;

		CDetail&	Object		= objects[O];
		DWORD	vCount_Object	= Object.number_vertices;
		DWORD	iCount_Object	= Object.number_indices;
		DWORD	vCount_Total	= vis.size()*vCount_Object;

		// calculate lock count needed
		DWORD	lock_count		= vCount_Total/vs_size;
		if	(vCount_Total>(lock_count*vs_size))	lock_count++;

		// calculate objects per lock
		DWORD	o_total			= vis.size();
		DWORD	o_per_lock		= o_total/lock_count;
		if  (o_total > (o_per_lock*lock_count))	o_per_lock++;

		// Fill VB (and flush it as nesessary)
		Device.Shader.Set				(Object.shader);
		Device.Shader.SetupPass			(0);
		Device.Primitive.setVertices	(VS->getFVF(),VS->getStride(),VS->getBuffer());

		Fmatrix		mXform,mRotXZ;
		for (DWORD L_ID=0; L_ID<lock_count; L_ID++)
		{
			// Calculate params 
			DWORD	item_start	= L_ID*o_per_lock;
			DWORD	item_end	= item_start+o_per_lock;
			if (item_end>o_total)	item_end = o_total;
			if (item_end<=item_start)	break;
			DWORD	item_range	= item_end-item_start;

			// Calc Lock params
			DWORD	vCount_Lock	= item_range*vCount_Object;
			DWORD	iCount_Lock = item_range*iCount_Object;
	
			// Lock buffers
			DWORD	vBase,iBase,iOffset=0;
			CDetail::fvfVertexOut* vDest	= (CDetail::fvfVertexOut*)	VS->Lock(vCount_Lock,vBase);
			WORD*	iDest					= (WORD*)					IS->Lock(iCount_Lock,iBase);
			WORD*	dbgIndices				= iDest;

			// Filling itself
			for (DWORD item=item_start; item<item_end; item++)
			{
				SlotItem&	Instance	= *(vis[item]);
				float	scale			= Instance.scale_calculated;

				// Build matrix
				if (scale>0.7f)	
				{
					mRotXZ.setXYZ			(Instance.phase_x+fPhaseX,0,Instance.phase_z+fPhaseZ);
					mXform.mul_43			(mRotXZ,Instance.mRotY);
					mXform._11				*= scale;
					mXform._22				*= scale;
					mXform._33				*= scale;
					mXform.translate_over	(Instance.P);
				} else {
					Fmatrix& M = Instance.mRotY;
					Fvector& P = Instance.P;
					mXform._11=M._11*scale;	mXform._12=M._12;		mXform._13=M._13;		mXform._14=M._14;
					mXform._21=M._21;		mXform._22=M._22*scale;	mXform._23=M._23;		mXform._24=M._24;
					mXform._31=M._31;		mXform._32=M._32;		mXform._33=M._33*scale;	mXform._34=M._34;
					mXform._41=P.x;			mXform._42=P.y;			mXform._43=P.z;			mXform._44=1;
				}
				
				// Transfer vertices
				{
					DWORD					C = Instance.C;
					CDetail::fvfVertexIn	*srcIt = Object.vertices, *srcEnd = Object.vertices+Object.number_vertices;
					CDetail::fvfVertexOut	*dstIt = vDest;
					for	(; srcIt!=srcEnd; srcIt++, dstIt++)
					{
						mXform.transform_tiny	(dstIt->P,srcIt->P);
						dstIt->C	= C;
						dstIt->u	= srcIt->u;
						dstIt->v	= srcIt->v;
					}
				}
				
				// Transfer indices (in 32bit lines)
				VERIFY	(iOffset<65535);
				{
					DWORD	item	= (iOffset<<16) | iOffset;
					DWORD	count	= Object.number_indices/2;
					LPDWORD	sit		= LPDWORD(Object.indices);
					LPDWORD	send	= sit+count;
					LPDWORD	dit		= LPDWORD(iDest);
					for		(; sit!=send; dit++,sit++)	*dit=*sit+item;
					if		(Object.number_indices&1)	
						iDest[Object.number_indices-1]=Object.indices[Object.number_indices-1]+WORD(iOffset);
				}
				
				// Increment counters
				vDest					+=	vCount_Object;
				iDest					+=	iCount_Object;
				iOffset					+=	vCount_Object;
			}
			VS->Unlock	(vCount_Lock);
			IS->Unlock	(iCount_Lock);

			// Render
			Device.Primitive.setIndicesUC	(vBase, IS->getBuffer());
			DWORD	dwNumPrimitives			= iCount_Lock/3;
			Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,vCount_Lock,iBase,dwNumPrimitives);
			UPDATEC							(vCount_Lock,dwNumPrimitives,2);
		}

		// Clean up
		vis.clear	();
	}
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW));
	Device.Statistic.RenderDUMP_DT_Render.End	();
}

CDetailManager::Slot&	CDetailManager::Query	(int sx, int sz)
{
	// Search cache
	DWORD oldest	= 0;
	for (DWORD I=0; I<cache.size(); I++)
	{
		Slot&	S = cache[I];
		if ((S.sx==sx)&&(S.sz==sz)&&(S.type!=stInvalid))	return S;
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
IC float	Interpolate			(float* base,		DWORD x, DWORD y, DWORD size)
{
	float	f	= float(size);
	float	fx	= float(x)/f; float ifx = 1.f-fx;
	float	fy	= float(y)/f; float ify = 1.f-fy;
	
	float	c01	= base[0]*ifx + base[1]*fx;
	float	c23	= base[2]*ifx + base[3]*fx;
	float	c02	= base[0]*ify + base[2]*fy;
	float	c13	= base[1]*ify + base[3]*fy;
	
	float	cx	= ify*c01 + fy*c23;
	float	cy	= ifx*c02 + fx*c13;
	return	(cx+cy)/2;
}
IC BOOL		InterpolateAndDither(float* alpha255,	DWORD x, DWORD y, DWORD size, int dither[16][16] )
{
	int		c	= iFloor(Interpolate(alpha255,x%size,y%size,size)+.5f);
	
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
		}
		
		// Prepare to selection
		float		density		= psDetailDensity;
		float		jitter		= density/2.f;
		DWORD		d_size		= iCeil	(slot_size/density);
		svector<int,dm_obj_in_slot>		selected;
		
		CRandom				r_selection	(0x12071980);
		CRandom				r_jitter	(0x12071980);
		CRandom				r_yaw		(0x12071980);
		CRandom				r_scale		(0x12071980);
		
		// Decompressing itself
		for (DWORD z=0; z<=d_size; z++)
		{
			for (DWORD x=0; x<=d_size; x++)
			{
				// shift mask
                int shift_x = r_jitter.randI(16);
                int shift_z = r_jitter.randI(16);

				// Iterpolate and dither palette
				selected.clear();
				if ((DS.items[0].id!=0xff)&& InterpolateAndDither(alpha255[0],x+shift_x,z+shift_z,d_size,dither))	selected.push_back(0);
				if ((DS.items[1].id!=0xff)&& InterpolateAndDither(alpha255[1],x+shift_x,z+shift_z,d_size,dither))	selected.push_back(1);
				if ((DS.items[2].id!=0xff)&& InterpolateAndDither(alpha255[2],x+shift_x,z+shift_z,d_size,dither))	selected.push_back(2);
				if ((DS.items[3].id!=0xff)&& InterpolateAndDither(alpha255[3],x+shift_x,z+shift_z,d_size,dither))	selected.push_back(3);
				
				// Select 
				if (selected.empty())	continue;
				DWORD index;
				if (selected.size()==1)	index = selected[0];
				else					index = selected[r_selection.randI(selected.size())]; 
				
				CDetail&	Dobj = objects[DS.items[index].id];
				SlotItem	Item;
				
				// Position (XZ)
				float		rx = (float(x)/float(d_size))*slot_size + D.BB.min.x;
				float		rz = (float(z)/float(d_size))*slot_size + D.BB.min.z;
				Item.P.set	(rx + r_jitter.randFs(jitter), D.BB.max.y, rz + r_jitter.randFs(jitter));

				// Position (Y)
				float y		= D.BB.min.y-5;
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
				Item.scale	= r_scale.randF		(Dobj.s_min,Dobj.s_max);
				Item.phase_x= ::Random.randFs	(phase_range);
				Item.phase_z= ::Random.randF	(phase_range);
				Item.mRotY.rotateY(r_yaw.randF	(0,PI_MUL_2));
				
				// Color
				DetailPalette*	c_pal	= (DetailPalette*)&DS.color;
				float gray255	[4];
				gray255[0]		= 255.f*float(c_pal->a0)/15.f;
				gray255[1]		= 255.f*float(c_pal->a1)/15.f;
				gray255[2]		= 255.f*float(c_pal->a2)/15.f;
				gray255[3]		= 255.f*float(c_pal->a3)/15.f;
				int c_dw		= iFloor(Interpolate(gray255,x,z,d_size)+.5f);
				clamp			(c_dw,0,255);
				Item.C			= D3DCOLOR_RGBA		(c_dw,c_dw,c_dw,255);
				
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

	int db_x = sx+dtH.offs_x;
	int db_z = sz+dtH.offs_z;
	if ((db_x>=0) && (db_x<int(dtH.size_x)) && (db_z>=0) && (db_z<int(dtH.size_z)))
	{
		DWORD linear_id = db_z*dtH.size_x + db_x;
		return dtSlots			[linear_id];
	} else {
		// Empty slot
		DS.y_min				= 0;
		DS.y_max				= EPS_L;
		
		DS.items[0].id			= 0xff;
		DS.items[1].id			= 0xff;
		DS.items[2].id			= 0xff;
		DS.items[3].id			= 0xff;
		
		DS.color				= 0xffff;
		return DS;
	}
}
