//----------------------------------------------------
// file: DPatch.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "DPatch.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "D3DUtils.h"
#include "xrshader.h"
#include "Frustum.h"

#define DPATCH_VERSION   			0x0010
//----------------------------------------------------
#define DPATCH_CHUNK_VERSION   		0xD400
#define DPATCH_CHUNK_PATCHES   		0xD401
#define     DPATCH_CHUNK_SURFACE  	0xD411
#define     DPATCH_CHUNK_DATA  		0xD412
//----------------------------------------------------

st_DPSurface::~st_DPSurface(){ 
	UI->Device.Shader.Delete(m_Shader); 
}
//----------------------------------------------------

CDPatchSystem::CDPatchSystem(){
    m_Patches.clear();
}

CDPatchSystem::~CDPatchSystem(){
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++)
        delete it->first;
}

st_DPSurface* CDPatchSystem::CreateSurface(AStringVec& tex_names, const char* sh_name){
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++){
    	st_DPSurface* s = it->first;
        if (strcmp(sh_name,s->m_Shader->shader->cName)==0){
        	if (tex_names.size()==s->m_Textures.size()){
            	bool bRes=true;
            	for (DWORD k=0; k<tex_names.size(); k++)
                	if (tex_names[k]!=s->m_Textures[k]){bRes=false;break;}
                if (bRes) return s;
            }
        }
    }
    st_DPSurface* s = new st_DPSurface();
    s->m_Textures 	= tex_names;
    s->m_Shader		= UI->Device.Shader.Create(sh_name,tex_names);
	return s;
}

int CDPatchSystem::ObjCount(){
    int cnt=0;
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++)
        cnt+=it->second.size();
    return cnt;
}
//----------------------------------------------------
/*bool CDPatchSystem::GetBox( SPData& d, Fbox& box ){
	box.vmin.set( d.m_Position );
	box.vmax.set( d.m_Position );
	box.vmin.x -= d.m_Range;
	box.vmin.y -= d.m_Range;
	box.vmin.z -= d.m_Range;
	box.vmax.x += d.m_Range;
	box.vmax.y += d.m_Range;
	box.vmax.z += d.m_Range;
	return true;
}
*/
bool CDPatchSystem::GetBox( Fbox& p_box ){
	Fbox box;
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++){
        PatchVec& lst = it->second;
    	for(PatchIt i = lst.begin();i!=lst.end();i++){
            box.set( (*i).m_Position, (*i).m_Position );
            box.min.sub((*i).m_Range);
            box.max.add((*i).m_Range);
            p_box.modify( box.min );
            p_box.modify( box.max );
        }
    }
	return true;
}

void CDPatchSystem::Render( ){
    UI->Device.SetRS	(D3DRENDERSTATE_CLIPPING, FALSE);
	UI->Device.ResetMaterial();
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++){
        PatchVec& lst = it->second;
        st_DPSurface* surf = it->first;
        VERIFY(surf);
        VERIFY(surf->m_Shader);
        UI->Device.Shader.Set(surf->m_Shader);
        for(PatchIt i = lst.begin();i!=lst.end();i++)
            if (UI->Device.m_Frustum.testSphere(i->m_Position,i->m_Range))
                m_RenderSprite.Render(i->m_Position,i->m_Range);
    }
// draw selection
    UI->Device.SetRS(D3DRENDERSTATE_FILLMODE,D3DFILL_WIREFRAME);
	UI->Device.Shader.Set(UI->Device.m_WireShader);
    for(it=m_Patches.begin();it!=m_Patches.end(); it++){
        PatchVec& lst = it->second;
        for(PatchIt i = lst.begin();i!=lst.end();i++)
            if (UI->Device.m_Frustum.testSphere(i->m_Position,i->m_Range))
                if( i->m_Selected ) m_RenderSprite.Render(i->m_Position,i->m_Range);
    }
	UI->Device.SetRS(D3DRENDERSTATE_FILLMODE,UI->dwRenderFillMode);
    UI->Device.SetRS(D3DRENDERSTATE_CLIPPING, TRUE);
}

void CDPatchSystem::FrustumSelect( bool flag ){
	CFrustum frustum;
    if (!UI->SelectionFrustum(frustum)) return;

    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++){
        PatchVec& lst = it->second;
        for(PatchIt i = lst.begin();i!=lst.end();i++)
            (*i).m_Selected = (frustum.testSphere((*i).m_Position,(*i).m_Range))?flag:!flag;
    }
    UI->RedrawScene();
}

bool CDPatchSystem::RayPickSelect(float& dist, Fvector& start, Fvector& direction){
    bool b, bRes=false;
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++){
        PatchVec& lst = it->second;
        for(PatchIt i = lst.begin();i!=lst.end();i++){
            b = false;
        	Fvector ray2;
        	ray2.sub( (*i).m_Position, start );
            float d = ray2.dotproduct(direction);
            if( d > 0  ){
                float d2 = ray2.magnitude();
                if( ((d2*d2-d*d) < ((*i).m_Range*(*i).m_Range)) && (d>(*i).m_Range) ){
                	if (d<dist){
                    	dist = d;
	                    b = true;
    	                bRes=true;
                    }
                }
            }
            (*i).m_Selected = b;
        }
    }
    return bRes;
}

void CDPatchSystem::Clear(){
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++)
        delete it->first;
    m_Patches.clear();
}

void CDPatchSystem::AddDPatch(PatchVec& lst, float r, Fvector& p, Fvector& n){
    st_SPData d;
    d.m_Range = r;
    d.m_Position.set(p);
    d.m_Normal.set(n);
    lst.push_back(d);
}

void CDPatchSystem::AddDPatch(st_DPSurface* surf, float r, Fvector& p, Fvector& n){
	VERIFY(surf);
	AddDPatch(m_Patches[surf], r, p, n);
/* 
	previous release!!!!
   bool bFound=false;
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++){
        if (stricmp(it->first->name(),tname)==0){
            bFound=true;
            AddDPatch(it->second, tname, r, p, n);
            break;
        }
    }

    if (!bFound){
        PatchVec new_lst;
        ETexture* tex = new ETexture(tname);
        m_Patches.insert(make_pair(tex,new_lst));
        AddDPatch((*m_Patches.find(tex)).second, tname, r, p, n);
    }
*/
}

void  CDPatchSystem::MoveDPatches  (Fvector& amount){
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++){
        PatchVec& lst = it->second;
        for(PatchIt i = lst.begin();i!=lst.end();i++)
            (*i).m_Position.add(amount);
    }
}

int CDPatchSystem::InvertSelection(){
    int cnt=0;
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++){
        PatchVec& lst = it->second;
        for(PatchIt i = lst.begin();i!=lst.end();i++){
            (*i).m_Selected = !(*i).m_Selected;
            cnt++;
        }
    }
    return cnt;
}

int CDPatchSystem::Select( bool flag ){
    int cnt=0;
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++){
        PatchVec& lst = it->second;
        for(PatchIt i = lst.begin();i!=lst.end();i++){
            (*i).m_Selected=flag;
            cnt++;
        }
    }
    return cnt;
}

int CDPatchSystem::SelectionCount ( bool testflag ){
    int cnt=0;
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++){
        PatchVec& lst = it->second;
        for(PatchIt i = lst.begin();i!=lst.end();i++)
            if ((*i).m_Selected==testflag) cnt++;
    }
    return cnt;
}


class CmpDP
{
public:
    bool operator() (st_SPData &d) { return d.m_Selected; }
};

int CDPatchSystem::RemoveSelection(){
    int cnt=0;
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++){
        PatchVec& lst = it->second;
        PatchIt _R = remove_if(lst.begin(), lst.end(), CmpDP());
        cnt=lst.end()-_R;
        lst.erase(_R,lst.end());
/*        PatchIt _F = lst.begin();
        while(_F!=lst.end()){
            if ((*_F).m_Selected){
                PatchIt _D = _F; _F++;
                lst.erase(_D);
                cnt++;
            }else{
                _F++;
            }
        }
*/
    }
    return cnt;
}

bool CDPatchSystem::LoadPatches(CStream& F){
	char buf	[MAX_PATH];
	char sh_name[MAX_PATH];

    st_DPSurface* 	surf=0;

    R_ASSERT(F.FindChunk(DPATCH_CHUNK_SURFACE));
    surf 			= new st_DPSurface();
    F.RstringZ 		(sh_name);
    surf->m_Textures.resize(F.Rdword());
    for (AStringIt s_it=surf->m_Textures.begin(); s_it!=surf->m_Textures.end(); s_it++){
        F.RstringZ	(buf); *s_it = buf;
    }
    surf->m_Shader	= UI->Device.Shader.Create(sh_name,surf->m_Textures);

    R_ASSERT(F.FindChunk(DPATCH_CHUNK_DATA));
    PatchVec& lst	= m_Patches[surf];
    DWORD cnt		= F.Rdword();
    lst.resize		(cnt);
    F.Read			(lst.begin(), cnt*sizeof(st_SPData));

	return true;
}

//----------------------------------------------------
bool CDPatchSystem::Load(CStream& F){
	DWORD version = 0;

    R_ASSERT(F.ReadChunk(DPATCH_CHUNK_VERSION,&version));
    if( version!=DPATCH_VERSION ){
        Log->DlgMsg( mtError, "DetailPatch: Unsupported version.");
        return false;
    }
    
    // Load meshes
    CStream* OBJ = F.OpenChunk(DPATCH_CHUNK_PATCHES); VERIFY(OBJ);
	if(OBJ){
        CStream* P   = OBJ->OpenChunk(0);
        for (int count=1; P; count++){
            LoadPatches(*P);
            P->Close();
            P = OBJ->OpenChunk(count);
        }
        OBJ->Close();
    }
    
	return true;
}

void CDPatchSystem::Save(CFS_Base& F){
	F.open_chunk	(DPATCH_CHUNK_VERSION);
	F.Wword			(DPATCH_VERSION);
	F.close_chunk	();

    // patches
	F.open_chunk	(DPATCH_CHUNK_PATCHES);
    int count = 0;
    for(PatchMapIt it=m_Patches.begin();it!=m_Patches.end(); it++){
        if (it->second.empty()) continue;
        F.open_chunk(count); count++;
			// surface
	        F.open_chunk	(DPATCH_CHUNK_SURFACE);
    	    F.WstringZ      (it->first->m_Shader->shader->cName);
			F.Wdword       	(it->first->m_Textures.size());
	        for (AStringIt s_it=it->first->m_Textures.begin(); s_it!=it->first->m_Textures.end(); s_it++)
		        F.WstringZ 	(s_it->c_str());
	        F.close_chunk();
			// patches data
        	F.open_chunk	(DPATCH_CHUNK_DATA);
			F.Wdword       	(it->second.size());
            F.write			(it->second.begin(), it->second.size()*sizeof(st_SPData));
	        F.close_chunk();
        F.close_chunk();
    }
	F.close_chunk	();
}
//----------------------------------------------------

