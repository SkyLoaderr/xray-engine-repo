// DetailManager.cpp: implementation of the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "DetailManager.h"
#include "render.h"
#include "cl_intersect.h"

#ifdef _EDITOR
	#include "SceneClassList.h"
	#include "Scene.h"
	#include "SceneObject.h"
#else
	#include "xr_creator.h"
#endif

float		psDetailDensity		= 0.15f;

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
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDetailManager::CDetailManager	()
{
	dtFS 		= 0;
	dtSlots		= 0;
	soft_VS		= 0;
	hw_VS_wave	= 0;
	hw_VS_still	= 0;
	hw_BatchSize= 0;
	hw_VB		= 0;
	hw_IB		= 0;
}

CDetailManager::~CDetailManager	()
{
	
}
/*
*/
#ifndef _EDITOR
void CDetailManager::Load		()
{
	// Open file stream
	FILE_NAME	fn;
	if (!Engine.FS.Exist(fn,Path.Current,"level.details"))
	{
		dtFS	= NULL;
		return;
	}
	dtFS		= Engine.FS.Open(fn);
	
	// Header
	dtFS->ReadChunkSafe	(0,&dtH,sizeof(dtH));
	R_ASSERT			(dtH.version == DETAIL_VERSION);
	DWORD m_count		= dtH.object_count;
	
	// Models
	CStream* m_fs	= dtFS->OpenChunk(1);
	for (DWORD m_id = 0; m_id < m_count; m_id++)
	{
		CDetail*		dt	= new CDetail;
		CStream* S			= m_fs->OpenChunk(m_id);
		dt->Load			(S);
		objects.push_back	(dt);
		S->Close			();
	}
	m_fs->Close		();
	
	// Get pointer to database (slots)
	CStream* m_slots= dtFS->OpenChunk(2);
	dtSlots			= (DetailSlot*)m_slots->Pointer();
	m_slots->Close	();
	
	// Initialize 'vis' and 'cache'
	ZeroMemory(&visible,sizeof(visible));	
	visible[0].resize	(dm_max_objects);
	visible[1].resize	(dm_max_objects);
	visible[2].resize	(dm_max_objects);

	ZeroMemory(&cache,sizeof(cache));		cache.resize	(dm_cache_size);	
	for (DWORD s=0; s<cache.size(); s++)	cache[s].type	= stInvalid;
	
	// Make dither matrix
	bwdithermap		(2,dither);

	// Hardware specific optimizations
	if (UseVS())	hw_Load		();
	else			soft_Load	();
}
#endif
void CDetailManager::Unload		()
{
	if (UseVS())	hw_Unload	();
	else			soft_Unload	();
	
	for (DetailIt it=objects.begin(); it!=objects.end(); it++){
		(*it)->Unload();
		_DELETE		(*it);
    }
	objects.clear	();
	cache.clear		();
	visible[0].clear();
	visible[1].clear();
	visible[2].clear();
	_DELETE			(dtFS);
}

extern float r_ssaDISCARD;

void CDetailManager::Render		(Fvector& vecEYE)
{
#ifndef _EDITOR
	if (0==dtFS)	return;
#endif

	float	r_ssaCHEAP			= 16*r_ssaDISCARD;

	Fvector		EYE				= vecEYE;
	CFrustum	View			= ::Render->ViewBase;
    
	int s_x	= iFloor			(EYE.x/slot_size+.5f);
	int s_z	= iFloor			(EYE.z/slot_size+.5f);

	UpdateCache					(s_x,s_y,5);

	float fade_limit			= dm_fade;	fade_limit=fade_limit*fade_limit;
	float fade_start			= 1.f;		fade_start=fade_start*fade_start;
	float fade_range			= fade_limit-fade_start;

	// Collect objects for rendering
	Device.Statistic.RenderDUMP_DT_VIS.Begin	();
	for (int _z=s_z-dm_size; _z<=(s_z+dm_size); _z++)
	{
		for (int _x=s_x-dm_size; _x<=(s_x+dm_size); _x++)
		{
			// Query for slot
			Device.Statistic.RenderDUMP_DT_VIS.End		();
			Device.Statistic.RenderDUMP_DT_Cache.Begin	();
			Slot&	S		= Query(_x,_z);
			S.dwFrameUsed	= Device.dwFrame;
			Device.Statistic.RenderDUMP_DT_Cache.End	();
			Device.Statistic.RenderDUMP_DT_VIS.Begin	();

			// Transfer visibile and partially visible slot contents
			DWORD mask		= 0xff;
			DWORD res		= View.testAABB(S.BB.min,S.BB.max,mask);
			if ((fcvPartial==res)&&UseVS())	res = fcvFully;
			switch (res)
			{
			case fcvNone:		// nothing to do
				break;
			case fcvPartial:	// addition with TEST
				{
					if (!::Render->occ_visible(S.BB))		continue;
					for (int sp_id=0; sp_id<dm_obj_in_slot; sp_id++)
					{
						SlotPart&			sp	= S.G		[sp_id];
						if (sp.id==0xff)	continue;
						float				R   = objects	[sp.id]->bv_sphere.R;
						
						SlotItem			*siIT=sp.items.begin(), *siEND=sp.items.end();
						for (; siIT!=siEND; siIT++) 
						{
							SlotItem& Item	= *siIT;

							float	dist_sq = EYE.distance_to_sqr(Item.P);
							if (dist_sq>fade_limit)	continue;
							
							if (::Render->ViewBase.testSphere_dirty(siIT->P,R*Item.scale))	
							{
								float	alpha	= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
								float	scale	= Item.scale*(1-alpha);
								float	radius	= R*scale;
								
								float	ssa		= radius*radius/dist_sq;
								if (ssa < r_ssaDISCARD) continue;
								DWORD	vis_id	= Item.vis_ID;
								if (ssa < r_ssaCHEAP)	vis_id=0;
								
								Item.scale_calculated = scale;			//alpha;
								visible[vis_id][sp.id].push_back	(siIT);
							}
						}
					}
				}
				break;
			case fcvFully:		// addition
				{
					if (!::Render->occ_visible(S.BB))		continue;
					for (int sp_id=0; sp_id<3; sp_id++)
					{
						SlotPart&			sp	= S.G		[sp_id];
						if (sp.id==0xff)	continue;
						float				R   = objects	[sp.id]->bv_sphere.R;
						
						SlotItem			*siIT=sp.items.begin(), *siEND=sp.items.end();
						for (; siIT!=siEND; siIT++) 
						{
							SlotItem& Item	= *siIT;

							float	dist_sq = EYE.distance_to_sqr(Item.P);
							if (dist_sq>fade_limit)	continue;
							
							float	alpha	= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
							float	scale	= Item.scale*(1-alpha);
							float	radius	= R*scale;

							float	ssa		= radius*radius/dist_sq;
							if (ssa < r_ssaDISCARD) continue;
							DWORD	vis_id	= Item.vis_ID;
							if (ssa < r_ssaCHEAP)	vis_id=0;

							Item.scale_calculated = scale;			//alpha;
							visible[vis_id][sp.id].push_back	(siIT);
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
	if (UseVS())	hw_Render	();
	else			soft_Render	();
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

IC bool		InterpolateAndDither(float* alpha255,	DWORD x, DWORD y, DWORD sx, DWORD sy, DWORD size, int dither[16][16] )
{
	clamp 	(x,0ul,size-1);
	clamp 	(y,0ul,size-1);
	int		c	= iFloor(Interpolate(alpha255,x,y,size)+.5f);
    clamp   (c,0,255);

	DWORD	row	= (y+sy) % 16;
	DWORD	col	= (x+sx) % 16;
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
		D.G[i].id			= DS.items[i].id;
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
		Fvector		bC,bD;
		D.BB.get_CD			(bC,bD);
		XRC.box_options		(0); // BBOX_TRITEST
#ifdef _EDITOR
		// Select polygons
		SBoxPickInfoVec		pinf;
		Scene.BoxPick		(D.BB,pinf,GetSnapObjects());
		DWORD	triCount	= pinf.size();
#else
		XRC.box_query		(pCreator->ObjectSpace.GetStaticModel(),bC,bD);
		DWORD	triCount	= XRC.r_count	();
		CDB::TRI* tris		= pCreator->ObjectSpace.GetStaticTris();
#endif
		if (0==triCount)	continue;

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
		float		jitter		= density/1.7f;
		DWORD		d_size		= iCeil	(slot_size/density);
		svector<int,dm_obj_in_slot>		selected;
		
		CRandom				r_selection	(0x12071980^::Random.randI(32760));
		CRandom				r_jitter	(0x12071980^::Random.randI(32760));
		CRandom				r_yaw		(0x12071980^::Random.randI(32760));
		CRandom				r_scale		(0x12071980^::Random.randI(32760));

		// Prepare to actual-bounds-calculations
		Fbox				Bounds;	
		Bounds.invalidate	();
		
		// Decompressing itself
		for (DWORD z=0; z<=d_size; z++)
		{
			for (DWORD x=0; x<=d_size; x++)
			{
            	// shift
                DWORD shift_x =  r_jitter.randI(16);
                DWORD shift_z =  r_jitter.randI(16);

				// Iterpolate and dither palette
				selected.clear();
				if ((DS.items[0].id!=0xff)&& InterpolateAndDither(alpha255[0],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(0);
				if ((DS.items[1].id!=0xff)&& InterpolateAndDither(alpha255[1],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(1);
				if ((DS.items[2].id!=0xff)&& InterpolateAndDither(alpha255[2],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(2);
				if ((DS.items[3].id!=0xff)&& InterpolateAndDither(alpha255[3],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(3);
				
				// Select 
				if (selected.empty())	continue;
				DWORD index;
				if (selected.size()==1)	index = selected[0];
				else					index = selected[r_selection.randI(selected.size())]; 
				
				CDetail*	Dobj = objects[DS.items[index].id];
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
#ifdef _EDITOR                
                    Fvector verts[3];
                    SBoxPickInfo& I=pinf[tid];
		        	for (int k=0; k<(int)I.inf.size(); k++){
                    	R_ASSERT(I.s_obj);
                        I.s_obj->GetFaceWorld(I.e_mesh,I.inf[k].id,verts);
                        if (CDB::TestRayTri(Item.P,dir,verts,r_u,r_v,r_range,TRUE))
                        {
                            if (r_range>=0)	{
                                float y_test	= Item.P.y - r_range;
                                if (y_test>y)	y = y_test;
                            }
                        }
                    }
#else
					CDB::TRI&	T		= tris[XRC.r_begin()[tid].id];
					if (CDB::TestRayTri(Item.P,dir,T.verts,r_u,r_v,r_range,TRUE))
					{
						if (r_range>=0)	{
							float y_test	= Item.P.y - r_range;
							if (y_test>y)	y = y_test;
						}
					}
#endif
				}
				if (y<D.BB.min.y)	continue;
				Item.P.y	= y;
				
				// Angles and scale
				Item.scale	= r_scale.randF		(Dobj->s_min,Dobj->s_max);
				
				// X-Form BBox
				Fmatrix		mScale,mXform;
				Fbox		ItemBB;
				Item.mRotY.rotateY				(r_yaw.randF	(0,PI_MUL_2));
				Item.mRotY.translate_over		(Item.P);
				mScale.scale					(Item.scale,Item.scale,Item.scale);
				mXform.mul_43					(Item.mRotY,mScale);
				ItemBB.xform					(Dobj->bv_bb,mXform);
				Bounds.merge					(ItemBB);
				
				// Color
				DetailPalette*	c_pal	= (DetailPalette*)&DS.color;
				float gray255	[4];
				gray255[0]		=	255.f*float(c_pal->a0)/15.f;
				gray255[1]		=	255.f*float(c_pal->a1)/15.f;
				gray255[2]		=	255.f*float(c_pal->a2)/15.f;
				gray255[3]		=	255.f*float(c_pal->a3)/15.f;
				float c_f		=	Interpolate		(gray255,x,z,d_size)+.5f;
				int c_dw		=	iFloor			(c_f);
				clamp			(c_dw,0,255);
				Item.C_dw		=	D3DCOLOR_RGBA		(c_dw,c_dw,c_dw,255);
				Item.C			=	c_f/255.f;

				// Vis-sorting
				if (!UseVS())	
				{
					// Always still on CPU pipe
					Item.vis_ID	= 0;
				} else {
					if (Dobj->flags&DO_NO_WAVING)	Item.vis_ID	= 0;
					else 
					{
						if (::Random.randI(0,3)==0)	Item.vis_ID	= 2;	// Second wave
						else						Item.vis_ID = 1;	// First wave
					}
				}
				
				// Save it
				D.G[index].items.push_back(Item);
			}
		}

		// Check for number of decompressions
		limit--;
		D.BB.set	(Bounds);
	}
}

DetailSlot&	CDetailManager::QueryDB(int sx, int sz)
{
	int db_x = sx+dtH.offs_x;
	int db_z = sz+dtH.offs_z;
	if ((db_x>=0) && (db_x<int(dtH.size_x)) && (db_z>=0) && (db_z<int(dtH.size_z)))
	{
		DWORD linear_id = db_z*dtH.size_x + db_x;
		return dtSlots			[linear_id];
	} else {
		// Empty slot
		DS_empty.y_min				= 0;
		DS_empty.y_max				= EPS_L;
		
		DS_empty.items[0].id		= 0xff;
		DS_empty.items[1].id		= 0xff;
		DS_empty.items[2].id		= 0xff;
		DS_empty.items[3].id		= 0xff;
		
		DS_empty.color				= 0xffff;
		return DS_empty;
	}
}
