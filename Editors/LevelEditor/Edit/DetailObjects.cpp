//----------------------------------------------------
// file: DetailObjects.h
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "DetailObjects.h"
#include "EditMesh.h"
#include "EditObject.h"
#include "Texture.h"
#include "Scene.h"
#include "SceneObject.h"
#include "EditorPref.h"
#include "leftbar.h"
#include "cl_intersect.h"
#include "Library.h"
#include "ui_main.h"

#include "Library.h"
#include "DetailFormat.h"
#include "Texture.h"
#include "bottombar.h"
#include "ImageManager.h"
#include "d3dutils.h"
#include "render.h"
#include "PropertiesListHelper.h"

static Fvector down_vec	={0.f,-1.f,0.f};
static Fvector left_vec	={-1.f,0.f,0.f};
static Fvector right_vec={1.f,0.f,0.f};
static Fvector fwd_vec	={0.f,0.f,1.f};
static Fvector back_vec	={0.f,0.f,-1.f};

static CRandom DetailRandom(0x26111975);

const u32	vs_size				= 3000;
//------------------------------------------------------------------------------

#define DETMGR_CHUNK_VERSION		0x1000
#define DETMGR_CHUNK_HEADER 		0x0000
#define DETMGR_CHUNK_OBJECTS 		0x0001
#define DETMGR_CHUNK_SLOTS			0x0002
#define DETMGR_CHUNK_BBOX			0x1001
#define DETMGR_CHUNK_BASE_TEXTURE	0x1002
#define DETMGR_CHUNK_COLOR_INDEX 	0x1003
#define DETMGR_CHUNK_SNAP_OBJECTS 	0x1004
#define DETMGR_CHUNK_DENSITY	 	0x1005

#define DETMGR_VERSION 				0x0002
//------------------------------------------------------------------------------
EDetailManager::EDetailManager()
{
	dtSlots				= 0;
    ZeroMemory			(&dtH,sizeof(dtH));
    m_Selected.clear	();
    InitRender			();
	Device.seqDevCreate.Add	(this,REG_PRIORITY_LOW);
	Device.seqDevDestroy.Add(this,REG_PRIORITY_NORMAL);
}

EDetailManager::~EDetailManager(){
	Device.seqDevCreate.Remove(this);
	Device.seqDevDestroy.Remove(this);
	Clear	();
    Unload	();
}
//------------------------------------------------------------------------------

void EDetailManager::ClearColorIndices()
{
    RemoveObjects		();
    m_ColorIndices.clear();
}
void EDetailManager::ClearSlots()
{
    ZeroMemory			(&dtH,sizeof(DetailHeader));
    xr_free				(dtSlots);
	m_Selected.clear	();
    InvalidateCache		();
}
void EDetailManager::ClearBase()
{
    m_Base.Clear		();
    m_SnapObjects.clear	();
}
void EDetailManager::Clear(bool bSpecific)
{
	ClearBase			();
	ClearColorIndices	();
    ClearSlots			();
}
//------------------------------------------------------------------------------

void EDetailManager::InvalidateCache()
{
	// resize visible
	visible[0].resize	(objects.size());	// dump(visible[0]);
	visible[1].resize	(objects.size());	// dump(visible[1]);
	visible[2].resize	(objects.size());	// dump(visible[2]);
	// Initialize 'vis' and 'cache'
	cache_Initialize	();
}

extern void bwdithermap	(int levels, int magic[16][16] );
void EDetailManager::InitRender()
{
	// inavlidate cache
	InvalidateCache		();
	// Make dither matrix
	bwdithermap		(2,dither);

	soft_Load	();
}
//------------------------------------------------------------------------------

void EDetailManager::OnRender(int priority, bool strictB2F)
{
	if (dtSlots){
    	if (1==priority){
        	if (false==strictB2F){
            	if (fraBottomBar->miDrawDOSlotBoxes->Checked){
                    RCache.set_xform_world(Fidentity);
                    Device.SetShader	(Device.m_WireShader);

                    Fvector			c;
                    Fbox			bbox;
                    u32			inactive = 0xff808080;
                    u32			selected = 0xffffffff;
                    float dist_lim	= 75.f*75.f;
                    for (u32 z=0; z<dtH.size_z; z++){
                        c.z			= fromSlotZ(z);
                        for (u32 x=0; x<dtH.size_x; x++){
                            bool bSel 	= m_Selected[z*dtH.size_x+x];
                            DetailSlot* slot = dtSlots+z*dtH.size_x+x;
                            c.x			= fromSlotX(x);
                            c.y			= (slot->y_max+slot->y_min)*0.5f;
                            float dist = Device.m_Camera.GetPosition().distance_to_sqr(c);
                         	if ((dist<dist_lim)&&::Render->ViewBase.testSphere_dirty(c,DETAIL_SLOT_SIZE_2)){
								bbox.min.set(c.x-DETAIL_SLOT_SIZE_2, slot->y_min, c.z-DETAIL_SLOT_SIZE_2);
                            	bbox.max.set(c.x+DETAIL_SLOT_SIZE_2, slot->y_max, c.z+DETAIL_SLOT_SIZE_2);
                            	bbox.shrink	(0.05f);
								DU.DrawSelectionBox(bbox,bSel?&selected:&inactive);
							}
                        }
                    }
                }
            }else{
				RCache.set_xform_world(Fidentity);
                if (fraBottomBar->miDrawDOBaseTexture->Checked)	m_Base.Render();
                Fvector EYE = Device.m_Camera.GetPosition();
                if (fraBottomBar->miDODrawObjects->Checked) CDetailManager::Render(EYE);
            }
        }
    }
}
//------------------------------------------------------------------------------

void EDetailManager::OnDeviceCreate(){
	// base texture
    m_Base.CreateShader();
	// detail objects
	for (DetailIt it=objects.begin(); it!=objects.end(); it++)
    	(*it)->OnDeviceCreate();
	soft_Load	();
}

void EDetailManager::OnDeviceDestroy(){
	// base texture
    m_Base.DestroyShader();
	// detail objects
	for (DetailIt it=objects.begin(); it!=objects.end(); it++)
    	(*it)->OnDeviceDestroy();
	soft_Unload	();
}


void EDetailManager::OnObjectRemove(CCustomObject* O)
{
	m_SnapObjects.remove(O);
}

void EDetailManager::ExportColorIndices(LPCSTR fname)
{
	CMemoryWriter F;
    SaveColorIndices(F);
    F.save_to(fname);
}

void EDetailManager::ImportColorIndices(LPCSTR fname)
{
	ClearColorIndices	();
	IReader* F=FS.r_open(fname); R_ASSERT(F);
	LoadColorIndices	(*F);
	FS.r_close			(F);
}

void EDetailManager::SaveColorIndices(IWriter& F)
{
	// objects
	F.open_chunk		(DETMGR_CHUNK_OBJECTS);
    for (DetailIt it=objects.begin(); it!=objects.end(); it++){
		F.open_chunk	(it-objects.begin());
        (*it)->Save		(F);
	    F.close_chunk	();
    }
    F.close_chunk		();
    // color index map
	F.open_chunk		(DETMGR_CHUNK_COLOR_INDEX);
    F.w_u8				(m_ColorIndices.size());
    ColorIndexPairIt S 	= m_ColorIndices.begin();
    ColorIndexPairIt E 	= m_ColorIndices.end();
    ColorIndexPairIt i_it= S;
	for(; i_it!=E; i_it++){
		F.w_u32		(i_it->first);
        F.w_u8			(i_it->second.size());
	    for (DetailIt d_it=i_it->second.begin(); d_it!=i_it->second.end(); d_it++)
        	F.w_stringZ	((*d_it)->GetName());
    }
    F.close_chunk		();
}

bool EDetailManager::LoadColorIndices(IReader& F)
{
    // objects
    IReader* OBJ 		= F.open_chunk(DETMGR_CHUNK_OBJECTS);
    if (OBJ){
        IReader* O   	= OBJ->open_chunk(0);
        for (int count=1; O; count++) {
            CDetail* DO	= xr_new<CDetail>();
            if (!DO->Load(*O)) ELog.Msg(mtError,"Can't load detail object.");
			objects.push_back(DO);
            O->close();
            O = OBJ->open_chunk(count);
        }
        OBJ->close();
    }
    // color index map
    R_ASSERT			(F.find_chunk(DETMGR_CHUNK_COLOR_INDEX));
    int cnt				= F.r_u8();
    string256			buf;
    u32 index;
    int ref_cnt;
    for (int k=0; k<cnt; k++){
		index			= F.r_u32();
        ref_cnt			= F.r_u8();
		for (int j=0; j<ref_cnt; j++){
        	F.r_stringZ	(buf);
            CDetail* DO	= FindObjectByName(buf);
            if (DO) 	m_ColorIndices[index].push_back(DO);
        }
    }

    return true;
}

bool EDetailManager::Load(IReader& F){
    string256 buf;
    R_ASSERT			(F.find_chunk(DETMGR_CHUNK_VERSION));
	u32 version		= F.r_u32();

    if (version!=DETMGR_VERSION){
    	ELog.Msg(mtError,"EDetailManager: unsupported version.");
        return false;
    }

	// header
    R_ASSERT			(F.r_chunk(DETMGR_CHUNK_HEADER,&dtH));

    // objects
    LoadColorIndices	(F);

    // slots
    R_ASSERT			(F.find_chunk(DETMGR_CHUNK_SLOTS));
    int cnt 			= F.r_u32();
    dtSlots				= xr_alloc<DetailSlot>(cnt);
    m_Selected.resize	(cnt);
	F.r					(dtSlots,cnt*sizeof(DetailSlot));

    // internal
    // bbox
    R_ASSERT			(F.r_chunk(DETMGR_CHUNK_BBOX,&m_BBox));

	// snap objects
    if (F.find_chunk(DETMGR_CHUNK_SNAP_OBJECTS)){
		cnt 			= F.r_u32();
        if (cnt){
	        for (int i=0; i<cnt; i++){
    	    	F.r_stringZ	(buf);
        	    CCustomObject* O = Scene.FindObjectByName(buf,OBJCLASS_SCENEOBJECT);
            	if (!O)		ELog.Msg(mtError,"DetailManager: Can't find object '%s' in scene.",buf);
	            else		m_SnapObjects.push_back(O);
    	    }
        }
    }

    if (F.find_chunk(DETMGR_CHUNK_DENSITY))
		psDetailDensity		= F.r_float();

	// base texture
	if(F.find_chunk(DETMGR_CHUNK_BASE_TEXTURE)){
	    F.r_stringZ		(buf);
    	if (m_Base.LoadImage(buf)){
		    m_Base.CreateShader();
		    m_Base.CreateRMFromObjects(m_BBox,m_SnapObjects);
        }else{
        	ELog.Msg(mtError,"DetailManager: Can't find base texture '%s'.",buf);
            ClearSlots();
            ClearBase();
        }
    }

    InvalidateCache		();

    return true;
}

void EDetailManager::Save(IWriter& F){
	// version
	F.open_chunk		(DETMGR_CHUNK_VERSION);
    F.w_u32				(DETMGR_VERSION);
    F.close_chunk		();

	// header
	F.w_chunk			(DETMGR_CHUNK_HEADER,&dtH,sizeof(DetailHeader));

    // objects
    SaveColorIndices	(F);

    // slots
	F.open_chunk		(DETMGR_CHUNK_SLOTS);
    F.w_u32				(dtH.size_x*dtH.size_z);
	F.w					(dtSlots,dtH.size_x*dtH.size_z*sizeof(DetailSlot));
    F.close_chunk		();
    // internal
    // bbox
	F.w_chunk			(DETMGR_CHUNK_BBOX,&m_BBox,sizeof(Fbox));
	// base texture
    if (m_Base.Valid()){
		F.open_chunk	(DETMGR_CHUNK_BASE_TEXTURE);
    	F.w_stringZ		(m_Base.GetName());
	    F.close_chunk	();
    }
    F.open_chunk		(DETMGR_CHUNK_DENSITY);
    F.w_float			(psDetailDensity);
    F.close_chunk		();
	// snap objects
	F.open_chunk		(DETMGR_CHUNK_SNAP_OBJECTS);
    F.w_u32				(m_SnapObjects.size());
    for (ObjectIt o_it=m_SnapObjects.begin(); o_it!=m_SnapObjects.end(); o_it++)
    	F.w_stringZ		((*o_it)->Name);
    F.close_chunk		();
}

bool EDetailManager::Export(LPCSTR fn)
{
    bool bRes=true;

    UI.ProgressStart	(4,"Making details...");
    dtH.version			= DETAIL_VERSION;
	CMemoryWriter F;
    dtH.object_count	= objects.size();
	// header
	F.w_chunk			(DETMGR_CHUNK_HEADER,&dtH,sizeof(DetailHeader));
	UI.ProgressInc		();

    // objects
	F.open_chunk		(DETMGR_CHUNK_OBJECTS);
    for (DetailIt it=objects.begin(); it!=objects.end(); it++){
		F.open_chunk	(it-objects.begin());
        if (!(*it)->m_pRefs){
        	ELog.DlgMsg(mtError, "Bad object or object not found '%s'.", (*it)->m_sRefs.c_str());
            bRes=false;
        }else{
	        (*it)->Export	(F);
        }
	    F.close_chunk	();
        if (!bRes) break;
    }
    F.close_chunk		();
	UI.ProgressInc();

    // slots
    if (bRes){
		F.open_chunk	(DETMGR_CHUNK_SLOTS);
		F.w				(dtSlots,dtH.size_x*dtH.size_z*sizeof(DetailSlot));
	    F.close_chunk	();
		UI.ProgressInc	();
    }

    if (bRes)			F.save_to(fn);

	UI.ProgressInc();
    UI.ProgressEnd		();
    return bRes;
}

void __fastcall	EDetailManager::OnDensityChange(PropValue* prop)
{
	InvalidateCache		();
}	

void EDetailManager::FillProp(LPCSTR pref, PropItemVec& values)
{
	PropValue* P;
    P=PHelper.CreateFloat	(values, FHelper.PrepareKey(pref,"Objects per square"),	&psDetailDensity);
    P->OnChangeEvent		= OnDensityChange;
}

