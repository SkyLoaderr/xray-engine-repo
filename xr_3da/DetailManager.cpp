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
#endif

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
	soft_Geom	= 0;
	hw_Geom		= 0;
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

/*
void dump	(CDetailManager::vis_list& lst)
{
	for (int i=0; i<lst.size(); i++)
	{
		Msg("%8x / %8x / %8x",	lst[i]._M_start, lst[i]._M_finish, lst[i]._M_end_of_storage._M_data);
	}
}
*/
void CDetailManager::Load		()
{
	// Open file stream
	if (!FS.exist("$level$","level.details"))
	{
		dtFS	= NULL;
		return;
	}

	string256			fn;
	FS.update_path		(fn,"$level$","level.details");
	dtFS				= FS.r_open(fn);

	// Header
	dtFS->r_chunk_safe	(0,&dtH,sizeof(dtH));
	R_ASSERT			(dtH.version == DETAIL_VERSION);
	u32 m_count			= dtH.object_count;

	// Models
	IReader* m_fs		= dtFS->open_chunk(1);
	for (u32 m_id = 0; m_id < m_count; m_id++)
	{
		CDetail*		dt	= xr_new<CDetail> ();
		IReader* S			= m_fs->open_chunk(m_id);
		dt->Load			(S);
		objects.push_back	(dt);
		S->close			();
	}
	m_fs->close		();

	// Get pointer to database (slots)
	IReader* m_slots	= dtFS->open_chunk(2);
	dtSlots				= (DetailSlot*)m_slots->pointer();
	m_slots->close		();

	// Initialize 'vis' and 'cache'
	visible[0].resize	(objects.size());	// dump(visible[0]);
	visible[1].resize	(objects.size());	// dump(visible[1]);
	visible[2].resize	(objects.size());	// dump(visible[2]);
	cache_Initialize	();

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
		xr_delete		(*it);
    }
	objects.clear	();
	visible[0].clear();
	visible[1].clear();
	visible[2].clear();
	xr_delete			(dtFS);
}

extern float r_ssaDISCARD;


void CDetailManager::Render		(Fvector& vecEYE)
{
#ifndef _EDITOR
	if (0==dtFS)						return;
#endif
	if (!psDeviceFlags.is(rsDetails))	return;

	float	r_ssaCHEAP			= 16*r_ssaDISCARD;

	Fvector		EYE				= vecEYE;
	CFrustum	View			= ::Render->ViewBase;

	int s_x	= iFloor			(EYE.x/dm_slot_size+.5f);
	int s_z	= iFloor			(EYE.z/dm_slot_size+.5f);

	Device.Statistic.RenderDUMP_DT_Cache.Begin	();
	cache_Update				(s_x,s_z,EYE,dm_max_decompress);
	Device.Statistic.RenderDUMP_DT_Cache.End	();

	float fade_limit			= dm_fade;	fade_limit=fade_limit*fade_limit;
	float fade_start			= 1.f;		fade_start=fade_start*fade_start;
	float fade_range			= fade_limit-fade_start;

	// Collect objects for rendering
	Device.Statistic.RenderDUMP_DT_VIS.Begin	();
	for (int _z=-dm_size; _z<=dm_size; _z++)
	{
		for (int _x=-dm_size; _x<=dm_size; _x++)
		{
			// Query for slot
			Slot*	CS		= cache_Query(_x,_z);
			Slot&	S		= *CS;

			// Transfer visibile and partially visible slot contents
			u32 mask		= 0xff;
			u32 res			= View.testSAABB(S.vis.sphere.P,S.vis.sphere.R,S.vis.box.min,S.vis.box.max,mask);
			if ((fcvPartial==res)&&UseVS())	res = fcvFully;
			switch (res)
			{
			case fcvNone:		// nothing to do
				break;
			case fcvPartial:	// addition with TEST
				{
					if (!::Render->occ_visible(S.vis))		continue;
					for (int sp_id=0; sp_id<dm_obj_in_slot; sp_id++)
					{
						SlotPart&			sp	= S.G		[sp_id];
						if (sp.id==0xff)	continue;
						float				R   = objects	[sp.id]->bv_sphere.R;

						SlotItem			**siIT=sp.items.begin(), **siEND=sp.items.end();
						for (; siIT!=siEND; siIT++)
						{
							SlotItem& Item	= *(*siIT);

							float	dist_sq = EYE.distance_to_sqr(Item.P);
							if (dist_sq>fade_limit)	continue;

							if (::Render->ViewBase.testSphere_dirty(Item.P,R*Item.scale))
							{
								float	alpha	= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
								float	scale	= Item.scale*(1-alpha);
								float	radius	= R*scale;

								float	ssa		= radius*radius/dist_sq;
								if (ssa < r_ssaDISCARD) continue;
								u32	vis_id	= Item.vis_ID;
								if (ssa < r_ssaCHEAP)	vis_id=0;

								Item.scale_calculated = scale;			//alpha;
								visible[vis_id][sp.id].push_back	(*siIT);
							}
						}
					}
				}
				break;
			case fcvFully:		// addition
				{
					if (!::Render->occ_visible(S.vis))		continue;
					for (int sp_id=0; sp_id<dm_obj_in_slot; sp_id++)
					{
						SlotPart&			sp	= S.G		[sp_id];
						if (sp.id==0xff)	continue;
						float				R   = objects	[sp.id]->bv_sphere.R;

						SlotItem			**siIT=sp.items.begin(), **siEND=sp.items.end();
						for (; siIT!=siEND; siIT++)
						{
							SlotItem& Item	= *(*siIT);

							float	dist_sq = EYE.distance_to_sqr(Item.P);
							if (dist_sq>fade_limit)	continue;

							float	alpha	= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
							float	scale	= Item.scale*(1-alpha);
							float	radius	= R*scale;

							float	ssa		= radius*radius/dist_sq;
							if (ssa < r_ssaDISCARD) continue;
							u32	vis_id	= Item.vis_ID;
							if (ssa < r_ssaCHEAP)	vis_id=0;

							Item.scale_calculated = scale;			//alpha;
							visible[vis_id][sp.id].push_back	(*siIT);
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
