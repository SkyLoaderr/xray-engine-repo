#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "SceneClassList.h"
#include "ELight.h"
#include "SceneObject.h"
#include "EditObject.h"
#include "Communicate.h"
#include "Scene.h"
#include "EditMesh.h"
#include "Texture.h"
#include "glow.h"
#include "sector.h"
#include "groupobject.h"
#include "portal.h"
#include "frustum.h"
#include "xrLevel.h"
#include "UI_main.h"

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
	pF.shaders_xrlc	= (b_shader*	)transfer(F, L->shaders_xrlc,L->shader_xrlc_count);
	pF.textures		= (b_texture*	)transfer(F, L->textures,	L->tex_count);
	pF.lights		= (b_light*		)transfer(F, L->lights,  	L->light_count);
	pF.glows		= (b_glow*		)transfer(F, L->glows,		L->glow_count);
	pF.particles	= (b_particle*	)transfer(F, L->particles,	L->particle_count);
	pF.portals		= (b_portal*	)transfer(F, L->portals,	L->portal_count);
	pF.light_keys	= (Flight*		)transfer(F, L->light_keys,L->light_keys_count);

	F.seek			(0);
	F.write			(&pF,sizeof(pF));
}

int SceneBuilder::CalculateSector(const Fvector& P, float R){
    ObjectIt _F = Scene.FirstObj(OBJCLASS_SECTOR);
    ObjectIt _E = Scene.LastObj(OBJCLASS_SECTOR);
    for(;_F!=_E;_F++){
    	CSector* _S=(CSector*)(*_F);
        EVisible vis=_S->Intersect(P,R);
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
    l_lights.clear();
    l_textures.clear();
    l_shaders.clear();
    l_shaders_xrlc.clear();
    l_materials.clear();
    l_vnormals.clear();
    l_glows.clear();
    l_portals.clear();
    l_light_keys.clear();
}

//------------------------------------------------------------------------------
// CEditObject build functions
//------------------------------------------------------------------------------
void SceneBuilder::BuildMesh(const Fmatrix& parent, CEditableObject* object, CEditableMesh* mesh, int sect_num){
    int point_offs;
    point_offs = l_vertices_it;  // save offset

    // fill vertices
	for (FvectorIt pt_it=mesh->m_Points.begin(); pt_it!=mesh->m_Points.end(); pt_it++)
    	parent.transform_tiny(l_vertices[l_vertices_it++],*pt_it);

    if (object->IsDynamic()){
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
            parent.transform_dir(N);
            l_vnormals.push_back(N);
        }
	    // unload mesh normals
	    mesh->UnloadFNormals();
    }
    // fill faces
    for (SurfFacesPairIt sp_it=mesh->m_SurfFaces.begin(); sp_it!=mesh->m_SurfFaces.end(); sp_it++){
		INTVec& face_lst = sp_it->second;
        CSurface* surf = sp_it->first;
		DWORD dwTexCnt = ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
        R_ASSERT(dwTexCnt==1);
	    for (INTIt f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
			st_Face& face = mesh->m_Faces[*f_it];
        	{
                b_face& new_face = l_faces[l_faces_it++];
                new_face.dwMaterial = BuildMaterial(mesh,surf,sect_num);
                for (int k=0; k<3; k++){
                    st_FaceVert& fv = face.pv[k];
                    // vertex index
                    new_face.v[k]=fv.pindex+point_offs;
                    // uv maps
                    int offs = 0;
                    for (DWORD t=0; t<dwTexCnt; t++){
                        st_VMapPt& vm_pt 	= mesh->m_VMRefs[fv.vmref][t];
                        st_VMap& vmap		= *mesh->m_VMaps[vm_pt.vmap_index];
                        if (vmap.type!=vmtUV){
                            offs++;
                            t--;
                            continue;
                        }
                        Fvector2& uv		= vmap.getUV(vm_pt.index);
                        new_face.t[k].tu 	= uv.x;
                        new_face.t[k].tv 	= uv.y;
                    }
                }
            }

	        if (surf->_2Sided()){
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
                        st_VMap& vmap		= *mesh->m_VMaps[vm_pt.vmap_index];
                        if (vmap.type!=vmtUV){
                            offs++;
                            t--;
                            continue;
                        }
                        Fvector2& uv		= vmap.getUV(vm_pt.index);
                        new_face.t[k].tu = uv.x;
                        new_face.t[k].tv = uv.y;
                    }
                }
            }
        }
    }
}

void SceneBuilder::BuildObject(CSceneObject* obj){
	CEditableObject *O = obj->GetReference();
    AnsiString temp; temp.sprintf("Building object: %s",O->GetName());
    UI.SetStatus(temp.c_str());

    const Fmatrix& T = obj->_Transform();                           
    for(EditMeshIt M=O->FirstMesh();M!=O->LastMesh();M++){
		CSector* S = PortalUtils.FindSector(obj,*M);
	    int sect_num = S?S->sector_num:m_iDefaultSectorNum;
    	BuildMesh(T,O,*M,sect_num);
    }
}

//------------------------------------------------------------------------------
// light build functions
//------------------------------------------------------------------------------
void SceneBuilder::BuildLight(b_light* b, CLight* e){
	strcpy(b->name,e->Name);
	CopyMemory		(b,&e->m_D3D,sizeof(Flight));
    b->diffuse.mul_rgb(e->m_Brightness);
    b->ambient.mul_rgb(e->m_Brightness);
    b->specular.mul_rgb(e->m_Brightness);

    b->flags.bAffectStatic 	= e->m_Flags.bAffectStatic;
    b->flags.bAffectDynamic = e->m_Flags.bAffectDynamic;
    b->flags.bProcedural 	= e->m_Flags.bProcedural;
    if (e->m_Flags.bProcedural){
    }

    if (b->type==D3DLIGHT_POINT){
        // test fully and partial inside
        ObjectIt _F = Scene.FirstObj(OBJCLASS_SECTOR);
        ObjectIt _E = Scene.LastObj(OBJCLASS_SECTOR);
        for(;_F!=_E;_F++){
			if (b->sectors.size()>=16) break;
            CSector* _S=(CSector*)(*_F);
            EVisible vis=_S->Intersect(b->position,b->range);
            if ((vis==fvPartialInside)||(vis==fvFully))
            	b->sectors.push_back(_S->sector_num);
        }
        // test partial outside
        _F = Scene.FirstObj(OBJCLASS_SECTOR);
        for(;_F!=_E;_F++){
			if (b->sectors.size()>=16) break;
            CSector* _S=(CSector*)(*_F);
            EVisible vis=_S->Intersect(b->position,b->range);
            if (vis==fvPartialOutside)
            	b->sectors.push_back(_S->sector_num);
        }
        R_ASSERT(b->sectors.size());
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
    VERIFY(!e->m_ShaderName.IsEmpty());
    mtl.shader      = BuildShader		(e->m_ShaderName.c_str());
    mtl.shader_xrlc	= -1;
	mtl.surfidx		= BuildTexture		(e->m_TexName.c_str());
    mtl.sector		= CalculateSector	(e->PPosition,e->m_Range);

    mtl_idx = FindInMaterials(&mtl);
    if (mtl_idx<0){
        l_materials.push_back(mtl);
        mtl_idx = l_materials.size()-1;
    }

// fill params
	b->P.set        (e->PPosition);
    b->size         = e->m_Range;
	b->dwMaterial   = mtl_idx;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Portal build functions
//------------------------------------------------------------------------------
void SceneBuilder::BuildPortal(b_portal* b, CPortal* e){
	b->sector_front	= e->m_SectorFront->sector_num;
	b->sector_back	= e->m_SectorBack->sector_num;
    b->vertices.resize(e->m_SimplifyVertices.size());
    CopyMemory(b->vertices.begin(),e->m_SimplifyVertices.begin(),e->m_SimplifyVertices.size()*sizeof(Fvector));
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
// shader xrlc build functions
//------------------------------------------------------------------------------
int SceneBuilder::FindInShadersXRLC(b_shader* s){
    for (DWORD i=0; i<l_shaders_xrlc.size(); i++)
        if(strcmp(l_shaders_xrlc[i].name, s->name)==0)return i;
    return -1;
}
//------------------------------------------------------------------------------

int SceneBuilder::BuildShaderXRLC(const char * s){
    b_shader sh;
    strcpy(sh.name,s);
    int sh_id = FindInShadersXRLC(&sh);
    if (sh_id<0){
        l_shaders_xrlc.push_back(sh);
        return l_shaders_xrlc.size()-1;
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

int SceneBuilder::BuildTexture(const char* name){
    int tex_idx     = FindInTextures(name);
    if(tex_idx<0){
		b_texture       tex;
        ZeroMemory(&tex,sizeof(tex));
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
        if( (l_materials[i].surfidx		== m->surfidx) 		&&
            (l_materials[i].shader		== m->shader) 		&&
            (l_materials[i].shader_xrlc	== m->shader_xrlc) 	&&
            (l_materials[i].sector		== m->sector)) return i;
    }
    return -1;
}
//------------------------------------------------------------------------------

int SceneBuilder::BuildMaterial(CEditableMesh* m, CSurface* surf, int sector_num ){
    b_material mtl; ZeroMemory(&mtl,sizeof(mtl));
    VERIFY(sector_num>=0);
    int mtl_idx;
	DWORD tex_cnt 	= ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT); VERIFY(tex_cnt==1);
    mtl.shader      = BuildShader(surf->_ShaderName());
    mtl.shader_xrlc	= BuildShaderXRLC(surf->_ShaderXRLCName());
    mtl.sector		= sector_num;
	mtl.surfidx		= BuildTexture(surf->_Texture());
    mtl_idx 		= FindInMaterials(&mtl);
    if (mtl_idx<0){
        l_materials.push_back(mtl);
        mtl_idx 	= l_materials.size()-1;
    }
    return mtl_idx;
}
//------------------------------------------------------------------------------

void SceneBuilder::ParseStaticObjects(ObjectList& lst)
{
    for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
        UI.ProgressInc();
        if (UI.NeedAbort()) break;
        switch((*_F)->ClassID){
        case OBJCLASS_LIGHT:
            l_lights.push_back(b_light());
            BuildLight(&l_lights.back(),(CLight*)(*_F));
            break;
        case OBJCLASS_GLOW:
            l_glows.push_back(b_glow());
            BuildGlow(&l_glows.back(),(CGlow*)(*_F));
            break;
        case OBJCLASS_PORTAL:
            l_portals.push_back(b_portal());
            BuildPortal(&l_portals.back(),(CPortal*)(*_F));
            break;
        case OBJCLASS_SCENEOBJECT:{
            CSceneObject *obj = (CSceneObject*)(*_F);
            if (!obj->IsDynamic()) BuildObject(obj);
        }break;
        case OBJCLASS_GROUP:{ 
            CGroupObject* group = (CGroupObject*)(*_F);
            ParseStaticObjects(group->GetObjects());
        }break;
        }// end switch
    }
}
//------------------------------------------------------------------------------

bool SceneBuilder::CompileStatic()
{
    b_transfer  TSData;

    ResetStructures();

	int objcount = Scene.ObjCount();
	if( objcount <= 0 )	return true;

// Build Options
    ZeroMemory(&TSData, sizeof(b_transfer));
	TSData._version = XRCL_CURRENT_VERSION;
    CopyMemory(&TSData.params,&Scene.m_LevelOp.m_BuildParams,sizeof(b_params));
    strcpy(TSData.params.L_name,Scene.m_LevelOp.m_LevelName.c_str());
    strcpy(TSData.params.L_path,"");
    m_LevelPath.Update(TSData.params.L_path);

    UI.ProgressStart(Scene.ObjCount(OBJCLASS_SCENEOBJECT),"Allocating memory for static faces and vertices...");
// compute vertex/face count
    l_vertices_cnt	= 0;
    l_faces_cnt 	= 0;
	l_vertices_it 	= 0;
	l_faces_it		= 0;
    ObjectIt _O = Scene.FirstObj(OBJCLASS_SCENEOBJECT);
    ObjectIt _E = Scene.LastObj(OBJCLASS_SCENEOBJECT);
    for(;_O!=_E;_O++){
    	CSceneObject* obj = (CSceneObject*)(*_O);
	    UI.ProgressInc();
		if (!obj->IsDynamic()){
			l_faces_cnt		+= obj->GetFaceCount();
    	    l_vertices_cnt  += obj->GetVertexCount();
        }
    }
	l_faces		= new b_face[l_faces_cnt];
	l_vertices	= new b_vertex[l_vertices_cnt];

    UI.ProgressStart(Scene.ObjCount(OBJCLASS_SCENEOBJECT),"Parse static objects...");
// parse scene
    for(ObjectPairIt it=Scene.FirstClass(); it!=Scene.LastClass(); it++)
        ParseStaticObjects((*it).second);
	UI.ProgressEnd();

    if (!UI.NeedAbort()){
	    UI.ProgressStart(11,"Saving geometry to file...");
    // shaders
        TSData.shader_count = l_shaders.size();
        TSData.shaders      = new b_shader[TSData.shader_count+1];
        CopyMemory(TSData.shaders,l_shaders.begin(),sizeof(b_shader)*TSData.shader_count);
	    UI.ProgressUpdate(1);

    // shaders xrlc
        TSData.shader_xrlc_count = l_shaders_xrlc.size();
        TSData.shaders_xrlc      = new b_shader[TSData.shader_xrlc_count+1];
        CopyMemory(TSData.shaders_xrlc,l_shaders_xrlc.begin(),sizeof(b_shader)*TSData.shader_xrlc_count);

	    UI.ProgressUpdate(2);
    // materials
        TSData.mtl_count    = l_materials.size();
        TSData.material     = new b_material[TSData.mtl_count+1];
        CopyMemory(TSData.material,l_materials.begin(),sizeof(b_material)*TSData.mtl_count);

	    UI.ProgressUpdate(3);
		VERIFY(l_vertices_cnt==l_vertices_it);
    // vertices
        TSData.vertex_count = l_vertices_cnt;
        TSData.vertices     = l_vertices;

	    UI.ProgressUpdate(4);
		VERIFY(l_faces_cnt==l_faces_it);
    // faces
        TSData.face_count   = l_faces_cnt;
        TSData.faces        = l_faces;

	    UI.ProgressUpdate(5);
    // textures
        TSData.tex_count    = l_textures.size();
        TSData.textures     = new b_texture[TSData.tex_count+1];
        CopyMemory(TSData.textures,l_textures.begin(),sizeof(b_texture)*TSData.tex_count);

	    UI.ProgressUpdate(6);
    // glows
        TSData.glow_count   = l_glows.size();
        TSData.glows        = new b_glow[TSData.glow_count+1];
        CopyMemory(TSData.glows,l_glows.begin(),sizeof(b_glow)*TSData.glow_count);

	    UI.ProgressUpdate(7);
    // lights
        TSData.light_count	= l_lights.size();
        TSData.lights       = new b_light[TSData.light_count+1];
        CopyMemory(TSData.lights,l_lights.begin(),sizeof(b_light)*TSData.light_count);

	    UI.ProgressUpdate(8);
    // light keys
        TSData.light_keys_count= l_light_keys.size();
        TSData.light_keys = new Flight[TSData.light_keys_count+1];
        CopyMemory(TSData.light_keys,l_light_keys.begin(),sizeof(Flight)*TSData.light_keys_count);

	    UI.ProgressUpdate(9);
    // portals
        TSData.portal_count	= l_portals.size();
        TSData.portals    = new b_portal[TSData.portal_count+1];
        CopyMemory(TSData.portals,l_portals.begin(),sizeof(b_portal)*TSData.portal_count);

	    UI.ProgressUpdate(10);
    // save data
        SaveBuild(&TSData);

        UI.ProgressEnd();
    }

    _DELETEARRAY(TSData.material);
    _DELETEARRAY(TSData.shaders);
    _DELETEARRAY(TSData.shaders_xrlc);
    _DELETEARRAY(TSData.textures);
    _DELETEARRAY(TSData.lights);
    _DELETEARRAY(TSData.glows);
    _DELETEARRAY(TSData.particles);
    _DELETEARRAY(TSData.portals);
    _DELETEARRAY(TSData.light_keys);

    ResetStructures();

    return true;
}


