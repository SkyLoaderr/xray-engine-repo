// DetailManager.cpp: implementation of the CDetailManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "DetailManager.h"
#include "cl_intersect.h"

#ifdef _EDITOR
#	include "SceneClassList.h"
#	include "Scene.h"
#	include "SceneObject.h"
#	include "igame_persistent.h"
#	include "environment.h"
#else
#	include "..\igame_persistent.h"
#	include "..\environment.h"
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

void bwdithermap	(int levels, int magic[16][16])
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

void CDetailManager::SSwingValue::lerp(const SSwingValue& A, const SSwingValue& B, float f)
{
	float fi	= 1.f-f;
	amp1		= fi*A.amp1  + f*B.amp1;
	amp2		= fi*A.amp2  + f*B.amp2;
	rot1		= fi*A.rot1  + f*B.rot1;
	rot2		= fi*A.rot2  + f*B.rot2;
	speed		= fi*A.speed + f*B.speed;
}
//---------------------------------------------------

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

	// swing desc
	// normal
	swing_desc[0].amp1	= .1f;
	swing_desc[0].amp2	= .05f;
	swing_desc[0].rot1	= 30.f;
	swing_desc[0].rot2	= 1.f;
	swing_desc[0].speed	= 2.f;
	// fast
	swing_desc[1].amp1	= 1.f;
	swing_desc[1].amp2	= .5f;
	swing_desc[1].rot1	= .01f;
	swing_desc[1].rot2	= .9f;
	swing_desc[1].speed	= 1.f;
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

extern ECORE_API float r_ssaDISCARD;


void CDetailManager::Render		()
{
#ifndef _EDITOR
	if (0==dtFS)						return;
	if (!psDeviceFlags.is(rsDetails))	return;
#endif
	float factor				= g_pGamePersistent->Environment.wind_strength;
	swing_current.lerp			(swing_desc[0],swing_desc[1],factor);

	float		r_ssaCHEAP		= 16*r_ssaDISCARD;

	Fvector		EYE				= Device.vCameraPosition;
	CFrustum	View			= RImplementation.ViewBase;

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
			Slot*	CS			= cache_Query(_x,_z);
			Slot&	S			= *CS;

			// Transfer visibile and partially visible slot contents
			u32 mask			= 0xff;
			u32 res				= View.testSAABB		(S.vis.sphere.P,S.vis.sphere.R,S.vis.box.data(),mask);
			if (fcvNone==res)							continue;	// invisible-view frustum
#ifdef _EDITOR
			if (!RImplementation.occ_visible(S.vis))	continue;	// invisible-occlusion
#else
			if (!RImplementation.HOM.visible(S.vis))	continue;	// invisible-occlusion
#endif

			// Calc fade factor	(per slot)
			float	dist_sq		= EYE.distance_to_sqr	(S.vis.sphere.P);
			if		(dist_sq>fade_limit)				continue;
			float	alpha		= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
			float	alpha_i		= 1.f - alpha;
			float	dist_sq_rcp	= 1.f / dist_sq;

			// Add to visibility structures
			for (int sp_id=0; sp_id<dm_obj_in_slot; sp_id++)
			{
				SlotPart&			sp	= S.G		[sp_id];
				if (sp.id==DetailSlot::ID_Empty)	continue;
				float				R		= objects	[sp.id]->bv_sphere.R;
				float				Rq_drcp	= R*R*dist_sq_rcp;	// reordered expression for 'ssa' calc

				SlotItem			**siIT=&(*sp.items.begin()), **siEND=&(*sp.items.end());
				for (; siIT!=siEND; siIT++)
				{
					SlotItem& Item			= *(*siIT);

					float   scale			= Item.scale_calculated	= Item.scale*alpha_i;
					float	ssa				= scale*scale*Rq_drcp;
					if (ssa < r_ssaDISCARD) continue;
					u32		vis_id			= 0;
					if (ssa > r_ssaCHEAP)	vis_id = Item.vis_ID;
					visible[vis_id][sp.id].push_back	(*siIT);
				}
			}
		}
	}
	Device.Statistic.RenderDUMP_DT_VIS.End	();

	Device.Statistic.RenderDUMP_DT_Render.Begin	();
	RCache.set_CullMode		(CULL_NONE);
	RCache.set_xform_world	(Fidentity);
	if (UseVS())			hw_Render	();
	else					soft_Render	();
	RCache.set_CullMode		(CULL_CCW);
	Device.Statistic.RenderDUMP_DT_Render.End	();
}
