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
#include "frustum.h"
#include "bottombar.h"
#include "ImageManager.h"

static Fvector down_vec	={0.f,-1.f,0.f};
static Fvector left_vec	={-1.f,0.f,0.f};
static Fvector right_vec={1.f,0.f,0.f};
static Fvector fwd_vec	={0.f,0.f,1.f};
static Fvector back_vec	={0.f,0.f,-1.f};

static CRandom DetailRandom(0x26111975);

const DWORD	vs_size				= 3000;
//------------------------------------------------------------------------------

#define DETOBJ_CHUNK_VERSION		0x1000
#define DETOBJ_CHUNK_REFERENCE 		0x0101
#define DETOBJ_CHUNK_SCALE_LIMITS	0x0102
#define DETOBJ_CHUNK_DENSITY_FACTOR	0x0103
#define DETOBJ_CHUNK_FLAGS			0x0104

#define DETOBJ_VERSION 				0x0001
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
CDetail::CDetail(){
	m_pShader			= 0;
	m_dwFlags			= 0;
	m_fRadius			= 0;
	m_pRefs				= 0;
    m_bSideFlag			= false;
    m_bMarkDel			= false;
    m_fMinScale			= 0.5f;
    m_fMaxScale         = 2.f;
    m_fDensityFactor	= 1.f;
    m_sRefs				= "";
}

CDetail::~CDetail(){
	m_Vertices.clear	();
    Lib.RemoveEditObject(m_pRefs);
}

LPCSTR CDetail::GetName	(){
    return m_pRefs?m_pRefs->GetName():m_sRefs.c_str();
}

void CDetail::OnDeviceCreate(){
	if (!m_pRefs)		return;
    CSurface* surf		= *m_pRefs->FirstSurface();
    VERIFY				(surf);
    VERIFY				(surf->_Shader());
    m_pShader			= surf->_Shader();
}

void CDetail::OnDeviceDestroy(){
    m_pShader 			= 0;
}

int CDetail::_AddVert(const Fvector& p, float u, float v)
{
	fvfVertexIn V(p,u,v);
	for (DOVertIt v_it=m_Vertices.begin(); v_it!=m_Vertices.end(); v_it++)
    	if (v_it->similar(V)) return v_it-m_Vertices.begin();
	m_Vertices.push_back(V);
    return m_Vertices.size()-1;
}

bool CDetail::Update	(LPCSTR name){
	m_sRefs				= name;
    // update link
    CEditableObject* R	= Lib.CreateEditObject(name);
    if (!R){
        ELog.DlgMsg		(mtError, "CDetail: '%s' not found in library", name);
        return false;
    }
    if(R->SurfaceCount()!=1){
    	ELog.DlgMsg		(mtError,"Object must contain 1 material.");
	    Lib.RemoveEditObject(R);
    	return false;
    }
	if(R->MeshCount()==0){
    	ELog.DlgMsg		(mtError,"Object must contain 1 mesh.");
	    Lib.RemoveEditObject(R);
    	return false;
    }

    Lib.RemoveEditObject(m_pRefs);
    m_pRefs				= R;

    // get surface
    CSurface* surf		= *m_pRefs->FirstSurface();
    R_ASSERT			(surf);
    m_bSideFlag			= surf->_2Sided();

    // create shader
    m_pShader			= surf->_Shader();
    R_ASSERT			(m_pShader);

    // fill geometry
    CEditableMesh* M	= *m_pRefs->FirstMesh();

    m_Indices.resize	(M->GetFaceCount(false)*3);

    // fill vertices
    Fbox bbox;
    bbox.invalidate();
    WORDIt i_it			= m_Indices.begin();
    for (FaceIt f_it=M->m_Faces.begin(); f_it!=M->m_Faces.end(); f_it++){
    	for (int k=0; k<3; k++,i_it++){
        	st_Face& F 	= *f_it;
            Fvector& P  = M->m_Points[F.pv[k].pindex];
            st_VMapPt& vm=M->m_VMRefs[F.pv[k].vmref][0];
            Fvector2& uv= M->m_VMaps[vm.vmap_index]->getUV(vm.index);
        	*i_it 		= _AddVert	(P,uv.x,uv.y);
	        bbox.modify	(m_Vertices[*i_it].P);
        }
    }
    m_fRadius			= bbox.getradius();

    return true;
}

bool CDetail::Load(CStream& F){
	// check version
    R_ASSERT			(F.FindChunk(DETOBJ_CHUNK_VERSION));
    DWORD version		= F.Rdword();
    if (version!=DETOBJ_VERSION){
    	ELog.Msg(mtError,"CDetail: unsupported version.");
        return false;
    }

	// references
	char buf[255];
    R_ASSERT			(F.FindChunk(DETOBJ_CHUNK_REFERENCE));
    F.RstringZ			(buf);

    // scale
    R_ASSERT			(F.FindChunk(DETOBJ_CHUNK_SCALE_LIMITS));
    m_fMinScale			= F.Rfloat(); if (fis_zero(m_fMinScale)) 	m_fMinScale = 0.1f;
	m_fMaxScale         = F.Rfloat(); if (m_fMaxScale<m_fMinScale) 	m_fMaxScale = m_fMinScale;

	// density factor
    if (F.FindChunk(DETOBJ_CHUNK_DENSITY_FACTOR))
	    m_fDensityFactor= F.Rfloat();

    if (F.FindChunk(DETOBJ_CHUNK_FLAGS))
    	m_dwFlags		= F.Rdword();

    // update object
    return 				Update(buf);
}

void CDetail::Save(CFS_Base& F){
	// version
	F.open_chunk		(DETOBJ_CHUNK_VERSION);
    F.Wdword			(DETOBJ_VERSION);
    F.close_chunk		();

    // reference
	F.open_chunk		(DETOBJ_CHUNK_REFERENCE);
    F.WstringZ			(m_sRefs.c_str());
    F.close_chunk		();

	// scale
	F.open_chunk		(DETOBJ_CHUNK_SCALE_LIMITS);
    F.Wfloat			(m_fMinScale);
    F.Wfloat			(m_fMaxScale);
    F.close_chunk		();

	// density factor
	F.open_chunk		(DETOBJ_CHUNK_DENSITY_FACTOR);
    F.Wfloat			(m_fDensityFactor);
    F.close_chunk		();

    // flags
	F.open_chunk		(DETOBJ_CHUNK_FLAGS);
    F.Wdword			(m_dwFlags);
    F.close_chunk		();
}

void CDetail::Export(CFS_Base& F){
	R_ASSERT			(m_pRefs);
    CSurface* surf		= *m_pRefs->FirstSurface();
	R_ASSERT			(surf);
    // write data
	F.WstringZ			(surf->_ShaderName());
	F.WstringZ			(surf->_Texture());

    F.Wdword			(m_dwFlags);
    F.Wfloat			(m_fMinScale);
    F.Wfloat			(m_fMaxScale);

    F.Wdword			(m_Vertices.size());
    F.Wdword			(m_Indices.size());

    F.write				(m_Vertices.begin(), m_Vertices.size()*sizeof(fvfVertexIn));
    F.write				(m_Indices.begin(), m_Indices.size()*sizeof(WORD));
}

//------------------------------------------------------------------------------
CDetailManager::CDetailManager(){
	m_fDensity			= 0.1f;
    ZeroMemory			(&m_Header,sizeof(DetailHeader));
    m_Selected.clear	();
    InitRender			();
//s	VS					= Device.Streams.Create	(D3DFVF_XYZ | D3DFVF_TEX1, vs_size);
	Device.seqDevCreate.Add	(this,REG_PRIORITY_LOW);
	Device.seqDevDestroy.Add(this,REG_PRIORITY_NORMAL);
}

CDetailManager::~CDetailManager(){
	Device.seqDevCreate.Remove(this);
	Device.seqDevDestroy.Remove(this);
	Clear();
    m_Slots.clear		();
	m_Cache.clear		();
	m_Visible.clear		();
}

void CDetailManager::OnDeviceCreate(){
	// base texture
    m_Base.CreateShader();
	// detail objects
	for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnDeviceCreate();
}

void CDetailManager::OnDeviceDestroy(){
	// base texture
    m_Base.DestroyShader();
	// detail objects
	for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->OnDeviceDestroy();
}

DetailSlot&	CDetailManager::GetSlot(DWORD sx, DWORD sz){
	VERIFY(sx<m_Header.size_x);
	VERIFY(sz<m_Header.size_z);
	return m_Slots[sz*m_Header.size_x+sx];
}

void CDetailManager::FindClosestIndex(const Fcolor& C, SIndexDistVec& best){
	DWORD index;
    float dist = flt_max;
    Fcolor src;
    float inv_a = 1-C.a;
    bool bRes=false;
    ColorIndexPairIt S = m_ColorIndices.begin();
    ColorIndexPairIt E = m_ColorIndices.end();
    ColorIndexPairIt it= S;
	for(; it!=E; it++){
		src.set(it->first);
        float d = inv_a+sqrtf((C.r-src.r)*(C.r-src.r)+(C.g-src.g)*(C.g-src.g)+(C.b-src.b)*(C.b-src.b));
        if (d<dist){
        	dist 	= d;
            index 	= it->first;
            bRes	= true;
        }
    }

    if (bRes){
        if (best.size()<4){
            bool bFound=false;
            for (DWORD k=0; k<best.size(); k++){
                if (best[k].index==index){
                	if(dist<best[k].dist){
	                    best[k].dist 	= dist;
    	                best[k].index	= index;
                    }
                    bFound = true;
                    break;
                }
            }
            if (!bFound){
                best.inc();
                best[best.size()-1].dist = dist;
                best[best.size()-1].index= index;
            }
        }else{
            int i=-1;
            float dd=flt_max;
            bool bFound=false;
            for (int k=0; k<4; k++){
                float d = dist-best[k].dist;
                if ((d<0)&&(d<dd)){ i=k; dd=d;}
                if (best[k].index==index){
                	if(dist<best[k].dist){
	                    best[k].dist 	= dist;
    	                best[k].index    = index;
                    }
                    bFound = true;
                    break;
                }
            }
            if (!bFound&&(i>=0)){
                best[i].dist 	= dist;
                best[i].index	= index;
            }
        }
    }
}

bool CDetailManager::Initialize(LPCSTR tex_name){
	if (!fraLeftBar->ebEnableSnapList->Down||Scene.m_SnapObjects.empty()){
    	ELog.DlgMsg(mtError,"Fill snap list and activate before generating slots!");
    	return false;
    }

    if (m_Objects.empty()){
    	ELog.DlgMsg(mtError,"Fill object list before generating slots!");
    	return false;
    }

    m_SnapObjects.clear();
    m_SnapObjects = Scene.m_SnapObjects;

    if (!UpdateHeader())                return false;
	if (!UpdateBaseTexture(tex_name))	return false;
    if (!UpdateSlots()) 		   		return false;
    if (!UpdateObjects(false,false))	return false;
	return true;
}

bool CDetailManager::Reinitialize(){
	if (m_SnapObjects.empty()){
    	ELog.DlgMsg(mtError,"Snap list empty! Initialize at first.");
    	return false;
    }

    if (m_Objects.empty()){
    	ELog.DlgMsg(mtError,"Fill object list before reinitialize!");
    	return false;
    }

    InvalidateCache();

    if (!UpdateHeader())            return false;
    if (!UpdateBaseTexture(0))		return false;
    if (!UpdateSlots()) 			return false;
    if (!UpdateObjects(false,false))return false;							

	return true;
}

bool CDetailManager::UpdateHeader(){
    // get bounding box
	if (!Scene.GetBox(m_BBox,m_SnapObjects)) return false;

    // fill header
    int mn_x 			= iFloor(m_BBox.min.x/DETAIL_SLOT_SIZE);
    int mn_z 			= iFloor(m_BBox.min.z/DETAIL_SLOT_SIZE);
    int mx_x 			= iFloor(m_BBox.max.x/DETAIL_SLOT_SIZE)+1;
    int mx_z 			= iFloor(m_BBox.max.z/DETAIL_SLOT_SIZE)+1;
    m_Header.offs_x 	= -mn_x;
    m_Header.offs_z 	= -mn_z;
	m_Header.size_x 	= mx_x-mn_x;
	m_Header.size_z 	= mx_z-mn_z;
    return true;
}

bool CDetailManager::UpdateBaseTexture(LPCSTR tex_name){
    // create base texture
    if (!tex_name&&!m_Base.Valid()){
    	ELog.DlgMsg(mtError,"Initialize at first!");
    	return false;
    }
    AnsiString fn = tex_name?tex_name:m_Base.GetName();
//s    m_Base.Clear();
    m_Base.LoadImage(fn.c_str());
    if (!m_Base.Valid()){
//s	    m_Base.Clear();
    	ELog.DlgMsg(mtError,"Can't load base image '%s'!",fn.c_str());
    	return false;
    }
	m_Base.RecreateShader();
    m_Base.CreateRMFromObjects(m_BBox,m_SnapObjects);
    return true;
}

#define EPS_L_VAR 0.0012345f
void CDetailManager::UpdateSlotBBox(int sx, int sz, DetailSlot& slot){
	Fbox bbox;
    Frect rect;
    GetSlotRect			(rect,sx,sz);
    bbox.min.set		(rect.x1, slot.y_min, rect.y1);
    bbox.max.set		(rect.x2, slot.y_max, rect.y2);

    SBoxPickInfoVec pinf;
    XRC.box_options(0);
    if (Scene.BoxPick(bbox,pinf,&m_SnapObjects)){
		bbox.grow		(EPS_L_VAR);
    	Fplane			frustum_planes[4];
		frustum_planes[0].build(bbox.min,left_vec);
		frustum_planes[1].build(bbox.min,back_vec);
		frustum_planes[2].build(bbox.max,right_vec);
		frustum_planes[3].build(bbox.max,fwd_vec);

        CFrustum frustum;
        frustum.CreateFromPlanes(frustum_planes,4);

        slot.y_min		= flt_max;
        slot.y_max		= flt_min;
		for (SBoxPickInfoIt it=pinf.begin(); it!=pinf.end(); it++){
        	for (int k=0; k<(int)it->inf.size(); k++){
                float range;
                Fvector verts[3];
                it->s_obj->GetFaceWorld(it->e_mesh,it->inf[k].id,verts);
                sPoly sSrc	(verts,3);
                sPoly sDest;
                sPoly* sRes = frustum.ClipPoly(sSrc, sDest);
                if (sRes){
                    for (DWORD k=0; k<sRes->size(); k++){
                        float H = (*sRes)[k].y;
                        if (H>slot.y_max) slot.y_max = H+0.03f;
                        if (H<slot.y_min) slot.y_min = H-0.03f;
                    }
                }
            }
	    }
    }else{
    	ZeroMemory(&slot,sizeof(DetailSlot));
    	slot.items[0].id=0xff;
    	slot.items[1].id=0xff;
    	slot.items[2].id=0xff;
    	slot.items[3].id=0xff;
    }
}

bool CDetailManager::UpdateSlots(){
	// clear previous slots
    m_Slots.clear		();
    m_Slots.resize		(m_Header.size_x*m_Header.size_z);

    UI.ProgressStart	(m_Header.size_x*m_Header.size_z,"Updating bounding boxes...");
    for (DWORD z=0; z<m_Header.size_z; z++){
        for (DWORD x=0; x<m_Header.size_x; x++){
        	DSIt slot	= m_Slots.begin()+z*m_Header.size_x+x;
        	slot->y_min	= m_BBox.min.y;
        	slot->y_max	= m_BBox.max.y;
        	UpdateSlotBBox(x,z,*slot);
			UI.ProgressInc();
        }
    }
    UI.ProgressEnd		();

    m_Selected.resize	(m_Header.size_x*m_Header.size_z);

    return true;
}

void CDetailManager::GetSlotRect(Frect& rect, int sx, int sz){
    float x 			= fromSlotX(sx);
    float z 			= fromSlotZ(sz);
    rect.x1				= x-DETAIL_SLOT_SIZE_2+EPS_L;
    rect.y1				= z-DETAIL_SLOT_SIZE_2+EPS_L;
    rect.x2				= x+DETAIL_SLOT_SIZE_2-EPS_L;
    rect.y2				= z+DETAIL_SLOT_SIZE_2-EPS_L;
}

void CDetailManager::GetSlotTCRect(Irect& rect, int sx, int sz){
	Frect R;
	GetSlotRect			(R,sx,sz);
	rect.x1 			= m_Base.GetPixelUFromX(R.x1,m_BBox);
	rect.x2 			= m_Base.GetPixelUFromX(R.x2,m_BBox);
	rect.y2 			= m_Base.GetPixelVFromZ(R.y1,m_BBox); // v - координата флипнута
	rect.y1 			= m_Base.GetPixelVFromZ(R.y2,m_BBox);
}

void CDetailManager::CalcClosestCount(int part, const Fcolor& C, SIndexDistVec& best){
    float dist = flt_max;
    Fcolor src;
    float inv_a = 1-C.a;
    int idx = -1;

    for (DWORD k=0; k<best.size(); k++){
		src.set(best[k].index);
        float d = inv_a+sqrtf((C.r-src.r)*(C.r-src.r)+(C.g-src.g)*(C.g-src.g)+(C.b-src.b)*(C.b-src.b));
        if (d<dist){
        	dist 	= d;
            idx 	= k;
        }
    }
    if (idx>=0) best[idx].cnt[part]++;
}

BYTE CDetailManager::GetRandomObject(DWORD color_index){
	ColorIndexPairIt CI=m_ColorIndices.find(color_index);
	R_ASSERT(CI!=m_ColorIndices.end());
	int k = DetailRandom.randI(0,CI->second.size());
    DOIt it = find(m_Objects.begin(),m_Objects.end(),CI->second[k]);
    VERIFY(it!=m_Objects.end());
	return (it-m_Objects.begin());
}

BYTE CDetailManager::GetObject(ColorIndexPairIt& CI, BYTE id){
	VERIFY(CI!=m_ColorIndices.end());
    DOIt it = find(m_Objects.begin(),m_Objects.end(),CI->second[id]);
    VERIFY(it!=m_Objects.end());
	return (it-m_Objects.begin());
}

bool CompareWeightFunc(SIndexDist& d0, SIndexDist& d1){
	return d0.dist<d1.dist;
}

struct best_rand
{
	CRandom gen;
	best_rand(CRandom& A) { gen=A; }
	int operator()(int n) {return gen.randI(n);}
};

bool CDetailManager::UpdateSlotObjects(int x, int z){
    srand(time(NULL));

    DSIt slot	= m_Slots.begin()+z*m_Header.size_x+x;
    Irect		R;
    GetSlotTCRect(R,x,z);
    //ELog.Msg(mtInformation,"TC [%d,%d]-[%d,%d]",R.x1,R.y1,R.x2,R.y2);
    SIndexDistVec best;
    // find best color index
    {
        for (int v=R.y1; v<=R.y2; v++){
            for (int u=R.x1; u<=R.x2; u++){
                DWORD clr;
                if (m_Base.GetColor(clr,u,v)){
                    Fcolor C;
                    C.set(clr);
                    FindClosestIndex(C,best);
                }
            }
        }
    }
    sort(best.begin(),best.end(),CompareWeightFunc);
    // пройдем по 4 частям слота и определим плотность заполнения (учесть переворот V)
    Irect P[4];
    float dx=float(R.x2-R.x1)/2.f;
    float dy=float(R.y2-R.y1)/2.f;

//	2 3
//	0 1
    P[0].x1=R.x1; 		  			P[0].y1=iFloor(R.y1+dy+0.501f); P[0].x2=iFloor(R.x1+dx+.499f);	P[0].y2=R.y2;
    P[1].x1=iFloor(R.x1+dx+0.501f);	P[1].y1=iFloor(R.y1+dy+0.501f);	P[1].x2=R.x2; 					P[1].y2=R.y2;
    P[2].x1=R.x1; 		  			P[2].y1=R.y1; 		  			P[2].x2=iFloor(R.x1+dx+.499f); 	P[2].y2=iFloor(R.y1+dy+.499f);
    P[3].x1=iFloor(R.x1+dx+0.501f); P[3].y1=R.y1;		  			P[3].x2=R.x2; 					P[3].y2=iFloor(R.y1+dx+.499f);

    for (int part=0; part<4; part++){
        float	alpha=0;
        int 	cnt=0;
        for (int v=P[part].y1; v<=P[part].y2; v++){
            for (int u=P[part].x1; u<=P[part].x2; u++){
                DWORD clr;
                if (m_Base.GetColor(clr,u,v)){
                    Fcolor C;
                    C.set(clr);
                    CalcClosestCount(part,C,best);
                    alpha+=C.a;
                    cnt++;
                }
            }
        }
        alpha/=(cnt?float(cnt):1);
        alpha*=0.5f;
        for (DWORD i=0; i<best.size(); i++)
            best[i].dens[part] = cnt?(best[i].cnt[part]*alpha)/float(cnt):0;
    }

    // fill empty slots
    R_ASSERT(best.size());
    int id=-1;
    DWORD o_cnt=0;
    for (DWORD i=0; i<best.size(); i++)
        o_cnt+=m_ColorIndices[best[i].index].size();
    // равномерно заполняем пустые слоты
    if (o_cnt>best.size()){
        while (best.size()<4){
            do{
	            id++;
                if (id>3) id=0;
            }while(m_ColorIndices[best[id].index].size()<=1);
			best.push_back(SIndexDist());
            best.back()=best[id];
            if (best.size()==o_cnt) break;
        }
    }

    // заполним палитру и установим Random'ы
//	Msg("Slot: %d %d",x,z);
    for(DWORD k=0; k<best.size(); k++){
     	// objects
		ColorIndexPairIt CI=m_ColorIndices.find(best[k].index); R_ASSERT(CI!=m_ColorIndices.end());
        BYTEVec elem; elem.resize(CI->second.size());
        for (BYTEIt b_it=elem.begin(); b_it!=elem.end(); b_it++) *b_it=b_it-elem.begin();
//        best_rand A(DetailRandom);
        random_shuffle(elem.begin(),elem.end());//,A);
        for (b_it=elem.begin(); b_it!=elem.end(); b_it++){
			bool bNotFound=true;
            slot->items[k].id       = GetObject(CI,*b_it);
            for (DWORD j=0; j<k; j++)
                if (slot->items[j].id==slot->items[k].id){
                	bNotFound=false;
                    break;
                }
            if (bNotFound) break;
        }

        slot->color					= 0xffffffff;
        // density
        float f = m_Objects[slot->items[k].id]->m_fDensityFactor;

        slot->items[k].palette.a0 	= iFloor(best[k].dens[0]*f*15.f+.5f);
        slot->items[k].palette.a1 	= iFloor(best[k].dens[1]*f*15.f+.5f);
        slot->items[k].palette.a2 	= iFloor(best[k].dens[2]*f*15.f+.5f);
        slot->items[k].palette.a3 	= iFloor(best[k].dens[3]*f*15.f+.5f);
/*
		static bool bFnd=false;

        slot->items[k].palette.a0 	= bFnd?0:7;//iFloor(best[k].dens[3]*f*15.f+.5f);
        slot->items[k].palette.a1 	= 0;//iFloor(best[k].dens[2]*f*15.f+.5f);
        slot->items[k].palette.a2 	= 0;//iFloor(best[k].dens[1]*f*15.f+.5f);
        slot->items[k].palette.a3 	= 0;//iFloor(best[k].dens[0]*f*15.f+.5f);
        bFnd=true;
*/
//		Msg("Palette: %d %d %d %d",slot->items[k].palette.a0,slot->items[k].palette.a1,slot->items[k].palette.a2,slot->items[k].palette.a3);
    }

    // определим ID незаполненных слотов как пустышки
    for(k=best.size(); k<4; k++)
        slot->items[k].id = 0xff;
    return true;
}

bool CDetailManager::UpdateObjects(bool bUpdateTex, bool bUpdateSelectedOnly){
    // reload base texture
    if (bUpdateTex&&!UpdateBaseTexture(0)) 	return false;
    // update objects
    UI.ProgressStart	(m_Header.size_x*m_Header.size_z,"Updating objects...");
    for (DWORD z=0; z<m_Header.size_z; z++)
        for (DWORD x=0; x<m_Header.size_x; x++){
        	if (!bUpdateSelectedOnly||(bUpdateSelectedOnly&&m_Selected[z*m_Header.size_x+x]))
	        	UpdateSlotObjects(x,z);
		    UI.ProgressInc();
        }
    UI.ProgressEnd		();

    InvalidateCache		();

    return true;
}

void CDetailManager::Clear(bool bOnlySlots){
	if (!bOnlySlots){
		RemoveObjects		();
		m_ColorIndices.clear();
//s	    m_Base.Clear		();
   		m_SnapObjects.clear	();
    }
    ZeroMemory			(&m_Header,sizeof(DetailHeader));
	m_Selected.clear	();
    m_Slots.clear		();
    InvalidateCache		();
}


CDetail* CDetailManager::FindObjectByName(LPCSTR name){
	for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	if (stricmp((*it)->GetName(),name)==0) return *it;
    return 0;
}

void CDetailManager::MarkAllObjectsAsDel(){
	for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->m_bMarkDel = true;
}


CDetail* CDetailManager::AppendObject(LPCSTR name, bool bTestUnique){
    CDetail* D=0;
	if (bTestUnique&&(D=FindObjectByName(name))) return D;

    D = new CDetail();
    if (!D->Update(name)){
    	_DELETE(D);
        return 0;
    }
    m_Objects.push_back(D);
	return D;
}

void CDetailManager::InvalidateSlots(){
	for (DSIt it=m_Slots.begin(); it!=m_Slots.end(); it++){
    	it->items[0].id = 0xff;
    	it->items[1].id = 0xff;
    	it->items[2].id = 0xff;
    	it->items[3].id = 0xff;
    }
    InvalidateCache();
}

int CDetailManager::RemoveObjects(bool bOnlyMarked){
	int cnt=0;
	if (bOnlyMarked){
		for (DWORD i=0; i<m_Objects.size(); i++){  // не менять int i; на DWORD
    		if (m_Objects[i]->m_bMarkDel){
            	_DELETE(m_Objects[i]);
	            m_Objects.erase(m_Objects.begin()+i);
    	        i--;
                cnt++;
            }
        }
    }else{
		for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    		_DELETE(*it);
        cnt = m_Objects.size();
	    m_Objects.clear();
    }
    return cnt;
}

void CDetailManager::RemoveColorIndices(){
	m_ColorIndices.clear();
}

CDetail* CDetailManager::FindObjectInColorIndices(DWORD index, LPCSTR name){
	ColorIndexPairIt CI=m_ColorIndices.find(index);
	if (CI!=m_ColorIndices.end()){
    	DOVec& lst = CI->second;
		for (DOIt it=lst.begin(); it!=lst.end(); it++)
    		if (stricmp((*it)->GetName(),name)==0) return *it;
    }
    return 0;
}

void CDetailManager::AppendIndexObject(DWORD color,LPCSTR name, bool bTestUnique){
	if (bTestUnique){
		CDetail* DO = FindObjectInColorIndices(color,name);
        if (DO)
			m_ColorIndices[color].push_back(DO);
    }else{
		CDetail* DO = FindObjectByName(name);
	    R_ASSERT(DO);
		m_ColorIndices[color].push_back(DO);
    }
}

void CDetailManager::RemoveFromSnapList(CCustomObject* O)
{
	m_SnapObjects.remove(O);
}

void CDetailManager::ExportColorIndices(LPCSTR fname)
{
	CFS_Memory F;
    SaveColorIndices(F);
    F.SaveTo(fname,0);
}

void CDetailManager::ImportColorIndices(LPCSTR fname)
{
	Clear				(false);
	CStream* F=Engine.FS.Open(fname); R_ASSERT(F);
	LoadColorIndices(*F);
	Engine.FS.Close(F);
}

void CDetailManager::SaveColorIndices(CFS_Base& F)
{
	// objects
	F.open_chunk		(DETMGR_CHUNK_OBJECTS);
    for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
		F.open_chunk	(it-m_Objects.begin());
        (*it)->Save		(F);
	    F.close_chunk	();
    }
    F.close_chunk		();
    // color index map
	F.open_chunk		(DETMGR_CHUNK_COLOR_INDEX);
    F.Wbyte				(m_ColorIndices.size());
    ColorIndexPairIt S 	= m_ColorIndices.begin();
    ColorIndexPairIt E 	= m_ColorIndices.end();
    ColorIndexPairIt i_it= S;
	for(; i_it!=E; i_it++){
		F.Wdword		(i_it->first);
        F.Wbyte			(i_it->second.size());
	    for (DOIt do_it=i_it->second.begin(); do_it!=i_it->second.end(); do_it++)
        	F.WstringZ	((*do_it)->GetName());
    }
    F.close_chunk		();
}

bool CDetailManager::LoadColorIndices(CStream& F)
{
    // objects
    CStream* OBJ 		= F.OpenChunk(DETMGR_CHUNK_OBJECTS);
    if (OBJ){
        CStream* O   	= OBJ->OpenChunk(0);
        for (int count=1; O; count++) {
            CDetail* DO = new CDetail();
            if (!DO->Load(*O)) ELog.Msg(mtError,"Can't load detail object.");
			m_Objects.push_back(DO);
            O->Close();
            O = OBJ->OpenChunk(count);
        }
        OBJ->Close();
    }
    // color index map
    R_ASSERT			(F.FindChunk(DETMGR_CHUNK_COLOR_INDEX));
    int cnt				= F.Rbyte();
    string256			buf;
    DWORD index;
    int ref_cnt;
    for (int k=0; k<cnt; k++){
		index			= F.Rdword();
        ref_cnt			= F.Rbyte();
		for (int j=0; j<ref_cnt; j++){
        	F.RstringZ	(buf);
            CDetail* DO	= FindObjectByName(buf);
            if (DO) 	m_ColorIndices[index].push_back(DO);
        }
    }

    return true;
}

bool CDetailManager::Load(CStream& F){
    string256 buf;
    R_ASSERT			(F.FindChunk(DETMGR_CHUNK_VERSION));
	DWORD version		= F.Rdword();

    if (version!=DETMGR_VERSION){
    	ELog.Msg(mtError,"CDetailManager: unsupported version.");
        return false;
    }

	// header
    R_ASSERT			(F.ReadChunk(DETMGR_CHUNK_HEADER,&m_Header));

    // objects
    LoadColorIndices	(F);

    // slots
    R_ASSERT			(F.FindChunk(DETMGR_CHUNK_SLOTS));
    int cnt 			= F.Rdword();
    m_Slots.resize		(cnt);
    m_Selected.resize	(cnt);
	F.Read				(m_Slots.begin(),m_Slots.size()*sizeof(DetailSlot));

    // internal
    // bbox
    R_ASSERT			(F.ReadChunk(DETMGR_CHUNK_BBOX,&m_BBox));

	// snap objects
    if (F.FindChunk(DETMGR_CHUNK_SNAP_OBJECTS)){
		cnt 			= F.Rdword();
        if (cnt){
	        for (int i=0; i<cnt; i++){
    	    	F.RstringZ	(buf);
        	    CCustomObject* O = Scene.FindObjectByName(buf,OBJCLASS_SCENEOBJECT);
            	if (!O)		ELog.Msg(mtError,"DetailManager: Can't find object '%s' in scene.",buf);
	            else		m_SnapObjects.push_back(O);
    	    }
        }else{
	    	m_SnapObjects= Scene.m_SnapObjects;
        }
    }else{
    	m_SnapObjects	= Scene.m_SnapObjects;
    }

    if (F.FindChunk(DETMGR_CHUNK_DENSITY))
		m_fDensity 			= F.Rfloat();

	// base texture
	if(F.FindChunk(DETMGR_CHUNK_BASE_TEXTURE)){
	    F.RstringZ		(buf);
    	if (m_Base.LoadImage(buf)){
		    m_Base.CreateShader();
		    m_Base.CreateRMFromObjects(m_BBox,m_SnapObjects);
        }else{
        	ELog.Msg(mtError,"DetailManager: Can't find base texture '%s'.",buf);
            Clear(true);
        }
    }

    InvalidateCache		();

    return true;
}

void CDetailManager::Save(CFS_Base& F){
	// version
	F.open_chunk		(DETMGR_CHUNK_VERSION);
    F.Wdword			(DETMGR_VERSION);
    F.close_chunk		();

	// header
	F.write_chunk		(DETMGR_CHUNK_HEADER,&m_Header,sizeof(DetailHeader));

    // objects
    SaveColorIndices	(F);

    // slots
	F.open_chunk		(DETMGR_CHUNK_SLOTS);
    F.Wdword			(m_Slots.size());
	F.write				(m_Slots.begin(),m_Slots.size()*sizeof(DetailSlot));
    F.close_chunk		();
    // internal
    // bbox
	F.write_chunk		(DETMGR_CHUNK_BBOX,&m_BBox,sizeof(Fbox));
	// base texture
    if (m_Base.Valid()){
		F.open_chunk	(DETMGR_CHUNK_BASE_TEXTURE);
    	F.WstringZ		(m_Base.GetName());
	    F.close_chunk	();
    }
    F.open_chunk		(DETMGR_CHUNK_DENSITY);
    F.Wfloat			(m_fDensity);
    F.close_chunk		();
	// snap objects
	F.open_chunk		(DETMGR_CHUNK_SNAP_OBJECTS);
    F.Wdword			(m_SnapObjects.size());
    for (ObjectIt o_it=m_SnapObjects.begin(); o_it!=m_SnapObjects.end(); o_it++)
    	F.WstringZ		((*o_it)->Name);
    F.close_chunk		();
}

bool CDetailManager::Export(LPCSTR fn){
    bool bRes=true;

    UI.ProgressStart	(4,"Making details...");
    m_Header.version	= DETAIL_VERSION;
	CFS_Memory F;
    m_Header.object_count=m_Objects.size();
	// header
	F.write_chunk		(DETMGR_CHUNK_HEADER,&m_Header,sizeof(DetailHeader));
	UI.ProgressInc();

    // objects
	F.open_chunk		(DETMGR_CHUNK_OBJECTS);
    for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
		F.open_chunk	(it-m_Objects.begin());
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
		F.write			(m_Slots.begin(),m_Slots.size()*sizeof(DetailSlot));
	    F.close_chunk	();
		UI.ProgressInc	();
    }

    if (bRes)			F.SaveTo(fn,0);

	UI.ProgressInc();
    UI.ProgressEnd		();
    return bRes;
}

//------------------------------------------------------------------------------
// SBase
//------------------------------------------------------------------------------
#define MAX_BUF_SIZE 0xFFFF

CDetailManager::SBase::SBase(){
	name[0]			= 0;
	shader_overlap	= 0;
    shader_blended	= 0;
}

bool CDetailManager::SBase::LoadImage(LPCSTR nm){
	strcpy(name,nm);
    ImageManager.LoadTextureData(nm,data,w,h);
    return Valid();
}

void CDetailManager::SBase::CreateRMFromObjects(const Fbox& box, ObjectList& lst)
{
	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
    	CSceneObject*	 S = (CSceneObject*)(*it);
    	CEditableObject* O = S->GetReference(); VERIFY(O);

        Fmatrix T; S->GetFullTransformToWorld(T);
        mesh.reserve	(mesh.size()+S->GetFaceCount()*3);
        for (EditMeshIt m_it=O->FirstMesh(); m_it!=O->LastMesh(); m_it++){
		    FaceVec&	faces 	= (*m_it)->GetFaces();
		    FvectorVec&	pts 	= (*m_it)->GetPoints();
        	for (FaceIt f_it=faces.begin(); f_it!=faces.end(); f_it++){
            	FVF::V v;
                for (int k=0; k<3; k++){
                	T.transform_tiny(v.p,pts[f_it->pv[k].pindex]);
					v.t.x = GetUFromX(v.p.x,box);
					v.t.y = GetVFromZ(v.p.z,box);
                    mesh.push_back(v);
                }
            }
        }
    }
//s	stream = Device.Streams.Create(FVF::F_V,MAX_BUF_SIZE);
}

void CDetailManager::SBase::Render()
{
	if (!Valid()) return;
    Device.RenderNearer(0.001f);
	Device.SetTransform(D3DTS_WORLD,Fidentity);
    Device.SetShader((fraBottomBar->miDrawDOBlended->Checked)?shader_blended:shader_overlap);
    div_t cnt = div(mesh.size(),MAX_BUF_SIZE);
    DWORD vBase;
    for (int k=0; k<cnt.quot; k++){
//s		LPBYTE pv = (LPBYTE)stream->Lock(MAX_BUF_SIZE,vBase);
//s		CopyMemory(pv,mesh.begin()+k*MAX_BUF_SIZE,sizeof(FVF::V)*MAX_BUF_SIZE);
//s		stream->Unlock(MAX_BUF_SIZE);
//s		Device.DP(D3DPT_TRIANGLELIST,stream,vBase,MAX_BUF_SIZE/3);
    }
    if (cnt.rem){
//s	    LPBYTE pv = (LPBYTE)stream->Lock(cnt.rem,vBase);
//s		CopyMemory(pv,mesh.begin()+cnt.quot*MAX_BUF_SIZE,sizeof(FVF::V)*cnt.rem);
//s	    stream->Unlock(cnt.rem);
//s    	Device.DP(D3DPT_TRIANGLELIST,stream,vBase,cnt.rem/3);
    }
    Device.ResetNearer();
}

void CDetailManager::SBase::CreateShader()
{
	if (Valid()){
		shader_blended = Device.Shader.Create("editor\\do_base",name);
		shader_overlap = Device.Shader.Create("default",name);
	}
}

void CDetailManager::SBase::DestroyShader()
{
	if (shader_blended) Device.Shader.Delete(shader_blended);
	if (shader_overlap) Device.Shader.Delete(shader_overlap);
}
