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
DetailSlot&	EDetailManager::GetSlot(DWORD sx, DWORD sz){
	VERIFY(sx<dtH.size_x);
	VERIFY(sz<dtH.size_z);
	return dtSlots[sz*dtH.size_x+sx];
}

void EDetailManager::FindClosestIndex(const Fcolor& C, SIndexDistVec& best){
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

bool EDetailManager::Initialize(LPCSTR tex_name){
	if (!fraLeftBar->ebEnableSnapList->Down||Scene.m_SnapObjects.empty()){
    	ELog.DlgMsg(mtError,"Fill snap list and activate before generating slots!");
    	return false;
    }

    if (objects.empty()){
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

bool EDetailManager::Reinitialize(){
	if (m_SnapObjects.empty()){
    	ELog.DlgMsg(mtError,"Snap list empty! Initialize at first.");
    	return false;
    }

    if (objects.empty()){
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

bool EDetailManager::UpdateHeader(){
    // get bounding box
	if (!Scene.GetBox(m_BBox,m_SnapObjects)) return false;

    // fill header
    int mn_x 			= iFloor(m_BBox.min.x/DETAIL_SLOT_SIZE);
    int mn_z 			= iFloor(m_BBox.min.z/DETAIL_SLOT_SIZE);
    int mx_x 			= iFloor(m_BBox.max.x/DETAIL_SLOT_SIZE)+1;
    int mx_z 			= iFloor(m_BBox.max.z/DETAIL_SLOT_SIZE)+1;
    dtH.offs_x 	= -mn_x;
    dtH.offs_z 	= -mn_z;
	dtH.size_x 	= mx_x-mn_x;
	dtH.size_z 	= mx_z-mn_z;
    return true;
}

bool EDetailManager::UpdateBaseTexture(LPCSTR tex_name){
    // create base texture
    if (!tex_name&&!m_Base.Valid()){
    	ELog.DlgMsg(mtError,"Initialize at first!");
    	return false;
    }
    AnsiString fn = tex_name?tex_name:m_Base.GetName();
    m_Base.Clear();
    m_Base.LoadImage(fn.c_str());
    if (!m_Base.Valid()){
	    m_Base.Clear();
    	ELog.DlgMsg(mtError,"Can't load base image '%s'!",fn.c_str());
    	return false;
    }
	m_Base.RecreateShader();
    m_Base.CreateRMFromObjects(m_BBox,m_SnapObjects);
    return true;
}

#define EPS_L_VAR 0.0012345f
void EDetailManager::UpdateSlotBBox(int sx, int sz, DetailSlot& slot){
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

bool EDetailManager::UpdateSlots()
{
	// clear previous slots
    _DELETEARRAY(dtSlots);
    dtSlots				= new DetailSlot[dtH.size_x*dtH.size_z];

    UI.ProgressStart	(dtH.size_x*dtH.size_z,"Updating bounding boxes...");
    for (DWORD z=0; z<dtH.size_z; z++){
        for (DWORD x=0; x<dtH.size_x; x++){
        	DetailSlot* slot = dtSlots+z*dtH.size_x+x;
        	slot->y_min	= m_BBox.min.y;
        	slot->y_max	= m_BBox.max.y;
        	UpdateSlotBBox(x,z,*slot);
			UI.ProgressInc();
        }
    }
    UI.ProgressEnd		();

    m_Selected.resize	(dtH.size_x*dtH.size_z);

    return true;
}

void EDetailManager::GetSlotRect(Frect& rect, int sx, int sz){
    float x 			= fromSlotX(sx);
    float z 			= fromSlotZ(sz);
    rect.x1				= x-DETAIL_SLOT_SIZE_2+EPS_L;
    rect.y1				= z-DETAIL_SLOT_SIZE_2+EPS_L;
    rect.x2				= x+DETAIL_SLOT_SIZE_2-EPS_L;
    rect.y2				= z+DETAIL_SLOT_SIZE_2-EPS_L;
}

void EDetailManager::GetSlotTCRect(Irect& rect, int sx, int sz){
	Frect R;
	GetSlotRect			(R,sx,sz);
	rect.x1 			= m_Base.GetPixelUFromX(R.x1,m_BBox);
	rect.x2 			= m_Base.GetPixelUFromX(R.x2,m_BBox);
	rect.y2 			= m_Base.GetPixelVFromZ(R.y1,m_BBox); // v - координата флипнута
	rect.y1 			= m_Base.GetPixelVFromZ(R.y2,m_BBox);
}

void EDetailManager::CalcClosestCount(int part, const Fcolor& C, SIndexDistVec& best){
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

BYTE EDetailManager::GetRandomObject(DWORD color_index){
	ColorIndexPairIt CI=m_ColorIndices.find(color_index);
	R_ASSERT(CI!=m_ColorIndices.end());
	int k = DetailRandom.randI(0,CI->second.size());
    DetailIt it = find(objects.begin(),objects.end(),CI->second[k]);
    VERIFY(it!=objects.end());
	return (it-objects.begin());
}

BYTE EDetailManager::GetObject(ColorIndexPairIt& CI, BYTE id){
	VERIFY(CI!=m_ColorIndices.end());
    DOIt it = find(objects.begin(),objects.end(),CI->second[id]);
    VERIFY(it!=objects.end());
	return (it-objects.begin());
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

bool EDetailManager::UpdateSlotObjects(int x, int z){
    srand(time(NULL));

    DetailSlot* slot	= dtSlots+z*dtH.size_x+x;
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
        float f = objects[slot->items[k].id]->m_fDensityFactor;

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

bool EDetailManager::UpdateObjects(bool bUpdateTex, bool bUpdateSelectedOnly){
    // reload base texture
    if (bUpdateTex&&!UpdateBaseTexture(0)) 	return false;
    // update objects
    UI.ProgressStart	(dtH.size_x*dtH.size_z,"Updating objects...");
    for (DWORD z=0; z<dtH.size_z; z++)
        for (DWORD x=0; x<dtH.size_x; x++){
        	if (!bUpdateSelectedOnly||(bUpdateSelectedOnly&&m_Selected[z*dtH.size_x+x]))
	        	UpdateSlotObjects(x,z);
		    UI.ProgressInc();
        }
    UI.ProgressEnd		();

    InvalidateCache		();

    return true;
}

CDetail* EDetailManager::FindObjectByName(LPCSTR name){
	for (DOIt it=objects.begin(); it!=objects.end(); it++)
    	if (stricmp((*it)->GetName(),name)==0) return *it;
    return 0;
}

void EDetailManager::MarkAllObjectsAsDel(){
	for (DOIt it=objects.begin(); it!=objects.end(); it++)
    	(*it)->m_bMarkDel = true;
}


CDetail* EDetailManager::AppendObject(LPCSTR name, bool bTestUnique)
{
    CDetail* D=0;
	if (bTestUnique&&(D=FindObjectByName(name))) return D;

    D = new CDetail();
    if (!D->Update(name)){
    	_DELETE(D);
        return 0;
    }
    objects.push_back(D);
	return D;
}

void EDetailManager::InvalidateSlots()
{
	int slot_cnt = dtH.size_x*dtH.size_z;
	for (int k=0; k<slot_cnt; k++){
    	DetailSlot* it = &dtSlots[k];
    	it->items[0].id = 0xff;
    	it->items[1].id = 0xff;
    	it->items[2].id = 0xff;
    	it->items[3].id = 0xff;
    }
    InvalidateCache();
}

int EDetailManager::RemoveObjects(bool bOnlyMarked)
{
	int cnt=0;
	if (bOnlyMarked){
		for (DWORD i=0; i<objects.size(); i++){  // не менять int i; на DWORD
    		if (objects[i]->m_bMarkDel){
            	_DELETE(objects[i]);
	            objects.erase(objects.begin()+i);
    	        i--;
                cnt++;
            }
        }
    }else{
		for (DOIt it=objects.begin(); it!=objects.end(); it++)
    		_DELETE(*it);
        cnt = objects.size();
	    objects.clear();
    }
    return cnt;
}

void EDetailManager::RemoveColorIndices(){
	m_ColorIndices.clear();
}

CDetail* EDetailManager::FindObjectInColorIndices(DWORD index, LPCSTR name)
{
	ColorIndexPairIt CI=m_ColorIndices.find(index);
	if (CI!=m_ColorIndices.end()){
    	DOVec& lst = CI->second;
		for (DOIt it=lst.begin(); it!=lst.end(); it++)
    		if (stricmp((*it)->GetName(),name)==0) return *it;
    }
    return 0;
}

void EDetailManager::AppendIndexObject(DWORD color,LPCSTR name, bool bTestUnique)
{
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


