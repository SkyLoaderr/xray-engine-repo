//----------------------------------------------------
// file: DetailObjects.h
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "DetailObjects.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "EditMesh.h"
#include "EditObject.h"
#include "Texture.h"
#include "D3DUtils.h"
#include "Scene.h"
#include "EditorPref.h"
#include "leftbar.h"
#include "cl_intersect.h"

#include "Library.h"
#include "DetailFormat.h"
#include "XRShader.h"
#include "Texture.h"
#include "frustum.h"

static Fvector down_vec	={0.f,-1.f,0.f};
static Fvector left_vec	={-1.f,0.f,0.f};
static Fvector right_vec={1.f,0.f,0.f};
static Fvector fwd_vec	={0.f,0.f,1.f};
static Fvector back_vec	={0.f,0.f,-1.f};
//------------------------------------------------------------------------------

#define DETOBJ_CHUNK_REFERENCE 		0x0100

#define DETMGR_CHUNK_VERSION		0x1000

#define DETMGR_CHUNK_HEADER 		0x0000
#define DETMGR_CHUNK_OBJECTS 		0x0001
#define DETMGR_CHUNK_SLOTS			0x0002
#define DETMGR_CHUNK_BBOX			0x1001
#define DETMGR_CHUNK_BASE_TEXTURE	0x1002
#define DETMGR_CHUNK_COLOR_INDEX 	0x1003

#define DETMGR_VERSION 				0x0001
//------------------------------------------------------------------------------
CDetail::CDetail(){
	m_pShader			= 0;
	m_dwFlags			= 0;
	m_fRadius			= 0;
	m_pRefs				= 0;
}

CDetail::~CDetail(){
	if (m_pShader) 		UI->Device.Shader.Delete(m_pShader);
	m_Vertices.clear	();
}

bool CDetail::Update	(LPCSTR name){
    // update link
    CLibObject* LO 		= Lib->SearchObject(name);
    if (!LO){
        Log->DlgMsg		(mtError, "CDetail: '%s' not found in library", name);
        return false;
    }
    if (0==(m_pRefs=LO->GetReference())){
        Log->DlgMsg		(mtError, "CDetail: '%s' can't load", name);
        return false;
    }
    if(m_pRefs->SurfaceCount()!=1){
    	Log->DlgMsg		(mtError,"Object must contain 1 material.");
    	return false;
    }
	if(m_pRefs->MeshCount()==0){
    	Log->DlgMsg		(mtError,"Object must contain 1 mesh.");
    	return false;
    }

    // get surface
    st_Surface* surf	= *m_pRefs->FirstSurface();
    R_ASSERT			(surf);

    // create shader
	if (m_pShader) 		UI->Device.Shader.Delete(m_pShader);
    m_pShader			= UI->Device.Shader.Create(surf->shader->shader->cName,surf->textures);
    R_ASSERT			(m_pShader);

    // create mesh
    CEditMesh* M 		= *m_pRefs->FirstMesh();

	m_Vertices.resize	(M->GetFaceCount(true)*3);
    DOVertIt v_it		= m_Vertices.begin();
    for (FaceIt it=M->m_Faces.begin(); it!=M->m_Faces.end(); it++){
    	for (int k=0; k<3; k++){
			v_it->P.set	(M->m_Points[it->pv[k].pindex]);
            st_VMapPt& vm=M->m_VMRefs[it->pv[k].vmref][0];
            Fvector2& uv= M->m_VMaps[vm.vmap_index].getUV(vm.index);
			v_it->u		= uv.x;
			v_it->v		= uv.y;
            v_it++;
        }
        if (surf->sideflag){
            for (k=0; k<3; k++){
                v_it->set(*(v_it-(k*2+1)));
                v_it++;
            }
        }
    }
    return true;
}

bool CDetail::Load(CStream& F){
	char buf[255];
	// references
    R_ASSERT			(F.FindChunk(DETOBJ_CHUNK_REFERENCE));
    F.RstringZ			(buf);

    // update object
    return 				Update(buf);
}

void CDetail::Save(CFS_Base& F){
	R_ASSERT			(m_pRefs);
	F.open_chunk		(DETOBJ_CHUNK_REFERENCE);
    F.WstringZ			(m_pRefs->GetName());
    F.close_chunk		();
}

void CDetail::Export(CFS_Base& F){
	R_ASSERT			(m_pRefs);
    st_Surface* surf	= *m_pRefs->FirstSurface();
	R_ASSERT			(surf);
    // write data
	F.WstringZ			(surf->shader->shader->cName);
	F.WstringZ			(surf->textures[0].c_str());

    F.Wdword			(m_dwFlags);
    F.Wdword			(m_Vertices.size());
    F.write				(m_Vertices.begin(), m_Vertices.size()*sizeof(fvfVertexIn));
}

//------------------------------------------------------------------------------
CDetailManager::CDetailManager(){
	m_pBaseTexture 		= 0;
    ZeroMemory			(&m_Header,sizeof(DetailHeader));
    m_Header.version	= DETAIL_VERSION;
}

CDetailManager::~CDetailManager(){
	Clear();
}

DWORD CDetailManager::GetColor(float x, float z){
	return m_pBaseTexture->GetPixel(GetUFromX(x),GetVFromZ(z));
}

DWORD CDetailManager::GetColor(DWORD U, DWORD V){
	return m_pBaseTexture->GetPixel(U,V);
}

DWORD CDetailManager::GetUFromX(float x){
	float u = (x-m_BBox.min.x)/(m_BBox.max.x-m_BBox.min.x);
	int U = iFloor(u*(m_pBaseTexture->width()-1)+0.5f); 	U %= m_pBaseTexture->width();
    return U;
}

DWORD CDetailManager::GetVFromZ(float z){
	float v = (z-m_BBox.min.z)/(m_BBox.max.z-m_BBox.min.z);
	int V = iFloor(v*(m_pBaseTexture->height()-1)+0.5f);
    V %= m_pBaseTexture->height();
    return V;
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
            for (int k=0; k<best.size(); k++){
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

//static BYTEVec alpha;
/*
void CDetailManager::GenerateOneSlot(int sx, int sz, DetailSlot& slot){
	Fvector 			center;
    Fbox				bbox;
    center.x 			= fromSlotX(sx);
    center.y 			= (slot.y_max+slot.y_min)/2;
    center.z 			= fromSlotZ(sz);
    bbox.min.set		(center.x-DETAIL_SLOT_SIZE_2, slot.y_min, center.z-DETAIL_SLOT_SIZE_2);
    bbox.max.set		(center.x+DETAIL_SLOT_SIZE_2, slot.y_max, center.z+DETAIL_SLOT_SIZE_2);

    SBoxPickInfoVec pinf;
    if (Scene->BoxPick(bbox,pinf,true)){
    	int tpm			= 10; // 10 тестов на метр
    	float 	delta 	= 1/float(tpm); // interval 10 cm
//	    alpha.resize	((DETAIL_SLOT_SIZE*tpm)*(DETAIL_SLOT_SIZE*tpm));
        Fvector P;
        P.y				= slot.y_max;
        slot.y_min		= flt_max;
        slot.y_max		= flt_min;
        int i=0;

        IDS_count		= 0;

		for (P.z=bbox.min.z; P.z<bbox.max.z; P.z+=delta){
    		for (P.x=bbox.min.x; P.x<bbox.max.x; P.x+=delta,i++){
                float 	H;
                Fcolor	color;
                if (TestTris(H,P,pinf)){
                	// calc bbox
                    if (H>slot.y_max) slot.y_max = H;
                    if (H<slot.y_min) slot.y_min = H;
                    // calc nearest color(object) index
					color.set(GetColor(P.x,P.z));
                    float dist;
                    DWORD index;
                    FindClosestApproach(color);
                }
//                alpha[i]= ;
            }
        }

//+высоты 	- Ymax, Ymin
// цвета 	- выбрать 3 сильнейших
// объекты	- выбрать ближайшие в списке
// палитра	- по альфе текстуры определить плотность
    }else{
    	ZeroMemory(&slot,sizeof(DetailSlot));
    	slot.items[0].id=0xff;
    	slot.items[1].id=0xff;
    	slot.items[2].id=0xff;
    	slot.items[3].id=0xff;
    }
}
*/

bool CDetailManager::GenerateSlots(LPCSTR tex_name){
	if (!fraLeftBar->ebEnableSnapList->Down||Scene->m_SnapObjects.empty()){
    	Log->DlgMsg(mtError,"Fill snap list and activate before generating slots!");
    	return false;
    }

    if (m_Objects.empty()){
    	Log->DlgMsg(mtError,"Fill object list before generating slots!");
    	return false;
    }

    // create base texture
    R_ASSERT(tex_name);
    _DELETE(m_pBaseTexture);
    m_pBaseTexture = new ETextureCore(tex_name);
    if (!m_pBaseTexture->Valid()){
    	Log->DlgMsg(mtError,"Can't load base texture '%s'!",tex_name);
    	return false;
    }

    UpdateBBox();

	return true;
}

void CDetailManager::UpdateSlotBBox(int sx, int sz, DetailSlot& slot){
	Fbox bbox;
    Frect rect;
    GetSlotRect			(rect,sx,sz);
    bbox.min.set		(rect.x1, slot.y_min, rect.y1);
    bbox.max.set		(rect.x2, slot.y_max, rect.y2);

    SBoxPickInfoVec pinf;
    if (Scene->BoxPick(bbox,pinf,true)){
		bbox.grow		(EPS_L);
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
    		float range;
            sPoly sSrc	(it->bp_inf.p,3);
            sPoly sDest;
            sPoly* sRes = frustum.ClipPoly(sSrc, sDest);
            if (sRes){
            	for (int k=0; k<sRes->size(); k++){
                	float H = (*sRes)[k].y;
                    if (H>slot.y_max) slot.y_max = H;
                    if (H<slot.y_min) slot.y_min = H;
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

bool CDetailManager::UpdateBBox(){
    // get bounding box
	ObjectList& lst=Scene->m_SnapObjects;
	if (!Scene->GetBox(m_BBox,lst)) return false;

    // fill header
    int mn_x 			= iFloor(m_BBox.min.x/DETAIL_SLOT_SIZE+0.5f);
    int mn_z 			= iFloor(m_BBox.min.z/DETAIL_SLOT_SIZE+0.5f);
    int mx_x 			= iFloor(m_BBox.max.x/DETAIL_SLOT_SIZE+0.5f);
    int mx_z 			= iFloor(m_BBox.max.z/DETAIL_SLOT_SIZE+0.5f);
    m_Header.offs_x 	= -mn_x;
    m_Header.offs_z 	= -mn_z;
	m_Header.size_x 	= mx_x-mn_x;
	m_Header.size_z 	= mx_z-mn_z;

    m_Slots.clear		();
    m_Slots.resize		(m_Header.size_x*m_Header.size_z);

    UI->ProgressStart	(m_Header.size_x*m_Header.size_z,"Updating bbox...");
    for (int z=0; z<m_Header.size_z; z++){
        for (int x=0; x<m_Header.size_x; x++){
        	DSIt slot	= m_Slots.begin()+z*m_Header.size_x+x;
        	slot->y_min	= m_BBox.min.y;
        	slot->y_max	= m_BBox.max.y;
        	UpdateSlotBBox(x,z,*slot);
			UI->ProgressInc();
        }
    }
    UI->ProgressEnd		();
}

void CDetailManager::GetSlotRect(Frect& rect, int sx, int sz){
    float x 			= fromSlotX(sx);
    float z 			= fromSlotZ(sz);
    rect.x1				= x-DETAIL_SLOT_SIZE_2;
    rect.y1				= z-DETAIL_SLOT_SIZE_2;
    rect.x2				= x+DETAIL_SLOT_SIZE_2;
    rect.y2				= z+DETAIL_SLOT_SIZE_2;
}

void CDetailManager::GetSlotTCRect(Irect& rect, int sx, int sz){
	Frect R;
	GetSlotRect			(R,sx,sz);
	rect.x1 			= GetUFromX(R.x1);
	rect.x2 			= GetUFromX(R.x2);
	rect.y1 			= GetVFromZ(R.y1);
	rect.y2 			= GetVFromZ(R.y2);
}

void CDetailManager::CalcClosestCount(int part, const Fcolor& C, SIndexDistVec& best){
    float dist = flt_max;
    Fcolor src;
    float inv_a = 1-C.a;
    int idx = -1;

    for (int k=0; k<best.size(); k++){
		src.set(best[k].index);
        float d = inv_a+sqrtf((C.r-src.r)*(C.r-src.r)+(C.g-src.g)*(C.g-src.g)+(C.b-src.b)*(C.b-src.b));
        if (d<dist){
        	dist 	= d;
            idx 	= k;
        }
    }
    if (idx>=0) best[idx].count[part]++;
}

bool CDetailManager::UpdateObjects(){
    UI->ProgressStart	(m_Header.size_x*m_Header.size_z,"Updating slot objects...");
    for (int z=0; z<m_Header.size_z; z++){
        for (int x=0; x<m_Header.size_x; x++){
        	DSIt slot	= m_Slots.begin()+z*m_Header.size_x+x;
            Irect		R;
            GetSlotTCRect(R,x,z);
            SIndexDistVec best;
            // find best color index
            {
                for (DWORD v=R.y1; v<R.y2; v++){
                    for (DWORD u=R.x1; u<R.x2; u++){
                        Fcolor C;
                        C.set(GetColor(u,v));
                        FindClosestIndex(C,best);
                    }
                }
            }
            // sum
            Irect P[4];
            float dx=(R.x2-R.x1)/2;
            float dy=(R.y2-R.y1)/2;
            P[0].x1=R.x1; 					P[0].y1=R.y1; 					P[0].x2=R.x1+dx; 	P[0].y2=R.y1+dy;
            P[1].x1=iFloor(R.x1+dx+0.5f); 	P[1].y1=R.y1;					P[1].x2=R.x2; 		P[1].y2=R.y1+dx;
            P[2].x1=R.x1; 					P[2].y1=iFloor(R.y1+dy+0.5f); 	P[2].x2=R.x1+dx;	P[2].y2=R.y2;
            P[3].x1=iFloor(R.x1+dx+0.5f);	P[3].y1=iFloor(R.y1+dy+0.5f);	P[3].x2=R.x2; 		P[3].y2=R.y2;
            for (int k=0; k<4; k++){
	            for (DWORD v=P[k].y1; v<P[k].y2; v++){
		            for (DWORD u=P[k].x1; u<P[k].x2; u++){
                        Fcolor C;
                        C.set(GetColor(u,v));
                        CalcClosestCount(k,C,best);
                    }
                }
            }
			UI->ProgressInc();
        }
    }
    UI->ProgressEnd		();
}

void CDetailManager::Render(ERenderPriority flag){
	if (m_Slots.size()){
    	switch (flag){
		case rpNormal:{
            Fvector 		center;
            Fbox			bbox;
			UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);
			UI->Device.Shader.Set(UI->Device.m_WireShader);
		    for (int z=0; z<m_Header.size_z; z++){
            	center.z	= fromSlotZ(z);
        		for (int x=0; x<m_Header.size_x; x++){
		        	DSIt slot	= m_Slots.begin()+z*m_Header.size_x+x;
		            center.x	= fromSlotX(x);
					center.y	= (slot->y_max+slot->y_min)/2;
            		bbox.min.set(center.x-DETAIL_SLOT_SIZE_2, slot->y_min, center.z-DETAIL_SLOT_SIZE_2);
		            bbox.max.set(center.x+DETAIL_SLOT_SIZE_2, slot->y_max, center.z+DETAIL_SLOT_SIZE_2);
    	            DU::DrawSelectionBox(bbox);
                }
            }
        }break;
		case rpAlphaNormal: break;
        }
    }
}

void CDetailManager::Clear(){
	RemoveObjects		();
	m_ColorIndices.clear();
    _DELETE				(m_pBaseTexture);
    m_Slots.clear		();
}


CDetail* CDetailManager::FindObjectByName(LPCSTR name){
	for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	if (stricmp((*it)->GetName(),name)==0) return *it;
}

void CDetailManager::AppendObject(LPCSTR name, bool bTestUnique){
	if (bTestUnique&&FindObjectByName(name)) return;

    CDetail* D = new CDetail();
    if (!D->Update(name)){
    	_DELETE(D);
        return;
    }
    m_Objects.push_back(D);
}

void CDetailManager::RemoveObjects(){
	for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	_DELETE(*it);
    m_Objects.clear();
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

bool CDetailManager::Load(CStream& F){
    R_ASSERT			(F.FindChunk(DETMGR_CHUNK_VERSION));
	DWORD version		= F.Rdword();
    if (version!=DETMGR_VERSION){
    	Log->Msg(mtError,"CDetailManager: unsupported version.");
        return false;
    }

	// header
    R_ASSERT			(F.ReadChunk(DETMGR_CHUNK_HEADER,&m_Header));

    // objects
    CStream* OBJ 		= F.OpenChunk(DETMGR_CHUNK_OBJECTS);
    if (OBJ){
        CStream* O   	= OBJ->OpenChunk(0);
        for (int count=1; O; count++) {
            CDetail* DO = new CDetail();
            if (DO->Load(*O)){
                m_Objects.push_back(DO);
            }else{
                Log->Msg(mtError,"Can't load detail object.");
                _DELETE(DO);
            }
            O->Close();
            O = OBJ->OpenChunk(count);
        }
        OBJ->Close();
    }

    // slots
    R_ASSERT			(F.FindChunk(DETMGR_CHUNK_SLOTS));
    DWORD cnt 			= F.Rdword();
    m_Slots.resize		(cnt);
	F.Read				(m_Slots.begin(),m_Slots.size()*sizeof(DetailSlot));

    // internal
    // bbox
    R_ASSERT			(F.ReadChunk(DETMGR_CHUNK_BBOX,&m_BBox));
	// base texture
    char buf[255];
	if(F.FindChunk(DETMGR_CHUNK_BASE_TEXTURE)){
	    F.RstringZ			(buf);
    	m_pBaseTexture		= new ETextureCore(buf);
    }
    // color index map
    R_ASSERT			(F.FindChunk(DETMGR_CHUNK_COLOR_INDEX));
    cnt					= F.Rbyte();
    DWORD index;
    int ref_cnt;
    for (int k=0; k<cnt; k++){
		index			= F.Rdword();
        ref_cnt			= F.Rbyte();
		for (int j=0; j<ref_cnt; j++){
        	F.RstringZ	(buf);
            CDetail* DO	= FindObjectByName(buf);
            R_ASSERT	(DO);
        	m_ColorIndices[index].push_back(DO);
        }
    }
}

void CDetailManager::Save(CFS_Base& F){
	// version
	F.open_chunk		(DETMGR_CHUNK_VERSION);
    F.Wdword			(DETMGR_VERSION);
    F.close_chunk		();

	// header
	F.write_chunk		(DETMGR_CHUNK_HEADER,&m_Header,sizeof(DetailHeader));
    // objects
	F.open_chunk		(DETMGR_CHUNK_OBJECTS);
    for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
		F.open_chunk	(it-m_Objects.begin());
        (*it)->Save		(F);
	    F.close_chunk	();
    }
    F.close_chunk		();
    // slots
	F.open_chunk		(DETMGR_CHUNK_SLOTS);
    F.Wdword			(m_Slots.size());
	F.write				(m_Slots.begin(),m_Slots.size()*sizeof(DetailSlot));
    F.close_chunk		();

    // internal
    // bbox
	F.write_chunk		(DETMGR_CHUNK_BBOX,&m_BBox,sizeof(Fbox));
	// base texture
    if (m_pBaseTexture){
		F.open_chunk	(DETMGR_CHUNK_BASE_TEXTURE);
    	F.WstringZ		(m_pBaseTexture->name());
	    F.close_chunk	();
    }
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

void CDetailManager::Export(CFS_Base& F){
	// header
	F.write_chunk		(DETMGR_CHUNK_HEADER,&m_Header,sizeof(DetailHeader));
    // objects
	F.open_chunk		(DETMGR_CHUNK_OBJECTS);
    for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
		F.open_chunk	(it-m_Objects.begin());
        (*it)->Export	(F);
	    F.close_chunk	();
    }
    F.close_chunk		();
    // slots
	F.open_chunk		(DETMGR_CHUNK_SLOTS);
	F.write				(m_Slots.begin(),m_Slots.size()*sizeof(DetailSlot));
    F.close_chunk		();
}

