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
void CDOCluster::Update(){
	Fvector pos,dir;
    dir.set(0.f,-1.f,0.f);
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
    for (int x=0; x<cnt; x++,pos.x+=delta){
	    pos.z = m_Position.z-cl_size*0.5f+delta*.5f;
	    for (int z=0; z<cnt; z++,pos.z+=delta){
        	pos.y = m_Position.y+offs;
            SPickInfo pinf;
            if (Scene->RTL_Pick(pos, dir, OBJCLASS_EDITOBJECT, &pinf, false, fraLeftBar->ebEnableSnapList->Down)){
				clr[x*cnt+z]=DetermineColor(pinf);
                alt[x*cnt+z]=pinf.pt.y;
                if (pinf.pt.y>m_MaxHeight) m_MaxHeight = pinf.pt.y;
                if (pinf.pt.y<m_MinHeight) m_MinHeight = pinf.pt.y;
            }else{
				alt[x*cnt+z]=0;
            }
        }
    }
    float sz = m_MaxHeight-m_MinHeight;
    for (int k=0; k<cnt2; k++) m_Objects[k].m_Alt = DO_PACK(alt[k],m_MinHeight,sz);
}
//----------------------------------------------------
DWORD CDOCluster::DetermineColor(const SPickInfo& pinf){
	const RAPID::raypick_info& rpinf = pinf.rp_inf;
    // barycentric coords
    // note: W,U,V order
    Fvector B;
    B.set(1.0f - rpinf.u - rpinf.v,rpinf.u,rpinf.v);

    // calc UV
    const Fvector2* TC[3];
    st_Surface* surf = pinf.mesh->GetFaceTC(rpinf.id,TC);
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
    float sz = m_MaxHeight-m_MinHeight;
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

bool CDOCluster::RTL_Pick(float& distance, Fvector& S, Fvector& D, Fmatrix& parent, SPickInfo* pinf){
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


