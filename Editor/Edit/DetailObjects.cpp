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

static Fvector down_vec={0.f,-1.f,0.f};

#define DOALT_QUANT 					63.f

#define DOCLUSTER_VERSION				0x0010
//----------------------------------------------------
#define DOCLUSTER_CHUNK_VERSION			0xE701
#define DOCLUSTER_CHUNK_POINT			0xE702
#define DOCLUSTER_CHUNK_LINKS			0xE703

#define DO_PACK(a,mn,sz)((a-mn)/sz*DOALT_QUANT)
#define DO_UNPACK(a,mn,sz)(float(a)*sz/DOALT_QUANT+mn)
//------------------------------------------------------------------------------
// DO Cluster
//------------------------------------------------------------------------------
CDOCluster::CDOCluster(char *name):SceneObject(){
	Construct();
	strcpy( m_Name, name );
}

CDOCluster::CDOCluster():SceneObject(){
	Construct();
}

void CDOCluster::Construct(){
	m_ClassID   = OBJCLASS_DOCLUSTER;
	m_Position.set(0,0,0);
}

CDOCluster::~CDOCluster(){
}
//----------------------------------------------------
class FindSimilar{
	CDOCluster* obj;
public:
    FindSimilar(CDOCluster* cl):obj(cl){;}
	IC bool operator()(ObjectIt it) { 
		return (fsimilar(((CDOCluster*)*it)->Position().x,obj->Position().x,EPS_L)&&
        		fsimilar(((CDOCluster*)*it)->Position().z,obj->Position().z,EPS_L));
    }
};
//----------------------------------------------------
bool CDOCluster::AppendCluster(int density){
	m_Position.x = snapto(m_Position.x,Scene->m_LevelOp.m_DOClusterSize);
	m_Position.z = snapto(m_Position.z,Scene->m_LevelOp.m_DOClusterSize);
    ObjectList lst;
    if (Scene->GetQueryObjects_if(lst,OBJCLASS_DOCLUSTER,FindSimilar(this))){
    	Log->Msg(mtError,"Cluster already occupied.");
    	return false;
    }

	ChangeDensity(density);
    Update();
	return true;
}
//----------------------------------------------------
static FloatVec alt;
static DWORDVec clr;
IC bool TestTris(CEditMesh*& M, int& id, float& h, float& u, float& v, const Fvector& start, SBoxPickInfoVec& tris){
	h	= flt_max;
    id	= -1;
	for (SBoxPickInfoIt it=tris.begin(); it!=tris.end(); it++){
    	float range;
		if (RAPID::TestRayTri(start,down_vec,it->bp_inf.p,u,v,range,true))
        	if (range<h){
            	h	= range;
                id 	= it->bp_inf.id;
                M	= it->mesh;
            }
    }
    if (id>=0){
    	h = start.y-h;
		return true;
    }
    return false;
}
void CDOCluster::Update(){
	Fvector pos;
    int cnt = Scene->m_LevelOp.m_DOClusterSize*m_Density;
    int cnt2 = cnt*cnt;
    float cl_size = Scene->m_LevelOp.m_DOClusterSize;
    float delta = cl_size/cnt;
    float offs = frmEditorPreferences->seDORayPickHeight->Value;
    alt.resize(cnt2);
    clr.resize(cnt2);
    m_MinHeight = flt_max;
    m_MaxHeight = flt_min;
    pos.x = m_Position.x-cl_size*0.5f+delta*.5f;

    //
    Fbox bb;
    SBoxPickInfoVec pinf;
    float h_size = cl_size*0.5f;
    bb.set(m_Position,m_Position);
    bb.min.x-=h_size; bb.min.y-=offs; bb.min.z-=h_size;
    bb.max.x+=h_size; bb.max.y+=offs; bb.max.z+=h_size;

    bool bPick = !!Scene->BoxPick(bb,pinf,fraLeftBar->ebEnableSnapList->Down);

    for (int x=0; x<cnt; x++,pos.x+=delta){
	    pos.z = m_Position.z-h_size+delta*0.5f;
	    for (int z=0; z<cnt; z++,pos.z+=delta){
        	pos.y = m_Position.y+offs;
			float H,U,V;
            int id;
            CEditMesh* M;
            if (bPick&&TestTris(M,id,H,U,V,pos,pinf)){
				clr[x*cnt+z]=DetermineColor(M,id,U,V);
                alt[x*cnt+z]=H;
                if (H>m_MaxHeight) m_MaxHeight = H;
                if (H<m_MinHeight) m_MinHeight = H;
            }else{
				alt[x*cnt+z]=0;
            }
        }
    }
    float sz = m_MaxHeight-m_MinHeight+EPS;
    for (int k=0; k<cnt2; k++) m_Objects[k].m_Alt = DO_PACK(alt[k],m_MinHeight,sz);
}
//----------------------------------------------------
DWORD CDOCluster::DetermineColor(CEditMesh* M, int id, float u, float v){
    // barycentric coords
    // note: W,U,V order
    Fvector B;
    B.set(1.0f - u - v,u,v);

    // calc UV
    const Fvector2* TC[3];
    st_Surface* surf = M->GetFaceTC(id,TC);
    Fvector2 uv;
    uv.x = TC[0]->x*B.x + TC[1]->x*B.y + TC[2]->x*B.z;
    uv.y = TC[0]->y*B.x + TC[1]->y*B.y + TC[2]->y*B.z;

    AnsiString& t_name = surf->textures[0];
    VERIFY(!t_name.IsEmpty());
    ETextureCore* tex = UI->Device.Shader.FindTexture(t_name.c_str());
    VERIFY(tex);

    int U = tex->ConvertU(uv.x);
    int V = tex->ConvertV(uv.y);

	Fcolor SUM;
    DWORD c;
    int cnt=0;
    for (int dx=-1; dx<=1; dx++){
	    for (int dy=-1; dy<=1; dy++){
			if (tex->GetPixel(c,U+dx,V+dy)){
            	if (cnt==0){
                    SUM.r = RGBA_GETRED(c)/255.f;
                    SUM.g = RGBA_GETGREEN(c)/255.f;
                    SUM.b = RGBA_GETBLUE(c)/255.f;
                    SUM.a = RGBA_GETALPHA(c)/255.f;
                }else{
                    SUM.r += RGBA_GETRED(c)/255.f;
                    SUM.g += RGBA_GETGREEN(c)/255.f;
                    SUM.b += RGBA_GETBLUE(c)/255.f;
                    SUM.a += RGBA_GETALPHA(c)/255.f;
                }
                cnt++;
            }
        }
    }
    VERIFY(cnt);
    SUM.r/=cnt;
    SUM.g/=cnt;
    SUM.b/=cnt;
    SUM.a/=cnt;

    return SUM.get();
}
//----------------------------------------------------
void CDOCluster::ChangeDensity(int new_density){
    m_Density = new_density;
    int cnt = Scene->m_LevelOp.m_DOClusterSize*m_Density;
    m_Objects.resize(cnt*cnt);
}
//----------------------------------------------------
bool CDOCluster::GetBox( Fbox& box ){
	box.set( m_Position, m_Position );
	box.min.x -= Scene->m_LevelOp.m_DOClusterSize*0.5f;
	box.min.y = m_MinHeight;
	box.min.z -= Scene->m_LevelOp.m_DOClusterSize*0.5f;
	box.max.x += Scene->m_LevelOp.m_DOClusterSize*0.5f;
	box.max.y = m_MaxHeight;
	box.max.z += Scene->m_LevelOp.m_DOClusterSize*0.5f;
	return true;
}

static FLvertexVec V;

void CDOCluster::DrawCluster(Fcolor& c){
	Fvector pos,dir;
    dir.set(0.f,-1.f,0.f);
    float cl_size = Scene->m_LevelOp.m_DOClusterSize;
    int cnt = Scene->m_LevelOp.m_DOClusterSize*m_Density;
    int cnt2 = cnt*cnt;
    float delta = cl_size/cnt;

    FVF::L v;
    v.color = c.get();
    V.resize(cnt2,v);

    UI->Device.RenderNearer(0.001f);
    float sz = m_MaxHeight-m_MinHeight+EPS;
    pos.x = m_Position.x-cl_size*0.5f+delta*.5f;
    for (int x=0; x<cnt; x++,pos.x+=delta){
	    pos.z = m_Position.z-cl_size*0.5f+delta*.5f;
	    for (int z=0; z<cnt; z++,pos.z+=delta)
	    	V[x*cnt+z].p.set(pos.x,DO_UNPACK(m_Objects[x*cnt+z].m_Alt,m_MinHeight,sz),pos.z);
    }
	UI->Device.DP(D3DPT_POINTLIST,FVF::F_L,V.begin(),V.size());
    UI->Device.ResetNearer();
}
//----------------------------------------------------

void CDOCluster::DrawObjects(){
}
//----------------------------------------------------

void CDOCluster::Render( Fmatrix& parent, ERenderPriority flag ){
    if (flag==rpNormal){
    	st_LevelOptions& opt = Scene->m_LevelOp;
		if(UI->Device.m_Frustum.testSphere(m_Position,opt.m_DOClusterSize)){
        	float dist = frmEditorPreferences->seDOHideDistance->Value;
	        if(UI->Device.m_Camera.GetPosition().distance_to_sqr(m_Position)<=(dist*dist)){
    	        Fcolor c1; c1.set(1.f,1.f,0.f,1.f);
	            DrawCluster(c1);
    	    }
	        if(Selected()){
    	        Fbox bb; GetBox(bb);
        	    DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
	            DU::DrawSelectionBox(bb,&clr);
    	    }
        }
    }
//            DrawObjects(c2);
}
//----------------------------------------------------

bool CDOCluster::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
//    return (frustum.testSphere(m_Position,AITPOINT_SIZE))?true:false;
    return false;
}
//----------------------------------------------------

bool CDOCluster::RayPick(float& distance, Fvector& S, Fvector& D, Fmatrix& parent, SRayPickInfo* pinf){
/*	Fvector transformed;
	parent.transform_tiny(transformed, m_Position);

	Fvector ray2;
	ray2.sub( transformed, S );

    float d = ray2.dotproduct(D);
    if( d > 0  ){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (AITPOINT_SIZE*AITPOINT_SIZE)) && (d>AITPOINT_SIZE) ){
        	if (d<distance){
	            distance = d;
    	        return true;
            }
        }
    }
*/
	return false;
}
//----------------------------------------------------

void CDOCluster::Move( Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
	m_Position.add( amount );
}
//----------------------------------------------------

bool CDOCluster::Load(CStream& F){
	DWORD version = 0;
	char buf[1024];
/*
    R_ASSERT(F.ReadChunk(AITPOINT_CHUNK_VERSION,&version));
    if( version!=AITPOINT_VERSION ){
        Log->DlgMsg( mtError, "CAITPoint: Unsuported version.");
        return false;
    }
*/    
	SceneObject::Load(F);
/*
    R_ASSERT(F.FindChunk(AITPOINT_CHUNK_POINT));
    F.Rvector		(m_Position);

    R_ASSERT(F.FindChunk(AITPOINT_CHUNK_LINKS));
    m_NameLinks.resize(F.Rdword());
    for (AStringIt s_it=m_NameLinks.begin(); s_it!=m_NameLinks.end(); s_it++){
		F.RstringZ	(buf); *s_it = buf;
    }
*/
    return true;
}
//----------------------------------------------------

void CDOCluster::Save(CFS_Base& F){
	SceneObject::Save(F);

/*
	F.open_chunk	(AITPOINT_CHUNK_VERSION);
	F.Wword			(AITPOINT_VERSION);
	F.close_chunk	();

    F.open_chunk	(AITPOINT_CHUNK_POINT);
    F.Wvector		(m_Position);
	F.close_chunk	();

    F.open_chunk	(AITPOINT_CHUNK_LINKS);
    F.Wdword		(m_Links.size());
    for (ObjectIt o_it=m_Links.begin(); o_it!=m_Links.end(); o_it++)
    	F.WstringZ	((*o_it)->GetName());
	F.close_chunk	();
*/
}
//----------------------------------------------------

#define DETOBJ_REFERENCE 	0x0100
#define DETOBJ_COLOR	 	0x0101

#define DETMGR_HEADER 		0x0000
#define DETMGR_OBJECTS 		0x0001
#define DETMGR_SLOTS		0x0002
#define DETMGR_BBOX			0x1000
#define DETMGR_BASE_TEXTURE	0x1001

CDetail::CDetail(){
	m_pShader			= 0;
	m_dwFlags			= 0;
	m_fRadius			= 0;
	m_pRefs				= 0;
    m_dwColor			= 0;
}

CDetail::~CDetail(){
	if (m_pShader) 		UI->Device.Shader.Delete(m_pShader);
	m_Vertices.clear	();
}

bool CDetail::Update(DWORD color, LPCSTR name){
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

    m_dwColor = color;

    return true;
}

bool CDetail::Load(CStream& F){
	char buf[255];
	// references
    R_ASSERT			(F.FindChunk(DETOBJ_REFERENCE));
    F.RstringZ			(buf);

    R_ASSERT			(F.FindChunk(DETOBJ_COLOR));
    m_dwColor			= F.Rdword();

    // update object
    return 				Update(m_dwColor,buf);
}

void CDetail::Save(CFS_Base& F){
	R_ASSERT			(m_pRefs);
	F.open_chunk		(DETOBJ_REFERENCE);
    F.WstringZ			(m_pRefs->GetName());
    F.close_chunk		();

	F.open_chunk		(DETOBJ_COLOR);
    F.Wdword			(m_dwColor);
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
}

CDetailManager::~CDetailManager(){
	for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	_DELETE(*it);
    _DELETE(m_pBaseTexture);
}

DWORD CDetailManager::GetColor(float x, float z){
	float u = (x-m_BBox.min.x)/(m_BBox.max.x-m_BBox.min.x);
	float v = (z-m_BBox.min.z)/(m_BBox.max.z-m_BBox.min.z);
	int U = iFloor(u*m_pBaseTexture->width()+0.5f); 	U %= m_pBaseTexture->width();
	int V = iFloor(v*m_pBaseTexture->height()+0.5f);	V %= m_pBaseTexture->height();
	return m_pBaseTexture->GetPixel(U,V);
}

IC bool TestTris(float& h, const Fvector& start, SBoxPickInfoVec& tris){
	h	= flt_max;
    bool bRes=false;
	for (SBoxPickInfoIt it=tris.begin(); it!=tris.end(); it++){
    	float range;
		if (RAPID::TestRayTri2(start,down_vec,it->bp_inf.p,range))
        	if (range<h){
            	h	= range;
                bRes= true;
            }
    }
    if (bRes) h = start.y-h;
    return bRes;
}

void CDetailManager::FindClosestApproach(const Fcolor& C){
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
        if (IDS_count<3){
            bool bFound=false;
            for (int k=0; k<IDS_count; k++){
                if (IDS[k].index==index){
                	if(dist<IDS[k].dist){
	                    IDS[k].dist 	= dist;
    	                IDS[k].index    = index;
                    }
                    bFound = true;
                    break;
                }
            }
            if (!bFound){
                IDS[IDS_count].dist = dist;
                IDS[IDS_count].index= index;
                IDS_count++;
            }
        }else{
            int i=-1;
            float dd=flt_max;
            bool bFound=false;
            for (int k=0; k<3; k++){
                float d = dist-IDS[k].dist;
                if ((d<0)&&(d<dd)){ i=k; dd=d;}
                if (IDS[k].index==index){
                	if(dist<IDS[k].dist){
	                    IDS[k].dist 	= dist;
    	                IDS[k].index    = index;
                    }
                    bFound = true;
                    break;
                }
            }
            if (!bFound&&(i>=0)){
                IDS[i].dist 	= dist;
                IDS[i].index    = index;
            }
        }
    }
}

//static BYTEVec alpha;
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

void CDetailManager::AppendObject(DWORD color, LPCSTR name){
	for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++)
    	if (stricmp((*it)->GetName(),name)==0) return;

    color = DU::subst_a(color,0);
    CDetail* D = new CDetail();
    if (!D->Update(color,name)){
    	_DELETE(D);
        return;
    }
    m_Objects.push_back(D);
    m_ColorIndices[color].push_back(D);
}

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

    // get bounding box
	ObjectList& lst=Scene->m_SnapObjects;
	if (!Scene->GetBox(m_BBox,lst)) return false;

    // fill header
    m_Header.version	= DETAIL_VERSION;
    int mn_x 			= iFloor(m_BBox.min.x/DETAIL_SLOT_SIZE+0.5f);
    int mn_z 			= iFloor(m_BBox.min.z/DETAIL_SLOT_SIZE+0.5f);
    int mx_x 			= iFloor(m_BBox.max.x/DETAIL_SLOT_SIZE+0.5f);
    int mx_z 			= iFloor(m_BBox.max.z/DETAIL_SLOT_SIZE+0.5f);
    m_Header.offs_x 	= -mn_x;
    m_Header.offs_z 	= -mn_z;
	m_Header.size_x 	= mx_x-mn_x;
	m_Header.size_z 	= mx_z-mn_z;

    UI->ProgressStart	(m_Header.size_x*m_Header.size_z,"Updating...");

    m_Slots.clear		();
    m_Slots.resize		(m_Header.size_x*m_Header.size_z);
    for (int z=0; z<m_Header.size_z; z++){
        for (int x=0; x<m_Header.size_x; x++){
        	DSIt slot	= m_Slots.begin()+z*m_Header.size_x+x;
        	slot->y_min	= m_BBox.min.y;
        	slot->y_max	= m_BBox.max.y;
        	GenerateOneSlot(x,z,*slot);
            UI->ProgressInc();
        }
    }
    UI->ProgressEnd		();

	return true;
}

void CDetailManager::Render(ERenderPriority flag){
	if (Valid()){
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
    m_Slots.clear		();
}

void CDetailManager::Load(CStream& F){
}

void CDetailManager::Save(CFS_Base& F){
	// header
	F.write_chunk		(DETMGR_HEADER,&m_Header,sizeof(DetailHeader));
    // objects
	F.open_chunk		(DETMGR_OBJECTS);
    for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
		F.open_chunk	(it-m_Objects.begin());
        (*it)->Save		(F);
	    F.close_chunk	();
    }
    F.close_chunk		();
    // slots
	F.open_chunk		(DETMGR_SLOTS);
	F.write				(m_Slots.begin(),m_Slots.size()*sizeof(DetailSlot));
    F.close_chunk		();

    // internal
    // bbox
	F.write_chunk		(DETMGR_BBOX,&m_BBox,sizeof(Fbox));
	// base texture
    R_ASSERT			(m_pBaseTexture);
	F.open_chunk		(DETMGR_BASE_TEXTURE);
    F.WstringZ			(m_pBaseTexture->name());
    F.close_chunk		();
}

void CDetailManager::Export(CFS_Base& F){
	// header
	F.write_chunk		(DETMGR_HEADER,&m_Header,sizeof(DetailHeader));
    // objects
	F.open_chunk		(DETMGR_OBJECTS);
    for (DOIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
		F.open_chunk	(it-m_Objects.begin());
        (*it)->Export	(F);
	    F.close_chunk	();
    }
    F.close_chunk		();
    // slots
	F.open_chunk		(DETMGR_SLOTS);
	F.write				(m_Slots.begin(),m_Slots.size()*sizeof(DetailSlot));
    F.close_chunk		();
}

