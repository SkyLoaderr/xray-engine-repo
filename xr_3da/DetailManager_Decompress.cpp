#include "stdafx.h"
#include "DetailManager.h"
#include "cl_intersect.h"

#ifdef _EDITOR
	#include "scene.h"
	#include "sceneobject.h"
#else
	#include "xr_creator.h"
#endif

float		psDetailDensity		= 0.15f;

//--------------------------------------------------- Decompression
IC float	Interpolate			(float* base,		u32 x, u32 y, u32 size)
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

IC bool		InterpolateAndDither(float* alpha255,	u32 x, u32 y, u32 sx, u32 sy, u32 size, int dither[16][16] )
{
	clamp 	(x,0u,size-1);
	clamp 	(y,0u,size-1);
	int		c	= iFloor(Interpolate(alpha255,x,y,size)+.5f);
	clamp   (c,0,255);

	u32	row	= (y+sy) % 16;
	u32	col	= (x+sx) % 16;
	return	c	> dither[col][row];
}

void		CDetailManager::cache_Decompress(Slot* S)
{
	VERIFY				(S);
	Slot&	D			= *S;
	DetailSlot&	DS		= QueryDB(D.sx,D.sz);
	D.type				= stReady;

	// Select polygons
	Fvector		bC,bD;
	D.BB.get_CD			(bC,bD);
	XRC.box_options		(0); // BBOX_TRITEST

#ifdef _EDITOR
	// Select polygons
	SBoxPickInfoVec		pinf;
	Scene.BoxPick		(D.BB,pinf,GetSnapObjects());
	u32	triCount	= pinf.size();
#else
	XRC.box_query		(pCreator->ObjectSpace.GetStaticModel(),bC,bD);
	u32	triCount	= XRC.r_count	();
	CDB::TRI* tris		= pCreator->ObjectSpace.GetStaticTris();
#endif

	if (0==triCount)	return;

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
	u32		d_size		= iCeil	(dm_slot_size/density);
	svector<int,dm_obj_in_slot>		selected;

	CRandom				r_selection	(0x12071980^::Random.randI(32760));
	CRandom				r_jitter	(0x12071980^::Random.randI(32760));
	CRandom				r_yaw		(0x12071980^::Random.randI(32760));
	CRandom				r_scale		(0x12071980^::Random.randI(32760));

	// Prepare to actual-bounds-calculations
	Fbox				Bounds;	
	Bounds.invalidate	();

	// Decompressing itself
	for (u32 z=0; z<=d_size; z++)
	{
		for (u32 x=0; x<=d_size; x++)
		{
			// shift
			u32 shift_x =  r_jitter.randI(16);
			u32 shift_z =  r_jitter.randI(16);

			// Iterpolate and dither palette
			selected.clear();
			if ((DS.items[0].id!=0xff)&& InterpolateAndDither(alpha255[0],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(0);
			if ((DS.items[1].id!=0xff)&& InterpolateAndDither(alpha255[1],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(1);
			if ((DS.items[2].id!=0xff)&& InterpolateAndDither(alpha255[2],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(2);
			if ((DS.items[3].id!=0xff)&& InterpolateAndDither(alpha255[3],x,z,shift_x,shift_z,d_size,dither))	selected.push_back(3);

			// Select 
			if (selected.empty())	continue;
			u32 index;
			if (selected.size()==1)	index = selected[0];
			else					index = selected[r_selection.randI(selected.size())]; 

			CDetail*	Dobj	= objects[DS.items[index].id];
			SlotItem*	ItemP	= poolSI.create();
			SlotItem&	Item	= *ItemP;

			// Position (XZ)
			float		rx = (float(x)/float(d_size))*dm_slot_size + D.BB.min.x;
			float		rz = (float(z)/float(d_size))*dm_slot_size + D.BB.min.z;
			Item.P.set	(rx + r_jitter.randFs(jitter), D.BB.max.y, rz + r_jitter.randFs(jitter));

			// Position (Y)
			float y		= D.BB.min.y-5;
			Fvector	dir; dir.set(0,-1,0);

			float		r_u,r_v,r_range;
			for (u32 tid=0; tid<triCount; tid++)
			{
#ifdef _EDITOR                
				Fvector verts[3];
				SBoxPickInfo& I=pinf[tid];
				for (int k=0; k<(int)I.inf.size(); k++){
					R_ASSERT(I.s_obj);
Device.Statistic.TEST0.Begin	();
					I.s_obj->GetFaceWorld(I.e_mesh,I.inf[k].id,verts);
Device.Statistic.TEST0.End		();
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
			D.G[index].items.push_back(ItemP);
		}
	}

	// Update bounds to more tight and real ones
	D.BB.set	(Bounds);
}
