//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop   

#include "ESceneDOTools.h"
#include "EditMesh.h"
#include "EditObject.h"
#include "Texture.h"
#include "Scene.h"
#include "SceneObject.h"
#include "leftbar.h"
#include "cl_intersect.h"
#include "Library.h"
#include "ui_levelmain.h"

#include "Library.h"
#include "DetailFormat.h"
#include "Texture.h"
#include "bottombar.h"
#include "ImageManager.h"
#include "d3dutils.h"
#include "render.h"
#include "PropertiesListHelper.h"

static const u32 DETMGR_VERSION = 0x0003ul;
//------------------------------------------------------------------------------
enum{
    DETMGR_CHUNK_VERSION		= 0x1000ul,
    DETMGR_CHUNK_HEADER 		= 0x0000ul,
    DETMGR_CHUNK_OBJECTS 		= 0x0001ul,
    DETMGR_CHUNK_SLOTS			= 0x0002ul,
    DETMGR_CHUNK_BBOX			= 0x1001ul,
    DETMGR_CHUNK_BASE_TEXTURE	= 0x1002ul,
    DETMGR_CHUNK_COLOR_INDEX 	= 0x1003ul,
    DETMGR_CHUNK_SNAP_OBJECTS 	= 0x1004ul,
    DETMGR_CHUNK_DENSITY	 	= 0x1005ul,
    DETMGR_CHUNK_FLAGS			= 0x1006ul,
};
//----------------------------------------------------

//------------------------------------------------------------------------------
EDetailManager::EDetailManager():ESceneCustomMTools(OBJCLASS_DO)
{
	dtSlots				= 0;
    ZeroMemory			(&dtH,sizeof(dtH));
    m_Selected.clear	();
    InitRender			();
	Device.seqDevCreate.Add	(this,REG_PRIORITY_LOW);
	Device.seqDevDestroy.Add(this,REG_PRIORITY_NORMAL);
    m_Flags.assign		(flObjectsDraw);
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
    RemoveDOs			();
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
    ExecCommand			(COMMAND_REFRESH_SNAP_OBJECTS);
}
void EDetailManager::Clear(bool bSpecific)
{
	ClearBase			();
	ClearColorIndices	();
    ClearSlots			();
    m_Flags.zero		();
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
            	if (m_Flags.is(flSlotBoxesDraw)){
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
                            c.y			= slot->r_ybase()+slot->r_yheight()*0.5f; //(slot->y_max+slot->y_min)*0.5f;
                            float dist = Device.m_Camera.GetPosition().distance_to_sqr(c);
                         	if ((dist<dist_lim)&&::Render->ViewBase.testSphere_dirty(c,DETAIL_SLOT_SIZE_2)){
								bbox.min.set(c.x-DETAIL_SLOT_SIZE_2, slot->r_ybase(), 					c.z-DETAIL_SLOT_SIZE_2);
                            	bbox.max.set(c.x+DETAIL_SLOT_SIZE_2, slot->r_ybase()+slot->r_yheight(),	c.z+DETAIL_SLOT_SIZE_2);
                            	bbox.shrink	(0.05f);
								DU.DrawSelectionBox(bbox,bSel?&selected:&inactive);
							}
                        }
                    }
                }
            }else{
				RCache.set_xform_world				(Fidentity);
                if (m_Flags.is(flBaseTextureDraw))	m_Base.Render			(m_Flags.is(flBaseTextureBlended));
				if (m_Flags.is(flObjectsDraw))		CDetailManager::Render	();
            }
        }
    }
}
//------------------------------------------------------------------------------

void EDetailManager::OnDeviceCreate()
{
	// base texture
    m_Base.CreateShader();
	// detail objects
	for (DetailIt it=objects.begin(); it!=objects.end(); it++)
    	((EDetail*)(*it))->OnDeviceCreate();
	soft_Load	();
}

void EDetailManager::OnDeviceDestroy()
{
	// base texture
    m_Base.DestroyShader();
	// detail objects
	for (DetailIt it=objects.begin(); it!=objects.end(); it++)
    	((EDetail*)(*it))->OnDeviceDestroy();
	soft_Unload	();
}


void EDetailManager::OnObjectRemove(CCustomObject* O)
{
	m_SnapObjects.remove(O);
}

void EDetailManager::ExportColorIndices(LPCSTR fname)
{
	IWriter* F 	= FS.w_open(fname);
    SaveColorIndices(*F);
    FS.w_close	(F);
}

bool EDetailManager::ImportColorIndices(LPCSTR fname)
{
	IReader* F=FS.r_open(fname);
    if (F){
        ClearColorIndices	();
        LoadColorIndices	(*F);
        FS.r_close			(F);
        return true;
    }else{
    	ELog.DlgMsg			(mtError,"Can't open file '%s'.",fname);
        return false;
    }
}

void EDetailManager::SaveColorIndices(IWriter& F)
{
	// objects
	F.open_chunk		(DETMGR_CHUNK_OBJECTS);
    for (DetailIt it=objects.begin(); it!=objects.end(); it++){
		F.open_chunk	(it-objects.begin());
        ((EDetail*)(*it))->Save		(F);
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
	    for (DOIt d_it=i_it->second.begin(); d_it!=i_it->second.end(); d_it++)
        	F.w_stringZ	((*d_it)->GetName());
    }
    F.close_chunk		();
}

bool EDetailManager::LoadColorIndices(IReader& F)
{
	VERIFY				(objects.empty());
    VERIFY  			(m_ColorIndices.empty());

    bool bRes			= true;
    // objects
    IReader* OBJ 		= F.open_chunk(DETMGR_CHUNK_OBJECTS);
    if (OBJ){
        IReader* O   	= OBJ->open_chunk(0);
        for (int count=1; O; count++) {
            EDetail* DO	= xr_new<EDetail>();
            if (DO->Load(*O)) 	objects.push_back(DO);
            else				bRes = false;
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
            EDetail* DO	= FindDOByName(buf);
            if (DO) 	m_ColorIndices[index].push_back(DO);    
            else		bRes=false;
        }
    }
	InvalidateCache		();

    return bRes;
}

bool EDetailManager::Load(IReader& F)
{
    string256 buf;
    R_ASSERT			(F.find_chunk(DETMGR_CHUNK_VERSION));
	u32 version			= F.r_u32();

    if (version!=DETMGR_VERSION){
    	ELog.Msg(mtError,"EDetailManager: unsupported version.");
        return false;
    }

    if (F.find_chunk(DETMGR_CHUNK_FLAGS)) m_Flags.assign(F.r_u32());
    
	// header
    R_ASSERT			(F.r_chunk(DETMGR_CHUNK_HEADER,&dtH));

    // slots
    R_ASSERT			(F.find_chunk(DETMGR_CHUNK_SLOTS));
    int slot_cnt		= F.r_u32();
    dtSlots				= xr_alloc<DetailSlot>(slot_cnt);
    m_Selected.resize	(slot_cnt);
	F.r					(dtSlots,slot_cnt*sizeof(DetailSlot));

    // objects
    if (!LoadColorIndices(F)){
        ELog.DlgMsg		(mtError,"EDetailManager: Some objects removed. Reinitialize objects.",buf);
        InvalidateSlots	();
    }

    // internal
    // bbox
    R_ASSERT			(F.r_chunk(DETMGR_CHUNK_BBOX,&m_BBox));

	// snap objects
    if (F.find_chunk(DETMGR_CHUNK_SNAP_OBJECTS)){
		int snap_cnt 		= F.r_u32();
        if (snap_cnt){
	        for (int i=0; i<snap_cnt; i++){
    	    	F.r_stringZ	(buf);
        	    CCustomObject* O = Scene->FindObjectByName(buf,OBJCLASS_SCENEOBJECT);
            	if (!O)		ELog.Msg(mtError,"EDetailManager: Can't find snap object '%s'.",buf);
	            else		m_SnapObjects.push_back(O);
    	    }
        }
    }

    if (F.find_chunk(DETMGR_CHUNK_DENSITY))
		ps_r__Detail_density= F.r_float();

	// base texture
	if(F.find_chunk(DETMGR_CHUNK_BASE_TEXTURE)){
	    F.r_stringZ		(buf);
    	if (m_Base.LoadImage(buf)){
		    m_Base.CreateShader();
		    m_Base.CreateRMFromObjects(m_BBox,m_SnapObjects);
        }else{
        	ELog.Msg(mtError,"EDetailManager: Can't find base texture '%s'.",buf);
            ClearSlots();
            ClearBase();
        }
    }

    InvalidateCache		();

    return true;
}

bool EDetailManager::LoadSelection(IReader& F)
{
	Clear();
	return Load			(F);
}

void EDetailManager::Save(IWriter& F)
{
	// version
	F.open_chunk		(DETMGR_CHUNK_VERSION);
    F.w_u32				(DETMGR_VERSION);
    F.close_chunk		();

	F.open_chunk		(DETMGR_CHUNK_FLAGS);
    F.w_u32				(m_Flags.get());
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
    F.w_float			(ps_r__Detail_density);
    F.close_chunk		();
	// snap objects
	F.open_chunk		(DETMGR_CHUNK_SNAP_OBJECTS);
    F.w_u32				(m_SnapObjects.size());
    for (ObjectIt o_it=m_SnapObjects.begin(); o_it!=m_SnapObjects.end(); o_it++)
    	F.w_stringZ		((*o_it)->Name);
    F.close_chunk		();
}

void EDetailManager::SaveSelection(IWriter& F)
{
	Save(F);
}

bool EDetailManager::Export(LPCSTR path) 
{
    AnsiString fn		= AnsiString(path)+"build.details";
    bool bRes=true;

    SPBItem* pb = UI->ProgressStart(5,"Making details...");
	CMemoryWriter F;

    pb->Inc				("merge textures");
    Fvector2Vec			offsets;
    Fvector2Vec			scales;
    boolVec				rotated;
    RStringSet 			textures_set;
    RStringVec 			textures;
    U32Vec				remap;

    int slot_cnt		= dtH.size_x*dtH.size_z;
	for (int slot_idx=0; slot_idx<slot_cnt; slot_idx++){
    	DetailSlot* it = &dtSlots[slot_idx];
        for (int part=0; part<4; part++){
        	u8 id		= it->r_id(part);
        	if (id!=DetailSlot::ID_Empty) textures_set.insert(((EDetail*)(objects[id]))->GetTextureName());
        }
    }
    textures.assign		(textures_set.begin(),textures_set.end());

    U8Vec remap_object	(objects.size(),DetailSlot::ID_Empty);
    U8It remap_object_it= remap_object.begin();

    int new_idx			= 0;
    for (DetailIt d_it=objects.begin(); d_it!=objects.end(); d_it++,remap_object_it++)
    	*remap_object_it= textures_set.find(((EDetail*)(*d_it))->GetTextureName())==textures_set.end()?DetailSlot::ID_Empty:new_idx++;
//    	textures_set.insert(((EDetail*)(*d_it))->GetTextureName());

    AnsiString 			do_tex_name = ChangeFileExt(fn,"_details");
    int res				= ImageLib.CreateMergedTexture(textures,do_tex_name.c_str(),STextureParams::tfADXT1,256,1024,256,1024,offsets,scales,rotated,remap);
    if (1!=res)			bRes=FALSE;

    pb->Inc				("export geometry");
    // objects
    int object_idx		= 0;
    if (bRes){
	    do_tex_name 	= ExtractFileName(do_tex_name);
        F.open_chunk	(DETMGR_CHUNK_OBJECTS);
        for (DetailIt it=objects.begin(); it!=objects.end(); it++){
        	if (remap_object[it-objects.begin()]!=DetailSlot::ID_Empty){
                F.open_chunk	(object_idx++);
                if (!((EDetail*)(*it))->m_pRefs){
                    ELog.DlgMsg(mtError, "Bad object or object not found '%s'.", ((EDetail*)(*it))->m_sRefs.c_str());
                    bRes=false;
                }else{
                    LPCSTR tex_name = ((EDetail*)(*it))->GetTextureName();
                    for (u32 t_idx=0; t_idx<textures.size(); t_idx++) 
                        if (textures[t_idx]==tex_name) break;
                    VERIFY(t_idx<textures.size());
                    t_idx = remap[t_idx];
                    ((EDetail*)(*it))->Export	(F,do_tex_name.c_str(),offsets[t_idx],scales[t_idx],rotated[t_idx]);
                }
                F.close_chunk	();
                if (!bRes) break;
            }
        }
        F.close_chunk		();
    }
    
    pb->Inc	("export slots");
    // slots
    if (bRes){
    	xr_vector<DetailSlot> dt_slots(slot_cnt); dt_slots.assign(dtSlots,dtSlots+slot_cnt);
        for (slot_idx=0; slot_idx<slot_cnt; slot_idx++){
            DetailSlot& it = dt_slots[slot_idx];
            for (int part=0; part<4; part++){
                u8 id		= it.r_id(part);
                if (id!=DetailSlot::ID_Empty) it.w_id(part,remap_object[id]);
            }
        }
		F.open_chunk	(DETMGR_CHUNK_SLOTS);
		F.w				(dt_slots.begin(),dtH.size_x*dtH.size_z*sizeof(DetailSlot));
	    F.close_chunk	();
        pb->Inc();

        // write header
        dtH.version		= DETAIL_VERSION;
        dtH.object_count= object_idx;

        F.w_chunk		(DETMGR_CHUNK_HEADER,&dtH,sizeof(DetailHeader));

    	F.save_to(fn.c_str());
    }

    pb->Inc();
    UI->ProgressEnd(pb);
    return bRes;
}

void EDetailManager::OnDensityChange(PropValue* prop)
{
	InvalidateCache		();
}	


void EDetailManager::OnBaseTextureChange(PropValue* prop)
{
	m_Base.OnImageChange	(prop);
    InvalidateSlots			();
    ELog.DlgMsg				(mtInformation,"Texture changed. Reinitialize objects.");
}

void EDetailManager::FillProp(LPCSTR pref, PropItemVec& values)
{
	PropValue* P;
    P=PHelper().CreateFloat	(values, PrepareKey(pref,"Objects per square"),				&ps_r__Detail_density);
    P->OnChangeEvent.bind	(this,&EDetailManager::OnDensityChange);
    P=PHelper().CreateChoose(values, PrepareKey(pref,"Base Texture"),					&m_Base.name, smTexture);
    P->OnChangeEvent.bind	(this,&EDetailManager::OnBaseTextureChange);
    PHelper().CreateFlag32	(values, PrepareKey(pref,"Common\\Draw objects"),			&m_Flags,	flObjectsDraw);
    PHelper().CreateFlag32	(values, PrepareKey(pref,"Common\\Draw base texture"),		&m_Flags,	flBaseTextureDraw);
    PHelper().CreateFlag32	(values, PrepareKey(pref,"Common\\Base texture blended"),	&m_Flags,	flBaseTextureBlended);
    PHelper().CreateFlag32	(values, PrepareKey(pref,"Common\\Draw slot boxes"),			&m_Flags,	flSlotBoxesDraw);
}

bool EDetailManager::GetSummaryInfo(SSceneSummary* inf)
{
	for (DetailIt it=objects.begin(); it!=objects.end(); it++){
    	((EDetail*)(*it))->OnDeviceCreate();
        CEditableObject* E 	= ((EDetail*)(*it))->m_pRefs;
		if (!E)				continue;
	    CSurface* surf		= *E->FirstSurface(); VERIFY(surf);
        inf->do_textures.insert(surf->_Texture());
    }
    return true;
}

