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
#include "bottombar.h"

static Fvector down_vec	={0.f,-1.f,0.f};
static Fvector left_vec	={-1.f,0.f,0.f};
static Fvector right_vec={1.f,0.f,0.f};
static Fvector fwd_vec	={0.f,0.f,1.f};
static Fvector back_vec	={0.f,0.f,-1.f};
//------------------------------------------------------------------------------

#define DETOBJ_CHUNK_VERSION		0x1000
#define DETOBJ_CHUNK_REFERENCE 		0x0101
#define DETOBJ_CHUNK_SCALE_LIMITS	0x0102
#define DETOBJ_CHUNK_ID				0x0103

#define DETOBJ_VERSION 				0x0001
//------------------------------------------------------------------------------

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
	m_ID				= 0;
	m_pShader			= 0;
	m_dwFlags			= 0;
	m_fRadius			= 0;
	m_pRefs				= 0;
    m_bSideFlag			= false;
    m_bMarkDel			= false;
    m_fMinScale			= 0.5f;
    m_fMaxScale         = 2.f;
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
    m_bSideFlag			= surf->sideflag;

    // create shader
	if (m_pShader) 		UI->Device.Shader.Delete(m_pShader);
    m_pShader			= UI->Device.Shader.Create(surf->shader->shader->cName,surf->textures);
    R_ASSERT			(m_pShader);

    // fill geometry
    CEditMesh* M 		= *m_pRefs->FirstMesh();

	m_Vertices.resize	(M->GetVertexCount());
    m_Indices.resize	(M->GetFaceCount(false)*3);

    // fill vertices
    Fbox bbox;
    bbox.invalidate();
    DOVertIt v_it		= m_Vertices.begin();
    for (FvectorIt p_it=M->m_Points.begin(); p_it!=M->m_Points.end(); p_it++,v_it++){
		v_it->P.set		(*p_it);
        bbox.modify		(*p_it);
    }
    m_fRadius			= bbox.getradius();


    WORDIt i_it			= m_Indices.begin();
    for (FaceIt f_it=M->m_Faces.begin(); f_it!=M->m_Faces.end(); f_it++){
    	for (int k=0; k<3; k++){
        	// index
         	*i_it = f_it->pv[k].pindex;
            // uv
            st_VMapPt& vm=M->m_VMRefs[f_it->pv[k].vmref][0];
            Fvector2& uv= M->m_VMaps[vm.vmap_index].getUV(vm.index);
            m_Vertices[*i_it].u = uv.x;
            m_Vertices[*i_it].v = uv.y;
            i_it++;
        }
    }

    return true;
}

bool CDetail::Load(CStream& F){
	// check version
    R_ASSERT			(F.FindChunk(DETOBJ_CHUNK_VERSION));
    DWORD version		= F.Rdword();
    if (version!=DETOBJ_VERSION){
    	Log->Msg(mtError,"CDetail: unsupported version.");
        return false;
    }

	// references
	char buf[255];
    R_ASSERT			(F.FindChunk(DETOBJ_CHUNK_REFERENCE));
    F.RstringZ			(buf);

    // scale
    R_ASSERT			(F.FindChunk(DETOBJ_CHUNK_SCALE_LIMITS));
    m_fMinScale			= F.Rfloat();
	m_fMaxScale         = F.Rfloat();

	// ID
    R_ASSERT			(F.FindChunk(DETOBJ_CHUNK_ID));
    m_ID				= F.Rbyte();

    // update object
    return 				Update(buf);
}

void CDetail::Save(CFS_Base& F){
	// version
	F.open_chunk		(DETOBJ_CHUNK_VERSION);
    F.Wdword			(DETOBJ_VERSION);
    F.close_chunk		();

    // reference
	R_ASSERT			(m_pRefs);
	F.open_chunk		(DETOBJ_CHUNK_REFERENCE);
    F.WstringZ			(m_pRefs->GetName());
    F.close_chunk		();

	// scale
	F.open_chunk		(DETOBJ_CHUNK_SCALE_LIMITS);
    F.Wfloat			(m_fMinScale);
    F.Wfloat			(m_fMaxScale);
    F.close_chunk		();

	// ID
	F.open_chunk		(DETOBJ_CHUNK_ID);
    F.Wbyte				(m_ID);
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
    F.Wfloat			(m_fMinScale);
    F.Wfloat			(m_fMaxScale);

    F.Wdword			(m_Vertices.size());
    F.Wdword			(m_Indices.size());

    F.write				(m_Vertices.begin(), m_Vertices.size()*sizeof(fvfVertexIn));
    F.write				(m_Indices.begin(), m_Indices.size()*sizeof(WORD));
}

//------------------------------------------------------------------------------
CDetailManager::CDetailManager(){
	m_pBaseTexture 		= 0;
    m_pBaseShader		= 0;
    ZeroMemory			(&m_Header,sizeof(DetailHeader));
    m_Header.version	= DETAIL_VERSION;
    InitRender			();
}

CDetailManager::~CDetailManager(){
	Clear();
    m_Slots.clear		();
	m_Cache.clear		();
	m_Visible.clear		();
}

DetailSlot&	CDetailManager::GetSlot(DWORD sx, DWORD sz){
	VERIFY(sx<m_Header.size_x);
	VERIFY(sz<m_Header.size_z);
	return m_Slots[sz*m_Header.size_x+sx];
}

bool CDetailManager::GetColor(DWORD& color, int U, int V){
	return m_pBaseTexture->GetPixel(color, U,V);
}

DWORD CDetailManager::GetUFromX(float x){
	float u = (x-m_BBox.min.x)/(m_BBox.max.x-m_BBox.min.x);
	int U = iFloor(u*(m_pBaseTexture->width()-1)+0.5f);// 	U %= m_pBaseTexture->width();
    return U;
}

DWORD CDetailManager::GetVFromZ(float z){
	float v = 1.f-(z-m_BBox.min.z)/(m_BBox.max.z-m_BBox.min.z);
	int V = iFloor(v*(m_pBaseTexture->height()-1)+0.5f);//    V %= m_pBaseTexture->height();
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
    	int tpm			= 10; // 10 ������ �� ����
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

//+������ 	- Ymax, Ymin
// ����� 	- ������� 3 ����������
// �������	- ������� ��������� � ������
// �������	- �� ����� �������� ���������� ���������
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
	if (!UpdateBaseTexture(tex_name))	return false;
    if (!UpdateBBox()) 					return false;
    if (!UpdateObjects(false))			return false;
	return true;
}

bool CDetailManager::UpdateBaseTexture(LPCSTR tex_name){
    // create base texture
    R_ASSERT(tex_name||m_pBaseTexture);
    AnsiString fn = tex_name?tex_name:m_pBaseTexture->name();
    _DELETE(m_pBaseTexture);
    m_pBaseTexture = new ETextureCore(fn.c_str());
    if (!m_pBaseTexture->Valid()){
    	Log->DlgMsg(mtError,"Can't load base texture '%s'!",fn.c_str());
    	return false;
    }
    if (m_pBaseShader) UI->Device.Shader.Delete(m_pBaseShader);
    m_pBaseShader = UI->Device.Shader.Create("def_trans",fn.c_str(),false);
    UI->Command(COMMAND_REFRESH_TEXTURES);
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
            	for (DWORD k=0; k<sRes->size(); k++){
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
    for (DWORD z=0; z<m_Header.size_z; z++){
        for (DWORD x=0; x<m_Header.size_x; x++){
        	DSIt slot	= m_Slots.begin()+z*m_Header.size_x+x;
        	slot->y_min	= m_BBox.min.y;
        	slot->y_max	= m_BBox.max.y;
        	UpdateSlotBBox(x,z,*slot);
			UI->ProgressInc();
        }
    }
    UI->ProgressEnd		();
    return true;
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
	rect.y2 			= GetVFromZ(R.y1); // v - ���������� ��������
	rect.y1 			= GetVFromZ(R.y2);
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
    if (idx>=0) best[idx].density[part]++;
}

BYTE CDetailManager::GetRandomObject(DWORD color_index, CRandom& R){
	ColorIndexPairIt CI=m_ColorIndices.find(color_index);
	R_ASSERT(CI!=m_ColorIndices.end());
	int k = R.randI(0,CI->second.size());
	return CI->second[k]->m_ID;
}

bool CompareWeightFunc(SIndexDist& d0, SIndexDist& d1){
	return d0.dist<d1.dist;
};

bool CDetailManager::UpdateObjects(bool bUpdateTex){
    // reload base texture
    if (bUpdateTex&&!UpdateBaseTexture(0)) 	return false;
    // update objects
    CRandom SlotRandom(0x26111975);
    UI->ProgressStart	(m_Header.size_x*m_Header.size_z,"Updating slot objects...");
    for (DWORD z=0; z<m_Header.size_z; z++){
        for (DWORD x=0; x<m_Header.size_x; x++){
        	DSIt slot	= m_Slots.begin()+z*m_Header.size_x+x;
            Irect		R;
            GetSlotTCRect(R,x,z);
            SIndexDistVec best;
            // find best color index
            {
                for (int v=R.y1; v<=R.y2; v++){
                    for (int u=R.x1; u<=R.x2; u++){
                    	DWORD clr;
                    	if (GetColor(clr,u,v)){
	                        Fcolor C;
    	                    C.set(clr);
        	                FindClosestIndex(C,best);
                        }
                    }
                }
            }
            sort(best.begin(),best.end(),CompareWeightFunc);
            // ������� �� 4 ������ ����� � ��������� ��������� ���������� (������ ��������� V)
            Irect P[4];
            float dx=float(R.x2-R.x1)/2.f;
            float dy=float(R.y2-R.y1)/2.f;
            P[0].x1=R.x1; 		  		P[2].y1=iFloor(R.y1+dy); 	P[0].x2=iFloor(R.x1+dx+.5f);	P[2].y2=R.y2;
            P[1].x1=iFloor(R.x1+dx);	P[3].y1=iFloor(R.y1+dy);	P[1].x2=R.x2; 					P[3].y2=R.y2;
            P[2].x1=R.x1; 		  		P[0].y1=R.y1; 		  		P[2].x2=iFloor(R.x1+dx+.5f); 	P[0].y2=iFloor(R.y1+dy+.5f);
            P[3].x1=iFloor(R.x1+dx); 	P[1].y1=R.y1;		  		P[3].x2=R.x2; 					P[1].y2=iFloor(R.y1+dx+.5f);
            for (int part=0; part<4; part++){
		        float	alpha=0;
                int 	cnt=0;
	            for (int v=P[part].y1; v<P[part].y2; v++){
		            for (int u=P[part].x1; u<P[part].x2; u++){
                    	DWORD clr;
                    	if (GetColor(clr,u,v)){
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
	                best[i].density[part] = cnt?(best[i].density[part]*alpha)/float(cnt):0;
            }

            // fill empty slots
            R_ASSERT(best.size());
            int id=0;
            DWORD o_cnt=0;
            for (DWORD i=0; i<best.size(); i++)
				o_cnt+=m_ColorIndices[best[i].index].size();
            // ���������� ��������� ������ �����
			if (o_cnt>best.size()){
                while (best.size()<4){
                    if (m_ColorIndices[best[id].index].size()>1){
                        best.push_back(SIndexDist());
                        best.back()=best[id];
                    }
                    if (best.size()==o_cnt) break;
                }
            }

            // �������� ������� � ��������� Random'�
            for(DWORD k=0; k<best.size(); k++){
				slot->items[k].palette.a0 	= iFloor(best[k].density[0]*15);
				slot->items[k].palette.a1 	= iFloor(best[k].density[1]*15);
				slot->items[k].palette.a2 	= iFloor(best[k].density[2]*15);
				slot->items[k].palette.a3 	= iFloor(best[k].density[3]*15);
				bool bReject;
                do{
                	bReject					= false;
	                slot->items[k].id       = GetRandomObject(best[k].index,SlotRandom);
                    for (int j=0; j<k; j++)
						if (slot->items[j].id==slot->items[k].id){bReject=true; break;}
                }while(bReject);
                slot->color					= 0xffffffff;
            }
			slot->r_yaw 	= SlotRandom.randIs(int_max);
			slot->r_scale 	= SlotRandom.randIs(int_max);
            // ��������� ID ������������� ������ ��� ��������
            for(k=best.size(); k<4; k++)
            	slot->items[k].id = 0xff;
			UI->ProgressInc();
        }
    }
    UI->ProgressEnd		();

    InvalidateCache();
    
    return true;
}

void CDetailManager::Render(ERenderPriority flag){
	if (m_Slots.size()){
    	switch (flag){
		case rpNormal:{
        	if (fraBottomBar->miDrawDOSlotBoxes->Checked){
                Fvector 		center;
                Fbox			bbox;
                UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);
                UI->Device.Shader.Set(UI->Device.m_WireShader);
                for (DWORD z=0; z<m_Header.size_z; z++){
                    center.z	= fromSlotZ(z);
                    for (DWORD x=0; x<m_Header.size_x; x++){
                        DSIt slot	= m_Slots.begin()+z*m_Header.size_x+x;
                        center.x	= fromSlotX(x);
                        center.y	= (slot->y_max+slot->y_min)/2;
                        bbox.min.set(center.x-DETAIL_SLOT_SIZE_2, slot->y_min, center.z-DETAIL_SLOT_SIZE_2);
                        bbox.max.set(center.x+DETAIL_SLOT_SIZE_2, slot->y_max, center.z+DETAIL_SLOT_SIZE_2);
                        DU::DrawSelectionBox(bbox);
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

void CDetailManager::Clear(){
	RemoveObjects		();
	m_ColorIndices.clear();
    if (m_pBaseShader){ UI->Device.Shader.Delete(m_pBaseShader); m_pBaseShader = 0;}
    _DELETE				(m_pBaseTexture);
}


CDetail* CDetailManager::FindObjectByName(LPCSTR name){
	for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	if (stricmp((*it)->GetName(),name)==0) return *it;
    return 0;
}

bool CompareIDFunc(CDetail* d0, CDetail* d1){
	return d0->m_ID<d1->m_ID;
};

void CDetailManager::SortObjectsByID(){
	sort(m_Objects.begin(),m_Objects.end(),CompareIDFunc);
}

BYTE CDetailManager::FindEmptyID(){
	BYTE ID = m_Objects.size();
    SortObjectsByID();
    if (ID&&(m_Objects.back()->m_ID>=ID)){
    	int id=-1;
        for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
            BYTE d=(*it)->m_ID-id;
            if (d>1){
                ID = id+1;
                break;
            }
            id = (*it)->m_ID;
        }
    }
    return ID;
}

void CDetailManager::MarkAllObjectsAsDel(){
	for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	(*it)->m_bMarkDel = true;
}


CDetail* CDetailManager::AppendObject(LPCSTR name, bool bTestUnique){
    CDetail* D=0;
	if (bTestUnique&&(D=FindObjectByName(name))) return D;

    D = new CDetail();
    DWORD ID = bTestUnique?FindEmptyID():m_Objects.size();
    R_ASSERT(ID<256);
    D->m_ID = ID;
    if (!D->Update(name)){
    	_DELETE(D);
        return 0;
    }
    m_Objects.push_back(D);
	return D;
}

void CDetailManager::RemoveObjects(bool bOnlyMarked){
	if (bOnlyMarked){
		for (int i=0; i<m_Objects.size(); i++){
    		if (m_Objects[i]->m_bMarkDel){
            	_DELETE(m_Objects[i]);
	            m_Objects.erase(m_Objects.begin()+i);
    	        i--;
            }
        }
    }else{
		for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    		_DELETE(*it);
	    m_Objects.clear();
    }
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
    int cnt 			= F.Rdword();
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
	    m_pBaseShader 		= UI->Device.Shader.Create("def_trans",m_pBaseTexture->name(),false);
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
            if (DO) 	m_ColorIndices[index].push_back(DO);
        }
    }

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

void CDetailManager::Export(LPCSTR fn){
	CFS_Memory F;
    m_Header.object_count=m_Objects.size();
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

    F.SaveTo			(fn,0);
}

