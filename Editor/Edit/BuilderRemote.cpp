#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "SceneClassList.h"
#include "ELight.h"
#include "EditObject.h"
#include "occluder.h"
#include "Communicate.h"
#include "Scene.h"
#include "EditMesh.h"
#include "Texture.h"
#include "glow.h"
#include "dpatch.h"
#include "sector.h"
#include "portal.h"
#include "frustum.h"
#include "xrLevel.h"
#include "xrShader.h"
#include "shader.h"

#define vSpecular			1
#define vLight				2
#define vAlpha				4
#define vFog				8
#define vZTest				16
#define vZWrite				32

#define vStatic             vFog|vLight|vZTest
/*
static HMODULE hStaticBuilder = 0;

typedef void __cdecl TStart (b_transfer *);
typedef void __cdecl TWait  (void);
typedef bool __cdecl TStop  (void);

TStart* g_BStart = 0;
TWait*  g_BWait  = 0;
TStop*  g_BStop  = 0;
*/
template <class T>
DWORD transfer(CFS_Base& F, T* src, DWORD cnt )
{
	DWORD save = F.tell();
	F.write(src,sizeof(T)*cnt);
	return save;
}

void SaveBuild(b_transfer *info)
{
	b_transfer * L 	= (b_transfer *)info;
	b_transfer	 pF = *L;

    AnsiString fn;
    fn 				= info->params.L_path; 	fn += "build.prj";
    CFS_File F		(fn.c_str());

	F.write			(&pF,sizeof(pF));

	pF.vertices		= (b_vertex*	)transfer(F, L->vertices,	L->vertex_count);
	pF.faces		= (b_face*		)transfer(F, L->faces,		L->face_count);
	pF.material		= (b_material*	)transfer(F, L->material,	L->mtl_count);
	pF.shaders		= (b_shader*	)transfer(F, L->shaders,	L->shader_count);
	pF.textures		= (b_texture*	)transfer(F, L->textures,	L->tex_count);
	pF.lights		= (b_light*		)transfer(F, L->lights,  	L->light_count);
	pF.glows		= (b_glow*		)transfer(F, L->glows,		L->glow_count);
	pF.particles	= (b_particle*	)transfer(F, L->particles,	L->particle_count);
	pF.occluders	= (b_occluder*	)transfer(F, L->occluders,	L->occluder_count);
	pF.portals		= (b_portal*	)transfer(F, L->portals,	L->portal_count);
	pF.light_keys	= (Flight*		)transfer(F, L->light_keys,L->light_keys_count);

	F.seek			(0);
	F.write			(&pF,sizeof(pF));
}

/*
void StartBuild(b_transfer *p)
{
    hStaticBuilder = LoadLibrary("xrLBuilder.dll");
    g_BStart = (TStart*)GetProcAddress(hStaticBuilder, "buildStart");
    g_BWait = (TWait*)GetProcAddress(hStaticBuilder, "buildWait");
    g_BStop = (TStop*)GetProcAddress(hStaticBuilder, "buildIsDone");
    VERIFY(g_BStart);
    VERIFY(g_BWait);
    VERIFY(g_BStop);
    g_BStart(p);
};
void WaitBuild(void)
{
    g_BWait();
    Sleep(1000);
    g_BStart = 0;
    g_BWait  = 0;
    g_BStop  = 0;
    FreeLibrary(hStaticBuilder);
};
bool BuildIsDone(void)
{
    BOOL bRes = g_BStop();
    if (bRes){
        Sleep(1000);
        g_BStart = 0;
        g_BWait  = 0;
        g_BStop  = 0;
        FreeLibrary(hStaticBuilder); hStaticBuilder=0;
    }
    return !bRes;
};
*/
int SceneBuilder::CalculateSector(const Fvector& P, float R){
    ObjectIt _F = Scene->FirstObj(OBJCLASS_SECTOR);
    ObjectIt _E = Scene->LastObj(OBJCLASS_SECTOR);
    for(;_F!=_E;_F++){
    	CSector* _S=(CSector*)(*_F);
        EVisible vis=_S->TestCHullSphereIntersection(P,R);
        if ((vis==fvPartialInside)||(vis==fvFully))
        	if (_S->sector_num!=m_iDefaultSectorNum) return _S->sector_num;
	}
    return m_iDefaultSectorNum; // по умолчанию
}

void SceneBuilder::ResetStructures (){
    l_vertices_cnt 	= 0;
	l_faces_cnt		= 0;
	l_vertices_it 	= 0;
	l_faces_it		= 0;
    _DELETEARRAY(l_vertices); 
    _DELETEARRAY(l_faces);
    l_textures.clear();
    l_shaders.clear();
    l_materials.clear();
    l_vnormals.clear();
    l_glows.clear();
    l_dpatches.clear();
    l_occluders.clear();
    l_portals.clear();
    l_light_keys.clear();
}

//------------------------------------------------------------------------------
// CEditObject build functions
//------------------------------------------------------------------------------
void SceneBuilder::BuildMesh(CEditObject* parent, CEditMesh* mesh){
    int point_offs;
    point_offs = l_vertices_it;  // save offset

    const Fmatrix& T = parent->GetTransform();
    
    // fill vertices
	for (FvectorIt pt_it=mesh->m_Points.begin(); pt_it!=mesh->m_Points.end(); pt_it++)
    	T.transform_tiny(l_vertices[l_vertices_it++],*pt_it);

    if (parent->IsDynamic()){
	    // update mesh
	    if (!(mesh->m_LoadState&EMESH_LS_FNORMALS)) mesh->GenerateFNormals();
		Fvector N;
        for(FvectorIt pt=mesh->m_Points.begin();pt!=mesh->m_Points.end();pt++){
            N.set(0,0,0);
            INTVec& a_lst = mesh->m_Adjs[pt-mesh->m_Points.begin()];
            VERIFY(a_lst.size());
            for (INTIt i_it=a_lst.begin(); i_it!=a_lst.end(); i_it++)
                N.add(mesh->m_FNormals[*i_it]);
            N.normalize_safe();
            T.transform_dir(N);
            l_vnormals.push_back(N);
        }
	    // unload mesh normals
	    mesh->UnloadFNormals();
    }
    // массив фейсов с указанием к какому сектору он принадлежит
    INTVec sector_faces;
	sector_faces.resize(mesh->GetFaceCount(false));
    fill(sector_faces.begin(),sector_faces.end(),m_iDefaultSectorNum);
	// пройдемся по всем секторам и определим принадлежность фейсов
    ObjectIt _F = Scene->FirstObj(OBJCLASS_SECTOR);
    ObjectIt _E = Scene->LastObj(OBJCLASS_SECTOR);
    for(;_F!=_E;_F++){
    	CSector* _S=(CSector*)(*_F);
		SItemIt s_it;
		if (_S->FindSectorItem(parent->GetName(), mesh->GetName(), s_it))
        	for (DWORDIt f_it=s_it->Face_IDs.begin(); f_it!=s_it->Face_IDs.end(); f_it++)
            	sector_faces[*f_it]=_S->sector_num;
	}

    // fill faces
    for (SurfFacesPairIt sp_it=mesh->m_SurfFaces.begin(); sp_it!=mesh->m_SurfFaces.end(); sp_it++){
		INTVec& face_lst = sp_it->second;
        st_Surface* surf = sp_it->first;
		DWORD dwTexCnt = ((surf->dwFVF&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
	    for (INTIt f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
			st_Face& face = mesh->m_Faces[*f_it];
        	{
                b_face& new_face = l_faces[l_faces_it++];
                new_face.dwMaterial = BuildMaterial(mesh,surf,sector_faces[*f_it]);
                for (int k=0; k<3; k++){
                    st_FaceVert& fv = face.pv[k];
                    // vertex index
                    new_face.v[k]=fv.pindex+point_offs;
                    // uv maps
                    int offs = 0;
                    for (DWORD t=0; t<dwTexCnt; t++){
                        st_VMapPt& vm_pt 	= mesh->m_VMRefs[fv.vmref][t];
                        st_VMap& vmap		= mesh->m_VMaps[vm_pt.vmap_index];
                        if (vmap.type!=vmtUV){
                            offs++;
                            t--; 
                            continue;
                        }
                        Fvector2& uv		= vmap.getUV(vm_pt.index);
                        new_face.t[t][k].tu = uv.x;
                        new_face.t[t][k].tv = uv.y;
                    }
                }
            }
            
	        if (surf->sideflag){
                b_face& new_face 	= l_faces[l_faces_it++];
                new_face.dwMaterial = l_faces[l_faces_it-2].dwMaterial;
                for (int k=0; k<3; k++){
                    st_FaceVert& fv = face.pv[2-k];
                    // vertex index
                    new_face.v[k]=fv.pindex+point_offs;
                    // uv maps
                    int offs = 0;
                    for (DWORD t=0; t<dwTexCnt; t++){
                        st_VMapPt& vm_pt 	= mesh->m_VMRefs[fv.vmref][t];
                        st_VMap& vmap		= mesh->m_VMaps[vm_pt.vmap_index];
                        if (vmap.type!=vmtUV){
                            offs++;
                            t--; 
                            continue;
                        }
                        Fvector2& uv		= vmap.getUV(vm_pt.index);
                        new_face.t[t][k].tu = uv.x;
                        new_face.t[t][k].tv = uv.y;
                    }
                }
            }
        }
    }
}

void SceneBuilder::BuildObject(CEditObject* obj){
	CEditObject *O = obj->IsReference()?obj->GetRef():obj;
    AnsiString temp; temp.sprintf("Building object: %s",O->GetName());
    UI->SetStatus(temp.c_str());
    for(EditMeshIt M=O->FirstMesh();M!=O->LastMesh();M++) BuildMesh(obj,*M);
}

void SceneBuilder::BuildObjectDynamic(CEditObject* obj){
// compute vertex/face count    
	l_faces_cnt		+= obj->GetFaceCount();
    l_vertices_cnt  += obj->GetVertexCount();
	l_faces			= new b_face[l_faces_cnt];
	l_vertices		= new b_vertex[l_vertices_cnt];
    for(EditMeshIt M=obj->FirstMesh();M!=obj->LastMesh();M++) BuildMesh(obj,*M);
}

//------------------------------------------------------------------------------
// light build functions
//------------------------------------------------------------------------------
void SceneBuilder::BuildLight(b_light* b, CLight* e){
	CopyMemory		(b,&e->m_D3D,sizeof(xrLIGHT));
    b->diffuse.mul_rgb(e->m_Brightness);
    b->ambient.mul_rgb(e->m_Brightness);
    b->specular.mul_rgb(e->m_Brightness);
	b->shadowed_scale=e->m_ShadowedScale;

    if (e->m_D3D.flags&XRLIGHT_PROCEDURAL){
    	b->p_key_start = l_light_keys.size();
        b->p_key_count = e->m_Data.size();
        for (ALItemIt it=e->m_Data.begin(); it!=e->m_Data.end(); it++){
            Flight l=*it; l.diffuse.mul_rgb(it->m_Brightness);
            l_light_keys.push_back(l);
        }
    }

    if (b->type==D3DLIGHT_POINT){
        b->s_count=0;
        // test fully and partial inside
        ObjectIt _F = Scene->FirstObj(OBJCLASS_SECTOR);
        ObjectIt _E = Scene->LastObj(OBJCLASS_SECTOR);
        for(;_F!=_E;_F++){
            CSector* _S=(CSector*)(*_F);
            EVisible vis=_S->TestCHullSphereIntersection(b->position,b->range);
            if ((vis==fvPartialInside)||(vis==fvFully))
            	b->s_sectors[b->s_count++] = _S->sector_num;
			if (b->s_count>=XRLIGHT_MAX_SECTORS) break;
        }
        // test partial outside
        _F = Scene->FirstObj(OBJCLASS_SECTOR);
        for(;_F!=_E;_F++){
            CSector* _S=(CSector*)(*_F);
            EVisible vis=_S->TestCHullSphereIntersection(b->position,b->range);
            if (vis==fvPartialOutside)
            	b->s_sectors[b->s_count++] = _S->sector_num;
			if (b->s_count>=XRLIGHT_MAX_SECTORS) break;
        }
        if ((b->s_count==0)||(b->s_count<XRLIGHT_MAX_SECTORS))
        	b->s_sectors[b->s_count++] = m_iDefaultSectorNum;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Glow build functions
//------------------------------------------------------------------------------
void SceneBuilder::BuildGlow(b_glow* b, CGlow* e){
// material
    b_material mtl; ZeroMemory(&mtl,sizeof(mtl));
    int mtl_idx;
    mtl.dwTexCount  = 1;
    VERIFY(!e->m_ShaderName.IsEmpty());
    mtl.shader      = BuildShader		(e->m_ShaderName.c_str());
    for (AStringIt s_it=e->m_TexNames.begin(); s_it!=e->m_TexNames.end(); s_it++)
        mtl.surfidx[s_it-e->m_TexNames.begin()]=BuildTexture(UI->Device.Shader.FindTexture(s_it->c_str()),s_it->c_str());
    mtl.sector		= CalculateSector	(e->m_Position,e->m_Range);

    mtl_idx = FindInMaterials(&mtl);
    if (mtl_idx<0){
        l_materials.push_back(mtl);
        mtl_idx = l_materials.size()-1;
    }

// fill params
	b->P.set        (e->m_Position);
    b->size         = e->m_Range;
	b->dwMaterial   = mtl_idx;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Particle build functions
//------------------------------------------------------------------------------
void SceneBuilder::BuildDPatch(b_particle* b, st_SPData* e, st_DPSurface* surf){
// material
    b_material mtl; ZeroMemory(&mtl,sizeof(mtl));
    int mtl_idx;
    mtl.dwTexCount  = 1;
    VERIFY(surf->m_Shader);
    mtl.shader      = BuildShader		(surf->m_Shader->shader->cName);
    for (AStringIt s_it=surf->m_Textures.begin(); s_it!=surf->m_Textures.end(); s_it++)
        mtl.surfidx[s_it-surf->m_Textures.begin()]=BuildTexture(UI->Device.Shader.FindTexture(s_it->c_str()),s_it->c_str());
    mtl.sector		= CalculateSector	(e->m_Position,e->m_Range);

    mtl_idx = FindInMaterials(&mtl);
    if (mtl_idx<0){
        l_materials.push_back(mtl);
        mtl_idx = l_materials.size()-1;
    }
// fill params
	b->P.set        (e->m_Position);
    b->N.set        (e->m_Normal);
    b->size         = e->m_Range;
	b->dwMaterial   = mtl_idx;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Occluder build functions
//------------------------------------------------------------------------------
void SceneBuilder::BuildOccluder(b_occluder* b, COccluder* e){
    e->Optimize();
    e->UpdatePoints3D();
    b->vert_count = e->Get3DPoints().size();
    R_ASSERT(b->vert_count<=MAX_OCCLUDER_POINTS);
    R_ASSERT(b->vert_count>=MIN_OCCLUDER_POINTS);
    CopyMemory(b->vertices,e->Get3DPoints().begin(),sizeof(Fvector)*b->vert_count);
    b->sector=m_iDefaultSectorNum;//CalculateSector(e->m_vCenter,e->m_vPlaneSize.magnitude());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Portal build functions
//------------------------------------------------------------------------------
void SceneBuilder::BuildPortal(b_portal* b, CPortal* e){
	b->sector_front	= e->m_SectorFront->sector_num;
	b->sector_back	= e->m_SectorBack->sector_num;
    b->vert_count	= e->m_SimplifyVertices.size();
    CopyMemory(b->vertices,e->m_SimplifyVertices.begin(),e->m_SimplifyVertices.size()*sizeof(Fvector));
}

//------------------------------------------------------------------------------
// shader build functions
//------------------------------------------------------------------------------
int SceneBuilder::FindInShaders(b_shader* s){
    for (DWORD i=0; i<l_shaders.size(); i++)
        if(strcmp(l_shaders[i].name, s->name)==0)return i;
    return -1;
}
//------------------------------------------------------------------------------

int SceneBuilder::BuildShader(const char * s){
    b_shader sh;
    strcpy(sh.name,s);
    int sh_id = FindInShaders(&sh);
    if (sh_id<0){
        l_shaders.push_back(sh);
        return l_shaders.size()-1;
    }
    return sh_id;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// texture build functions
//------------------------------------------------------------------------------
int SceneBuilder::FindInTextures(const char* name){
    for (DWORD i=0; i<l_textures.size(); i++) 
    	if(stricmp(l_textures[i].name,name)==0) return i;
    return -1;
}
//------------------------------------------------------------------------------

int SceneBuilder::BuildTexture(ETextureCore* e_tex, const char* name){
    int tex_idx     = FindInTextures(name);
    if(tex_idx<0){
		b_texture       tex; ZeroMemory(&tex,sizeof(tex));
	    if (!e_tex){
            tex.dwWidth     = 1;
            tex.dwHeight    = 1;
            tex.bHasAlpha   = 0;
    	}else{
            tex.dwWidth     = e_tex->m_Width;
            tex.dwHeight    = e_tex->m_Height;
            tex.bHasAlpha   = e_tex->m_AlphaPresent;
        }
	    tex.pSurface    = 0;
		strcpy          (tex.name,name);
    	l_textures.push_back(tex);
		tex_idx         = l_textures.size()-1;
		AddUniqueTexName(name);
    }
    return tex_idx;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// material build functions
//------------------------------------------------------------------------------
int SceneBuilder::FindInMaterials(b_material* m){
    for (DWORD i=0; i<l_materials.size(); i++){
        if( (l_materials[i].dwTexCount==m->dwTexCount) &&
            (l_materials[i].shader==m->shader) &&
            (l_materials[i].sector==m->sector) &&
            (memcmp(l_materials[i].surfidx,m->surfidx,sizeof(m->surfidx[0])*XR_MAX_TEXTURES)==0))return i;
    }
    return -1;
}
//------------------------------------------------------------------------------

int SceneBuilder::BuildMaterial(CEditMesh* m, st_Surface* surf, int sector_num ){
    b_material mtl; ZeroMemory(&mtl,sizeof(mtl));
    VERIFY(sector_num>=0);
    int mtl_idx;
	DWORD tex_cnt 	= ((surf->dwFVF&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
    mtl.dwTexCount  = tex_cnt;  VERIFY(mtl.dwTexCount);               
    mtl.shader      = BuildShader(surf->shader->shader->cName);
    mtl.sector		= sector_num;
    for (AStringIt s_it=surf->textures.begin(); s_it!=surf->textures.end(); s_it++)
        mtl.surfidx[s_it-surf->textures.begin()]=BuildTexture(UI->Device.Shader.FindTexture(s_it->c_str()),s_it->c_str());
    mtl_idx = FindInMaterials(&mtl);
    if (mtl_idx<0){
        l_materials.push_back(mtl);
        mtl_idx = l_materials.size()-1;
    }
/*    b_material mtl; ZeroMemory(&mtl,sizeof(mtl));
    VERIFY(sector_num>=0);
    int mtl_idx;
    mtl.dwTexCount  = l_ids.size();  VERIFY(mtl.dwTexCount);
    mtl.shader      = BuildShader(m,l_ids);
    mtl.sector		= sector_num;
    VERIFY((l_ids.size()<XR_MAX_TEXTURES)&&(l_ids.size()>0));
    for(DWORD i=0; i<l_ids.size(); i++){
        mtl.surfidx[i]=BuildTexture(m->GetLayer(l_ids[i])->GetTexture());
        mtl.tclass[i]=m->GetLayer(l_ids[i])->GetTClass();
        mtl.script[i]=BuildScript(m,l_ids[i]);
    }
    mtl_idx = FindInMaterials(&mtl);
    if (mtl_idx<0){
        l_materials.push_back(mtl);
        mtl_idx = l_materials.size()-1;
    }
    bHasAlpha = m->GetLayer(l_ids[0])->HasAlpha();
*/
    return mtl_idx;
}
//------------------------------------------------------------------------------

bool SceneBuilder::RemoteStaticBuild()
{
    b_transfer  TSData;

    ResetStructures();

	int objcount = Scene->ObjCount();
	if( objcount <= 0 )	return true;

// Build Options
    ZeroMemory(&TSData, sizeof(b_transfer));
	TSData._version = XRCL_CURRENT_VERSION;
    CopyMemory(&TSData.params,&Scene->m_BuildParams,sizeof(b_params));
    strcpy(TSData.params.L_name,Scene->m_LevelOp.m_LevelName.c_str());
    strcpy(TSData.params.L_path,"");
    m_LevelPath.Update(TSData.params.L_path);

// lights
    int cur_light = 0;
	TSData.light_count  = Scene->ObjCount(OBJCLASS_LIGHT);
    TSData.lights = 0;
    if (TSData.light_count){
    	TSData.lights   = new b_light[TSData.light_count];
    }

    UI->ProgressStart(Scene->ObjCount(OBJCLASS_EDITOBJECT),"Allocating memory for static faces and vertices...");
// compute vertex/face count    
    l_vertices_cnt	= 0;
    l_faces_cnt 	= 0;
	l_vertices_it 	= 0;
	l_faces_it		= 0;
    ObjectIt _O = Scene->FirstObj(OBJCLASS_EDITOBJECT);
    ObjectIt _E = Scene->LastObj(OBJCLASS_EDITOBJECT);
    for(;_O!=_E;_O++){
    	CEditObject* obj = (CEditObject*)(*_O);
	    UI->ProgressInc();
		if (!obj->IsDynamic()){
			l_faces_cnt		+= obj->GetFaceCount();
    	    l_vertices_cnt  += obj->GetVertexCount();
        }
    }
	l_faces		= new b_face[l_faces_cnt];
	l_vertices	= new b_vertex[l_vertices_cnt];

    UI->ProgressStart(Scene->ObjCount(OBJCLASS_EDITOBJECT),"Filling static mesh data...");
// parse scene
	int i=0;
    for(ObjectPairIt it=Scene->FirstClass(); it!=Scene->LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
		    UI->ProgressUpdate(i++);
            if (NeedAbort()) break;
            switch((*_F)->ClassID()){
            case OBJCLASS_LIGHT:
                BuildLight(&(TSData.lights[cur_light]),(CLight*)(*_F));
                cur_light++;
                break;
            case OBJCLASS_GLOW:
                l_glows.push_back(b_glow());
                BuildGlow(&l_glows.back(),(CGlow*)(*_F));
                break;
            case OBJCLASS_OCCLUDER:
                l_occluders.push_back(b_occluder());
                BuildOccluder(&l_occluders.back(),(COccluder*)(*_F));
                break;
            case OBJCLASS_PORTAL:
                l_portals.push_back(b_portal());
                BuildPortal(&l_portals.back(),(CPortal*)(*_F));
                break;
            case OBJCLASS_EDITOBJECT:{
                CEditObject *obj = (CEditObject*)(*_F);
                if (!obj->IsDynamic()){
                    BuildObject(obj);
                }else{
                	// add all textures from dynamic objects
					AddUniqueTexName(obj);
                }
            }break;
            }// end switch
        }
	}// end scene cycle
	UI->ProgressEnd();

    // add to used shader textures to texture export list
    for (DWORD k=0; k<l_shaders.size(); k++){
    	SH_ShaderDef* sh = SHLib->FindShader(AnsiString(l_shaders[k].name));
        VERIFY(sh);
        for (DWORD ps=0; ps<sh->Passes_Count; ps++)
	        for (DWORD st=0; st<sh->Passes[ps].Stages_Count; st++)
            	 if (sh->Passes[ps].Stages[st].Tname[0]!='$') AddUniqueTexName(sh->Passes[ps].Stages[st].Tname);
    }
    
    if (!NeedAbort()){
	    UI->ProgressStart(Scene->m_DetailPatches->ObjCount(),"Saving detail patches...");
        for(PatchMapIt p_it=Scene->m_DetailPatches->m_Patches.begin();p_it!=Scene->m_DetailPatches->m_Patches.end(); p_it++){
            PatchVec& lst = (*p_it).second;
            st_DPSurface* surf = (*p_it).first;
            for(PatchIt i = lst.begin();i!=lst.end();i++){
			    UI->ProgressInc();
                l_dpatches.push_back(b_particle());
                BuildDPatch(&l_dpatches.back(),i, surf);
            }
        }
	    UI->ProgressEnd();

	    UI->ProgressStart(11,"Saving geometry to file...");
    // shaders
        TSData.shader_count = l_shaders.size();
        TSData.shaders      = new b_shader[TSData.shader_count+1];
        CopyMemory(TSData.shaders,l_shaders.begin(),sizeof(b_shader)*TSData.shader_count);

	    UI->ProgressUpdate(1);
    // materials
        TSData.mtl_count    = l_materials.size();
        TSData.material     = new b_material[TSData.mtl_count+1];
        CopyMemory(TSData.material,l_materials.begin(),sizeof(b_material)*TSData.mtl_count);

	    UI->ProgressUpdate(2);
		VERIFY(l_vertices_cnt==l_vertices_it);
    // vertices
        TSData.vertex_count = l_vertices_cnt;
        TSData.vertices     = l_vertices;

	    UI->ProgressUpdate(3);
		VERIFY(l_faces_cnt==l_faces_it);
    // faces
        TSData.face_count   = l_faces_cnt;
        TSData.faces        = l_faces;

	    UI->ProgressUpdate(4);
    // textures
        TSData.tex_count    = l_textures.size();
        TSData.textures     = new b_texture[TSData.tex_count+1];
        CopyMemory(TSData.textures,l_textures.begin(),sizeof(b_texture)*TSData.tex_count);

	    UI->ProgressUpdate(5);
    // glows
        TSData.glow_count   = l_glows.size();
        TSData.glows        = new b_glow[TSData.glow_count+1];
        CopyMemory(TSData.glows,l_glows.begin(),sizeof(b_glow)*TSData.glow_count);

	    UI->ProgressUpdate(6);
    // particles
        TSData.particle_count= l_dpatches.size();
        TSData.particles    = new b_particle[TSData.particle_count+1];
        CopyMemory(TSData.particles,l_dpatches.begin(),sizeof(b_particle)*TSData.particle_count);

	    UI->ProgressUpdate(7);
    // occluders
        TSData.occluder_count= l_occluders.size();
        TSData.occluders    = new b_occluder[TSData.occluder_count+1];
        CopyMemory(TSData.occluders,l_occluders.begin(),sizeof(b_occluder)*TSData.occluder_count);

	    UI->ProgressUpdate(8);
    // light keys
        TSData.light_keys_count= l_light_keys.size();
        TSData.light_keys = new Flight[TSData.light_keys_count+1];
        CopyMemory(TSData.light_keys,l_light_keys.begin(),sizeof(Flight)*TSData.light_keys_count);

	    UI->ProgressUpdate(9);
    // portals
        TSData.portal_count	= l_portals.size();
        TSData.portals    = new b_portal[TSData.portal_count+1];
        CopyMemory(TSData.portals,l_portals.begin(),sizeof(b_portal)*TSData.portal_count);

	    UI->ProgressUpdate(10);
    // save data
        SaveBuild(&TSData);

        UI->ProgressEnd();
    }

    _DELETEARRAY(TSData.material);
    _DELETEARRAY(TSData.shaders);
    _DELETEARRAY(TSData.textures);
    _DELETEARRAY(TSData.lights);
    _DELETEARRAY(TSData.glows);
    _DELETEARRAY(TSData.particles);
    _DELETEARRAY(TSData.occluders);
    _DELETEARRAY(TSData.portals);
    _DELETEARRAY(TSData.light_keys);

    ResetStructures();

    return true;
}


