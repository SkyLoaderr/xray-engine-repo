//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ESceneWallmarkTools.h"
#include "ESceneWallmarkControls.h"
#include "Scene.h"
#include "EditObject.h"
#include "SceneObject.h"
#include "UI_ToolsCustom.h"
#include "scene.h"
#include "UI_Main.h"
#include "D3DUtils.h"

// chunks
#define WM_VERSION  				0x0001
//----------------------------------------------------
#define WM_CHUNK_VERSION			0x0001       
#define WM_CHUNK_FLAGS				0x0002
#define WM_CHUNK_PARAMS				0x0003
#define WM_CHUNK_ITEMS				0x0004
//----------------------------------------------------

ESceneWallmarkTools::ESceneWallmarkTools():ESceneCustomMTools(OBJCLASS_WM)
{
    m_MarkSize		= 1.f;
    m_MarkRotate	= 0.f;
    m_Flags.assign	(flDrawWallmark);
}

ESceneWallmarkTools::~ESceneWallmarkTools()
{
}

int ESceneWallmarkTools::RaySelect(int flag, float& distance, const Fvector& start, const Fvector& direction, BOOL bDistanceOnly)
{
	if (!m_Flags.is(flDrawWallmark)) return 0;
    
    wallmark* W		= 0;
    for (WMSVecIt slot_it=marks.begin(); slot_it!=marks.end(); slot_it++){
        wm_slot* slot		= *slot_it;	
        for (WMVecIt w_it=slot->items.begin(); w_it!=slot->items.end(); w_it++){
            Fvector 		pt;
            if (Fbox::rpOriginOutside==(*w_it)->bbox.Pick2(start,direction,pt)){
            	float range	= start.distance_to(pt);
                if (range<distance){ 
		            W		= *w_it;
                	distance= range;
                }
            }
        }
    }
    if (W&&!bDistanceOnly){
    	if (flag==-1)	W->flags.invert(wallmark::flSelected);
        W->flags.set	(wallmark::flSelected,flag);
        return 1;
    }
    return 0;
}

int ESceneWallmarkTools::FrustumSelect(int flag, const CFrustum& frustum)
{
	if (!m_Flags.is(flDrawWallmark)) return 0;

    int count = 0;
    for (WMSVecIt p_it=marks.begin(); p_it!=marks.end(); p_it++){
        for (WMVecIt m_it=(*p_it)->items.begin(); m_it!=(*p_it)->items.end(); m_it++){
        	wallmark* W = *m_it;
	        u32 mask 	= 0xffff;
            if (frustum.testSAABB(W->bounds.P,W->bounds.R,W->bbox.data(),mask)){
                if (-1==flag)	W->flags.invert	(wallmark::flSelected);
                else			W->flags.set	(wallmark::flSelected,flag);
                count++;
            }
        }
    }
    UI->RedrawScene		();
    return count;
}

void ESceneWallmarkTools::SelectObjects(bool flag)
{
	if (!m_Flags.is(flDrawWallmark)) return;
    for (WMSVecIt p_it=marks.begin(); p_it!=marks.end(); p_it++){
        for (WMVecIt m_it=(*p_it)->items.begin(); m_it!=(*p_it)->items.end(); m_it++)
            (*m_it)->flags.set(wallmark::flSelected,flag);
    }
    UI->RedrawScene		();
}

void ESceneWallmarkTools::InvertSelection()
{
	if (!m_Flags.is(flDrawWallmark)) return;
    for (WMSVecIt p_it=marks.begin(); p_it!=marks.end(); p_it++){
        for (WMVecIt m_it=(*p_it)->items.begin(); m_it!=(*p_it)->items.end(); m_it++)
            (*m_it)->flags.invert(wallmark::flSelected);
    }
    UI->RedrawScene		();
}

void ESceneWallmarkTools::RemoveSelection()
{
	if (!m_Flags.is(flDrawWallmark)) return;
    for (WMSVecIt p_it=marks.begin(); p_it!=marks.end(); p_it++){
        for (WMVecIt m_it=(*p_it)->items.begin(); m_it!=(*p_it)->items.end(); )
        	if ((*m_it)->flags.is(wallmark::flSelected)){
            	wm_destroy	(*m_it);
                *m_it		= (*p_it)->items.back();
                (*p_it)->items.pop_back();
            }else{
            	m_it++;
            }
    }
    UI->RedrawScene		();
}

int ESceneWallmarkTools::SelectionCount(bool testflag)
{
	if (!m_Flags.is(flDrawWallmark)) return 0;
	int count = 0;
    for (WMSVecIt p_it=marks.begin(); p_it!=marks.end(); p_it++)
        for (WMVecIt m_it=(*p_it)->items.begin(); m_it!=(*p_it)->items.end(); m_it++)
            if ((*m_it)->flags.is(wallmark::flSelected)) count++;
	return count;
}

void ESceneWallmarkTools::Clear(bool bOnlyNodes)
{
	{
		for (WMSVecIt p_it=marks.begin(); p_it!=marks.end(); p_it++){
			for (WMVecIt m_it=(*p_it)->items.begin(); m_it!=(*p_it)->items.end(); m_it++)
				wm_destroy	(*m_it);
			xr_delete(*p_it);
		}
		marks.clear	();
	}
	{
		for (u32 it=0; it<pool.size(); it++)
			xr_delete	(pool[it]);
		pool.clear	();
	}
}

bool ESceneWallmarkTools::Valid		(){return !marks.empty();}
bool ESceneWallmarkTools::IsNeedSave(){return marks.size();}
void ESceneWallmarkTools::OnFrame	(){}

extern ECORE_API float r_ssaDISCARD;
const int	MAX_TRIS		= 512;

void ESceneWallmarkTools::OnRender(int priority, bool strictB2F)
{
	if (!m_Flags.is(flDrawWallmark)) return;
    if (1==priority){
        if (marks.empty())			return;
    	if (false==strictB2F){
            for (WMSVecIt slot_it=marks.begin(); slot_it!=marks.end(); slot_it++){
                wm_slot* slot		= *slot_it;	
                for (WMVecIt w_it=slot->items.begin(); w_it!=slot->items.end(); w_it++){
                    wallmark* W		= *w_it;
                    if (W->flags.is(wallmark::flSelected))
                        if (RImplementation.ViewBase.testSphere_dirty(W->bounds.P,W->bounds.R))
                            DU.DrawSelectionBox(W->bbox);
                }
            }
        }else if (true==strictB2F){
            // Projection and xform
            float _43					= Device.mProjection._43;
            Device.mProjection._43 		-= 0.01f; 
            RCache.set_xform_world		(Fidentity);
            RCache.set_xform_project	(Device.mProjection);

            float	ssaCLIP				= r_ssaDISCARD/4;

            for (WMSVecIt slot_it=marks.begin(); slot_it!=marks.end(); slot_it++){
                u32			w_offset= 0;
                FVF::LIT*	w_verts = (FVF::LIT*)	RCache.Vertex.Lock	(MAX_TRIS*3,hGeom->vb_stride,w_offset);
                FVF::LIT*	w_start = w_verts;
                wm_slot* slot		= *slot_it;	
                for (WMVecIt w_it=slot->items.begin(); w_it!=slot->items.end(); w_it++){
                    wallmark* W		= *w_it;
                    if (RImplementation.ViewBase.testSphere_dirty(W->bounds.P,W->bounds.R)){
                        float dst	= Device.vCameraPosition.distance_to_sqr(W->bounds.P);
                        float ssa	= W->bounds.R * W->bounds.R / dst;
                        if (ssa>=ssaCLIP){
                            u32 w_count		= u32(w_verts-w_start);
                            if ((w_count+W->verts.size())>=(MAX_TRIS*3)){
                                if (w_count){
                                    // Flush stream
                                    RCache.Vertex.Unlock	(w_count,hGeom->vb_stride);
                                    RCache.set_Shader		(slot->shader);
                                    RCache.set_Geometry		(hGeom);
                                    RCache.Render			(D3DPT_TRIANGLELIST,w_offset,w_count/3);

                                    // Restart (re-lock/re-calc)
                                    w_verts		= (FVF::LIT*)	RCache.Vertex.Lock	(MAX_TRIS*3,hGeom->vb_stride,w_offset);
                                    w_start		= w_verts;
                                }
                            }

                            wm_render	(W,w_verts);
                        }
                    }
                }
                // Flush stream
                u32 w_count				= u32(w_verts-w_start);
                RCache.Vertex.Unlock	(w_count,hGeom->vb_stride);
                if (w_count)			
                {
                    RCache.set_Shader	(slot->shader);
                    RCache.set_Geometry	(hGeom);
                    RCache.Render		(D3DPT_TRIANGLELIST,w_offset,w_count/3);
                }
            }

            // Projection
            Device.mProjection._43		= _43;
            RCache.set_xform_project	(Device.mProjection);
        }
    }
}

bool ESceneWallmarkTools::Load(IReader& F)
{
	u16 version = 0;

    R_ASSERT(F.r_chunk(WM_CHUNK_VERSION,&version));
    if( version!=WM_VERSION ){
        ELog.DlgMsg( mtError, "WM: Unsupported version.");
        return false;
    }

    R_ASSERT(F.find_chunk(WM_CHUNK_FLAGS));
    F.r				(&m_Flags,sizeof(m_Flags));

	R_ASSERT(F.find_chunk(WM_CHUNK_PARAMS));
    m_MarkSize		= F.r_float	();
    m_MarkRotate	= F.r_float	();
    F.r_stringZ		(m_TexName);

	R_ASSERT(F.find_chunk(WM_CHUNK_ITEMS));
    u32 slot_cnt		= F.r_u32();
    for (u32 slot_idx=0; slot_idx<slot_cnt; slot_idx++){
        u32 item_count	= F.r_u32();	
        if (item_count){
        	shared_str		tex_name;
	        F.r_stringZ	(tex_name);
        	wm_slot* slot	= AppendSlot(tex_name);
            slot->items.resize(item_count);
            for (WMVecIt w_it=slot->items.begin(); w_it!=slot->items.end(); w_it++){
            	*w_it		= wm_allocate();
                wallmark* W	= *w_it;
                F.r		(&W->flags,sizeof(W->flags));
                F.r		(&W->bbox,sizeof(W->bbox));
                F.r		(&W->bounds,sizeof(W->bounds));
                W->verts.resize(F.r_u32());
                F.r		(&*W->verts.begin(),sizeof(FVF::LIT)*W->verts.size());
            }
        }
    }

    return true;
}

void ESceneWallmarkTools::Save(IWriter& F)
{
	F.open_chunk	(WM_CHUNK_VERSION);
    F.w_u16			(WM_VERSION);
	F.close_chunk	();

	F.open_chunk	(WM_CHUNK_FLAGS);
    F.w				(&m_Flags,sizeof(m_Flags));
	F.close_chunk	();

	F.open_chunk	(WM_CHUNK_PARAMS);
    F.w_float		(m_MarkSize);
    F.w_float		(m_MarkRotate);
    F.w_stringZ		(m_TexName);
	F.close_chunk	();

	F.open_chunk	(WM_CHUNK_ITEMS);
    F.w_u32			(marks.size());
    for (WMSVecIt slot_it=marks.begin(); slot_it!=marks.end(); slot_it++){
        wm_slot* slot= *slot_it;	
        F.w_u32		(slot->items.size());
        if (slot->items.size()){
            F.w_stringZ	(slot->tex_name);
            for (WMVecIt w_it=slot->items.begin(); w_it!=slot->items.end(); w_it++){
                wallmark* W	= *w_it;
                F.w		(&W->flags,sizeof(W->flags));
                F.w		(&W->bbox,sizeof(W->bbox));
                F.w		(&W->bounds,sizeof(W->bounds));
                F.w_u32	(W->verts.size());
                F.w		(&*W->verts.begin(),sizeof(FVF::LIT)*W->verts.size());
            }
        }
    }
	F.close_chunk	();
}

bool ESceneWallmarkTools::LoadSelection(IReader& F)
{
	Clear();
	return Load(F);
}

void ESceneWallmarkTools::SaveSelection(IWriter& F)
{
	Save(F);
}

bool ESceneWallmarkTools::Export(LPCSTR path)
{
    AnsiString fn		= AnsiString(path)+"level.wallmarks";
	IWriter*	F		= FS.w_open(fn.c_str());
                             
    F->open_chunk		(0);
    F->w_u32			(marks.size());
    for (WMSVecIt slot_it=marks.begin(); slot_it!=marks.end(); slot_it++){
        wm_slot* slot= *slot_it;	
        F->w_u32		(slot->items.size());
        if (slot->items.size()){
            F->w_stringZ(slot->tex_name);
            for (WMVecIt w_it=slot->items.begin(); w_it!=slot->items.end(); w_it++){
                wallmark* W	= *w_it;
                F->w	(&W->bounds,sizeof(W->bounds));
                F->w_u32(W->verts.size());
                F->w	(&*W->verts.begin(),sizeof(FVF::LIT)*W->verts.size());
            }
        }
    }
    F->close_chunk	();

    FS.w_close		(F);
    
	return true;
}                

void ESceneWallmarkTools::OnDeviceCreate()
{
	hGeom.create	(FVF::F_LIT, RCache.Vertex.Buffer(), NULL);
}

void ESceneWallmarkTools::OnDeviceDestroy()
{
	hGeom.destroy	();
}

void ESceneWallmarkTools::OnSynchronize()
{
}

// allocate
ESceneWallmarkTools::wallmark*	ESceneWallmarkTools::wm_allocate		()
{
	wallmark*			W = 0;
	if (pool.empty())	W = xr_new<wallmark> ();
	else				{ W = pool.back(); pool.pop_back(); }

	W->verts.clear		();
	return W;
}
// destroy
void		ESceneWallmarkTools::wm_destroy		(wallmark*	W	)
{
	pool.push_back		(W);
}
// render
void		ESceneWallmarkTools::wm_render			(wallmark*	W, FVF::LIT* &V)
{
	float		a		= 0.f;
	int			aC		= iFloor	( a * 255.f);	clamp	(aC,0,255);
	u32			C		= color_rgba(128,128,128,aC);

	FVF::LIT*	S		= &*W->verts.begin	();
	FVF::LIT*	E		= &*W->verts.end	();
	for (; S!=E; S++, V++)
	{
		V->p.set		(S->p);
		V->color		= C;
		V->t.set		(S->t);
	}
}

struct SWMSlotFindPredicate {
	shared_str				tex_name;
						SWMSlotFindPredicate(shared_str tx):tex_name(tx){}
	bool				operator()			(ESceneWallmarkTools::wm_slot* slot) const
	{
		return			(slot->tex_name == tex_name);
	}
};
ESceneWallmarkTools::wm_slot* ESceneWallmarkTools::FindSlot	(shared_str tex_name)
{
	WMSVecIt it					= std::find_if(marks.begin(),marks.end(),SWMSlotFindPredicate(tex_name));
	return						(it!=marks.end())?*it:0;
}
ESceneWallmarkTools::wm_slot* ESceneWallmarkTools::AppendSlot(shared_str tex_name)
{
	marks.push_back				(xr_new<wm_slot>(tex_name));
	return marks.back			();
}

void ESceneWallmarkTools::RecurseTri(u32 t, Fmatrix &mView, wallmark &W)
{
	CDB::TRI*	T			= sml_collector.getT()+t;
	if (T->dummy)			return;
	T->dummy				= 0xffffffff;
	
	// Some vars
	u32*		v_ids		= T->verts;
	Fvector*	v_data		= sml_collector.getV();
	sml_poly_src.clear		();
	sml_poly_src.push_back	(v_data[v_ids[0]]);
	sml_poly_src.push_back	(v_data[v_ids[1]]);
	sml_poly_src.push_back	(v_data[v_ids[2]]);
	sml_poly_dest.clear		();
	
	sPoly* P = sml_clipper.ClipPoly	(sml_poly_src, sml_poly_dest);
	
	if (P) {
		// Create vertices and triangulate poly (tri-fan style triangulation)
		FVF::LIT			V0,V1,V2;
		Fvector				UV;

		mView.transform_tiny(UV, (*P)[0]);
		V0.set				((*P)[0],0,(1+UV.x)*.5f,(1-UV.y)*.5f);
		mView.transform_tiny(UV, (*P)[1]);
		V1.set				((*P)[1],0,(1+UV.x)*.5f,(1-UV.y)*.5f);

		for (u32 i=2; i<P->size(); i++)
		{
			mView.transform_tiny(UV, (*P)[i]);
			V2.set				((*P)[i],0,(1+UV.x)*.5f,(1-UV.y)*.5f);
			W.verts.push_back	(V0);
			W.verts.push_back	(V1);
			W.verts.push_back	(V2);
			V1					= V2;
		}
		
		// recurse
		for (i=0; i<3; i++)
		{
			u32 adj					= sml_adjacency[3*t+i];
			if (0xffffffff==adj)	continue;
			CDB::TRI*	SML			= sml_collector.getT() + adj;
			v_ids					= SML->verts;

			Fvector test_normal;
			test_normal.mknormal	(v_data[v_ids[0]],v_data[v_ids[1]],v_data[v_ids[2]]);
			float cosa				= test_normal.dotproduct(sml_normal);
			if (cosa<EPS)			continue;
			RecurseTri				(adj,mView,W);
		}
	}
}

void ESceneWallmarkTools::BuildMatrix	(Fmatrix &mView, float invsz, const Fvector& from)
{
	// build projection
	Fmatrix				mScale;
    Fvector				at,up,right,y;
	at.sub				(from,sml_normal);
	y.set				(0,1,0);
	if (_abs(sml_normal.y)>.99f) y.set(1,0,0);
	right.crossproduct	(y,sml_normal);
	up.crossproduct		(sml_normal,right);
	mView.build_camera	(from,at,up);
	mScale.scale		(invsz,invsz,invsz);
	mView.mulA			(mScale);
}

int	ESceneWallmarkTools::ObjectCount()
{
	int count 			= 0;
    for (WMSVecIt p_it=marks.begin(); p_it!=marks.end(); p_it++)
        count			+= (*p_it)->items.size();
	return count;
}

#define MAX_WALLMARK_COUNT	50

BOOL ESceneWallmarkTools::AddWallmark	(const Fvector& start, const Fvector& dir)
{
	if (ObjectCount()>=MAX_WALLMARK_COUNT){
    	ELog.DlgMsg			(mtError,"Maximum wallmark per level is reached [Max: %d].",MAX_WALLMARK_COUNT);
    	return FALSE;
    }
    
    if (0==m_TexName.size()){
    	ELog.DlgMsg			(mtError,"Select texture before add wallmark.");
    	return 				FALSE;
    }
    // pick contact poly
    Fvector 				contact_pt;
    float dist				= flt_max;
    ObjectList* snap_list	= Scene->GetSnapList(false);
    if (!snap_list){
    	ELog.DlgMsg			(mtError,"Fill and activate snap list.");
    	return 				FALSE;
    }
    // pick contact poly
    SPickQuery				PQ;
    sml_collector.clear		();
    if (Scene->RayQuery(PQ,start,dir,dist,CDB::OPT_ONLYNEAREST|CDB::OPT_CULL,snap_list)){ 
        contact_pt.mad		(PQ.m_Start,PQ.m_Direction,PQ.r_begin()->range); 
        sml_normal.mknormal	(PQ.r_begin()->verts[0],PQ.r_begin()->verts[1],PQ.r_begin()->verts[2]);
        sml_collector.add_face_packed_D	(PQ.r_begin()->verts[0],PQ.r_begin()->verts[1],PQ.r_begin()->verts[2],0);
    }

    // box pick poly
    Fbox					bbox;
    bbox.set				(contact_pt,contact_pt);
    bbox.grow				(m_MarkSize*2);
    SPickQuery				BQ;
    if (Scene->BoxQuery(BQ,bbox,CDB::OPT_FULL_TEST,snap_list)){ 
    	for (u32 k=0; k<(u32)BQ.r_count(); k++){
        	SPickQuery::SResult* R = BQ.r_begin()+k;
			Fvector test_normal;
			test_normal.mknormal	(R->verts[0],R->verts[1],R->verts[2]);
			float cosa				= test_normal.dotproduct(sml_normal);
			if (cosa<0.1)			continue;
	        sml_collector.add_face_packed_D	(R->verts[0],R->verts[1],R->verts[2],0);
        }
    }

    // remove duplicate poly
	sml_collector.remove_duplicate_T	();
    // calculate adjacency
    sml_collector.calc_adjacency		(sml_adjacency);
    
	// build 3D ortho-frustum
	Fmatrix				mView,mRot;
	BuildMatrix			(mView,1/m_MarkSize,contact_pt);
	mRot.rotateZ		(m_MarkRotate);
	mView.mulA_43		(mRot);
	sml_clipper.CreateFromMatrix (mView,FRUSTUM_P_LRTB);

	// create wallmark
	wallmark* W			= wm_allocate();

    RecurseTri			(0,mView,*W);

	// calc sphere
	if (W->verts.size()<3) { wm_destroy(W); return FALSE; }
	else {
		W->bbox.invalidate();
		FVF::LIT* I=&*W->verts.begin	();
		FVF::LIT* E=&*W->verts.end		();
		for (; I!=E; I++) W->bbox.modify(I->p);
		W->bbox.getsphere				(W->bounds.P,W->bounds.R);
        W->flags.assign					(wallmark::flSelected);
        W->bbox.grow					(EPS_L);
	}

	// search if similar wallmark exists
	wm_slot* slot	= FindSlot(*m_TexName);
	if (slot){
		WMVecIt		it	= slot->items.begin	();
		WMVecIt		end	= slot->items.end	();
		for (; it!=end; it++)
		{
			wallmark* wm	= *it;
			if (wm->bounds.P.similar(W->bounds.P,0.02f)){ // replace
				wm_destroy	(wm);
				*it			= W;
				return TRUE;
			}
		}
	}else{
		slot		= AppendSlot(*m_TexName);
	}

	// no similar - register _new_
	slot->items.push_back(W);
    return TRUE;
}

void ESceneWallmarkTools::CreateControls()
{
	inherited::CreateControls();
	// node tools
    AddControl(xr_new<TUI_ControlWallmarkAdd>		(0,		etaAdd, 	this));
}
//----------------------------------------------------
 
void ESceneWallmarkTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

void ESceneWallmarkTools::FillProp(LPCSTR pref, PropItemVec& items)
{
    PHelper().CreateFlag32		(items, PrepareKey(pref,"Common\\Draw Wallmarks"),	&m_Flags, 		flDrawWallmark);
    PHelper().CreateFloat	 	(items, PrepareKey(pref,"Common\\Default Size"),		&m_MarkSize, 	0.01f, 10.f);
    PHelper().CreateAngle	 	(items, PrepareKey(pref,"Common\\Default Rotate"),	&m_MarkRotate);
    PHelper().CreateChoose		(items, PrepareKey(pref,"Common\\Default Texture"),	&m_TexName, 	smTexture);
}
//----------------------------------------------------

