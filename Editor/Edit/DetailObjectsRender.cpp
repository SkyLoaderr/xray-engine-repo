//----------------------------------------------------
// file: DetailObjects.h
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "DetailObjects.h"
#include "UI_Main.h"
#include "Scene.h"
#include "cl_intersect.h"
#include "D3DUtils.h"
#include "bottombar.h"

const DWORD	vs_size				= 3000;
const float slot_size			= DETAIL_SLOT_SIZE;
const float slot_radius			= DETAIL_SLOT_RADIUS;

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

void CDetailManager::InvalidateCache(){
	for (DWORD s=0; s<m_Cache.size(); s++)    	m_Cache[s].type 	= stInvalid;
}

void CDetailManager::InitRender(){
	ZeroMemory(&m_Visible,sizeof(m_Visible));	m_Visible.resize	(dm_max_objects);
	ZeroMemory(&m_Cache,sizeof(m_Cache));		m_Cache.resize		(dm_cache_size);
    InvalidateCache();

	// Make dither matrix
	bwdithermap		(2,m_Dither);
}
//------------------------------------------------------------------------------

void CDetailManager::Render(ERenderPriority flag){
	if (m_Slots.size()){
    	switch (flag){
		case rpNormal:{
            UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);
            UI->Device.Shader.Set(UI->Device.m_WireShader);

			Fvector			c;
            Fbox			bbox;
            DWORD			inactive = 0xff808080;
            DWORD			selected = 0xffffffff;
            for (DWORD z=0; z<m_Header.size_z; z++){
                c.z			= fromSlotZ(z);
                for (DWORD x=0; x<m_Header.size_x; x++){
                    bool bSel = m_Selected[z*m_Header.size_x+x];
                    if (bSel||fraBottomBar->miDrawDOSlotBoxes->Checked){
                        DSIt slot	= m_Slots.begin()+z*m_Header.size_x+x;
                        c.x			= fromSlotX(x);
                        c.y			= (slot->y_max+slot->y_min)*0.5f;
                        bbox.min.set(c.x-DETAIL_SLOT_SIZE_2, slot->y_min, c.z-DETAIL_SLOT_SIZE_2);
                        bbox.max.set(c.x+DETAIL_SLOT_SIZE_2, slot->y_max, c.z+DETAIL_SLOT_SIZE_2);
                        bbox.shrink	(0.05f);

                        if (UI->Device.m_Frustum.testSphere(c,DETAIL_SLOT_SIZE_2))
	                        DU::DrawSelectionBox(bbox,bSel?&selected:&inactive);
                    }
                }
            }
        }break;
		case rpAlphaNormal:{
        	if (fraBottomBar->miDODrawObjects->Checked)
	        	RenderObjects(UI->Device.m_Camera.GetPosition());
        }break;
		case rpAlphaLast:{
        	if (fraBottomBar->miDrawDOBaseTexture->Checked)
	        	RenderTexture(1.0f);
        }break;
        }
    }
}

void CDetailManager::RenderTexture(float alpha){
	FVF::LIT V[4];

    DWORD color = RGBA_MAKE(255,255,255,BYTE(alpha*255));

	V[0].set(m_BBox.min.x,m_BBox.max.y,m_BBox.min.z,color,0,1);
	V[1].set(m_BBox.min.x,m_BBox.max.y,m_BBox.max.z,color,0,0);
	V[2].set(m_BBox.max.x,m_BBox.max.y,m_BBox.max.z,color,1,0);
	V[3].set(m_BBox.max.x,m_BBox.max.y,m_BBox.min.z,color,1,1);

	UI->Device.Shader.Set(m_pBaseShader);
    UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);
    UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
    UI->Device.DP(D3DPT_TRIANGLEFAN,FVF::F_LIT, V, 4);
    UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);
}

//------------------------------------------------------------------------------
float ssaLIMIT = 3;

void CDetailManager::RenderObjects(const Fvector& EYE)
{
	int s_x	= iFloor			(EYE.x/DETAIL_SLOT_SIZE+0.5f);
	int s_z	= iFloor			(EYE.z/DETAIL_SLOT_SIZE+0.5f);

	UpdateCache					(5);

	float fade_limit			= 14.5f;fade_limit=fade_limit*fade_limit;
	float fade_start			= 1.f;	fade_start=fade_start*fade_start;
	float fade_range			= fade_limit-fade_start;

	// Collect objects for rendering
	for (int _z=s_z-dm_size; _z<=(s_z+dm_size); _z++){
		for (int _x=s_x-dm_size; _x<=(s_x+dm_size); _x++){
			// Query for slot
			Slot&	S		= Query(_x,_z);
			S.dwFrameUsed	= UI->Device.m_Statistic.dwTotalFrame;

			// Transfer visibile and partially visible slot contents
			BYTE mask		= 0xff;
			switch (UI->Device.m_Frustum.testAABB(S.BB.min,S.BB.max,mask))
			{
			case fcvNone:		// nothing to do
				break;
			case fcvPartial:	// addition with TEST
				{
					for (int sp_id=0; sp_id<dm_obj_in_slot; sp_id++)
					{
						SlotPart&			sp	= S.G		[sp_id];
						if (sp.id==0xff)	continue;
						vector<SlotItem*>&	vis = m_Visible	[sp.id];
						float				R   = m_Objects	[sp.id]->m_fRadius;

						SlotItem			*siIT=sp.items.begin(), *siEND=sp.items.end();
						for (; siIT!=siEND; siIT++)
						{
							SlotItem& Item	= *siIT;

							float	dist_sq = EYE.distance_to_sqr(Item.P);
							if (dist_sq>fade_limit)	continue;

							if (UI->Device.m_Frustum.testSphere(siIT->P,R*Item.scale))
							{
								float	alpha	= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
								float	scale	= Item.scale*(1-alpha);
								float	radius	= R*scale;

								if (UI->Device.GetRenderArea()*radius*radius/dist_sq < ssaLIMIT) continue;

								Item.scale_calculated = scale; //alpha;
								vis.push_back	(siIT);
							}
						}
					}
				}
				break;
			case fcvFully:		// addition */
				{
					for (int sp_id=0; sp_id<3; sp_id++)
					{
						SlotPart&			sp	= S.G		[sp_id];
						if (sp.id==0xff)	continue;
						vector<SlotItem*>&	vis = m_Visible	[sp.id];
						float				R   = m_Objects	[sp.id]->m_fRadius;

						SlotItem			*siIT=sp.items.begin(), *siEND=sp.items.end();
						for (; siIT!=siEND; siIT++)
						{
							SlotItem& Item	= *siIT;

							float	dist_sq = EYE.distance_to_sqr(Item.P);
							if (dist_sq>fade_limit)	continue;

							float	alpha	= (dist_sq<fade_start)?0.f:(dist_sq-fade_start)/fade_range;
							float	scale	= Item.scale*(1-alpha);
							float	radius	= R*scale;

							if (UI->Device.GetRenderArea()*radius*radius/dist_sq < ssaLIMIT) continue;

							Item.scale_calculated = scale; //alpha;
							vis.push_back	(siIT);
						}
					}
				}
				break;
			}
		}
	}

	UI->Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);

	// Render itself
	float	fPhaseRange	= PI/16;
	float	fPhaseX		= sinf(UI->Device.m_fTimeGlobal*0.1f)	*fPhaseRange;
	float	fPhaseZ		= sinf(UI->Device.m_fTimeGlobal*0.11f)	*fPhaseRange;

	for (DWORD O=0; O<dm_max_objects; O++){
		vector<SlotItem*>&	vis = m_Visible	[O];
		if (vis.empty())	continue;

		CDetail&	Object		= *m_Objects[O];
        UI->Device.Shader.Set	(Object.m_pShader);
		Fmatrix		mXform,mRotXZ;
		for (DWORD item=0; item<vis.size(); item++){
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
            // render objects
			UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,mXform);
			UI->Device.DIP(D3DPT_TRIANGLELIST,F_DOV,Object.m_Vertices.begin(),Object.m_Vertices.size(),Object.m_Indices.begin(),Object.m_Indices.size());
		}

		// Clean up
		vis.clear	();
	}
	UI->Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
}

CDetailManager::Slot&	CDetailManager::Query	(int sx, int sz)
{
	// Search cache
	DWORD oldest	= 0;
	for (DWORD I=0; I<m_Cache.size(); I++)
	{
		Slot&	S = m_Cache[I];
		if ((S.sx==sx)&&(S.sz==sz)&&(S.type!=stInvalid))	return S;
		else {
			if (S.dwFrameUsed < m_Cache[oldest].dwFrameUsed)	oldest = I;
		}
	}

	// Cache MISS or cache not filled at all
	if (m_Cache.size()<dm_cache_size)	{
		// Create new entry
		m_Cache.push_back(Slot());
		Decompress(sx,sz,m_Cache.back());
		return m_Cache.back();
	} else {
		// We should discard oldest cache entry on LRU basis
		Slot& S = m_Cache[oldest];
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
IC bool		InterpolateAndDither(float* alpha255,	DWORD x, DWORD y, DWORD size, int dither[16][16] )
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
	for (DWORD entry=0; limit && (entry<m_Cache.size()); entry++)
	{
		if (m_Cache[entry].type != stPending)	continue;

		// Gain access to data
		Slot&		D	= m_Cache[entry];
		DetailSlot&	DS	= QueryDB(D.sx,D.sz);
		D.type			= stReady;

		// Select polygons
		XRC.BBoxMode		(BBOX_TRITEST); // обязательно иначе точки установлены неверно
		SBoxPickInfoVec		pinf;
		Scene->BoxPick		(D.BB,pinf,&m_SnapObjects);
		DWORD	triCount	= pinf.size();
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
		float		density		= 0.1f;
		float		jitter		= density/2;
		DWORD		d_size		= iCeil	(slot_size/density);
		svector<int,dm_obj_in_slot>		selected;

		CRandom				r_selection	(0x12071980);
		CRandom				r_jitter	(0x12071980);
		CRandom				r_yaw		(0x12071980);
		CRandom				r_scale		(0x12071980);

		// Decompressing itself
		for (DWORD z=0; z<=d_size; z++){
			for (DWORD x=0; x<=d_size; x++){
				// shift mask
                int shift_x = r_jitter.randI(16);
                int shift_z = r_jitter.randI(16);

				// Iterpolate and dither palette
				selected.clear();
				if ((DS.items[0].id!=0xff)&& InterpolateAndDither(alpha255[0],x+shift_x,z+shift_z,d_size,m_Dither))	selected.push_back(0);
				if ((DS.items[1].id!=0xff)&& InterpolateAndDither(alpha255[1],x+shift_x,z+shift_z,d_size,m_Dither))	selected.push_back(1);
				if ((DS.items[2].id!=0xff)&& InterpolateAndDither(alpha255[2],x+shift_x,z+shift_z,d_size,m_Dither))	selected.push_back(2);
				if ((DS.items[3].id!=0xff)&& InterpolateAndDither(alpha255[3],x+shift_x,z+shift_z,d_size,m_Dither))	selected.push_back(3);

				// Select
				if (selected.empty())	continue;
				DWORD index;
				if (selected.size()==1)	index = selected[0];
				else					index = selected[r_selection.randI(selected.size())];

				BYTE ID 	= DS.items[index].id; // current object

				SlotItem	Item;

				// Position (XZ)
				float		rx = (float(x)/float(d_size))*slot_size + D.BB.min.x;
				float		rz = (float(z)/float(d_size))*slot_size + D.BB.min.z;
				Item.P.set	(rx+r_jitter.randFs(jitter), D.BB.max.y, rz+r_jitter.randFs(jitter));

				// Position (Y)
				float y		= D.BB.min.y-1;
				Fvector	dir; dir.set(0,-1,0);

				float		r_u,r_v,r_range;
				for (DWORD tid=0; tid<triCount; tid++)
				{
                	Fvector* verts = pinf[tid].bp_inf.p;
					if (RAPID::TestRayTri(Item.P,dir,verts,r_u,r_v,r_range,TRUE))
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
				Item.scale	= r_scale.randF		(m_Objects[ID]->m_fMinScale,m_Objects[ID]->m_fMaxScale);
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
				DWORD c_dw		= iFloor(Interpolate(gray255,x,z,d_size)+.5f);
				Item.C			= D3DCOLOR_RGBA		(c_dw,c_dw,c_dw,0);

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

    int db_x = sx + m_Header.offs_x;
    int db_z = sz + m_Header.offs_z;
    if ((db_x>=0)&&(db_x<m_Header.size_x)&&(db_z>=0)&&(db_z<m_Header.size_z)){
	    DS 					= GetSlot(db_x,db_z);
    }else{
    	DS.y_min			= 0;
        DS.y_max			= EPS;

    	DS.items[0].id		= 0xff;
    	DS.items[1].id		= 0xff;
    	DS.items[2].id		= 0xff;
        DS.items[3].id		= 0xff;
    }

	DS.color				= 0xffff;

	return DS;
}

