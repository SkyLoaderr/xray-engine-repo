#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
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
#include "ui_main.h"
#include "xrHemisphere.h"

//------------------------------------------------------------------------------
// !!! использовать prefix если нужно имя !!! (Связано с группами)
//------------------------------------------------------------------------------

/*
// hemi
struct          hemi_data
{
    vector<R_Light>* 	dest;
    R_Light				T;
};
void __stdcall 	hemi_callback(float x, float y, float z, float E, LPVOID P)
{
    hemi_data*	H		= (hemi_data*)P;
    H->T.energy     	= E;
    H->T.direction.set  (x,y,z);
    H->dest->push_back  (H->T);
}
if (g_params.area_quality)      
{
        hemi_data                               h_data;
        h_data.dest                             = dest;
        h_data.T                                = RL;
        h_data.T.diffuse.set    (g_params.area_color);
        xrHemisphereBuild               (g_params.area_quality,FALSE,0.5f,g_params.area_energy_summary,hemi_callback,&h_data);
}
*/
void SceneBuilder::SaveBuild()
{
	CFS_Memory F;
    F.open_chunk	(EB_Version);
    F.Wdword		(XRCL_CURRENT_VERSION);
    F.close_chunk	();

    F.open_chunk	(EB_Parameters);
    F.write			(&Scene.m_LevelOp.m_BuildParams,sizeof(b_params));
    F.close_chunk	();

    F.open_chunk	(EB_Vertices);    
    F.write			(l_vertices,sizeof(b_vertex)*l_vertices_cnt);
    F.close_chunk	();
    
    F.open_chunk	(EB_Faces);
    F.write			(l_faces,sizeof(b_face)*l_faces_cnt);
    F.close_chunk	();

    F.open_chunk	(EB_Materials);
    F.write			(l_materials.begin(),sizeof(b_material)*l_materials.size());
    F.close_chunk	();

    F.open_chunk	(EB_Shaders_Render);
    F.write			(l_shaders.begin(),sizeof(b_shader)*l_shaders.size());
    F.close_chunk	();

    F.open_chunk	(EB_Shaders_Compile);
    F.write			(l_shaders_xrlc.begin(),sizeof(b_shader)*l_shaders_xrlc.size());
    F.close_chunk	();

    F.open_chunk	(EB_Textures);
    F.write			(l_textures.begin(),sizeof(b_texture)*l_textures.size());
    F.close_chunk	();

    F.open_chunk	(EB_Glows);
    F.write			(l_glows.begin(),sizeof(b_glow)*l_glows.size());
    F.close_chunk	();

    F.open_chunk	(EB_Portals);
    F.write			(l_portals.begin(),sizeof(b_portal)*l_portals.size());
    F.close_chunk	();

    F.open_chunk	(EB_Light_control);
    for (vector<sb_light_control>::iterator lc_it=l_light_control.begin(); lc_it!=l_light_control.end(); lc_it++){
    	F.write		(lc_it->name,sizeof(lc_it->name));
    	F.Wdword	(lc_it->data.size());
    	F.write		(lc_it->data.begin(),sizeof(DWORD)*lc_it->data.size());
    }
    F.close_chunk	();

    F.open_chunk	(EB_Light_static);
    F.write			(l_light_static.begin(),sizeof(b_light_static)*l_light_static.size());
    F.close_chunk	();

    F.open_chunk	(EB_Light_dynamic);
    F.write			(l_light_dynamic.begin(),sizeof(b_light_dynamic)*l_light_dynamic.size());
    F.close_chunk	();

    F.open_chunk	(EB_LOD_models);
    F.write			(l_lods.begin(),sizeof(b_lod)*l_lods.size());
    F.close_chunk	();

    AnsiString fn	= "build.prj";
	m_LevelPath.Update(fn);
    F.SaveTo		(fn.c_str(),BUILD_PROJECT_MARK);
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
    l_vertices_cnt 			= 0;
	l_faces_cnt				= 0;
	l_vertices_it 			= 0;
	l_faces_it				= 0;
    _DELETEARRAY			(l_vertices);
    _DELETEARRAY			(l_faces);
    l_lods.clear			();
    l_light_static.clear	();
    l_light_dynamic.clear	();
    l_light_control.clear	();
    l_textures.clear		();
    l_shaders.clear			();
    l_shaders_xrlc.clear	();
    l_materials.clear		();
    l_vnormals.clear		();
    l_glows.clear			();
    l_portals.clear			();
    l_light_keys.clear		();
}

//------------------------------------------------------------------------------
// CEditObject build functions
//------------------------------------------------------------------------------
BOOL SceneBuilder::BuildMesh(const Fmatrix& parent, CEditableObject* object, CEditableMesh* mesh, int sect_num, int lod_id)
{
	BOOL bResult = TRUE;
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
            IntVec& a_lst = mesh->m_Adjs[pt-mesh->m_Points.begin()];
            VERIFY(a_lst.size());
            for (IntIt i_it=a_lst.begin(); i_it!=a_lst.end(); i_it++)
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
		IntVec& face_lst = sp_it->second;
        CSurface* surf = sp_it->first;
		DWORD dwTexCnt = ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
        R_ASSERT(dwTexCnt==1);
	    for (IntIt f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
			st_Face& face = mesh->m_Faces[*f_it];
        	{
                b_face& new_face 	= l_faces[l_faces_it++];
                int m_id			= BuildMaterial(surf,sect_num,lod_id);
                if (m_id<0){
                	bResult 		= FALSE;
                    break;
                }
                new_face.dwMaterial = m_id;
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
                        new_face.t[k].set(vmap.getUV(vm_pt.index));
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
                        new_face.t[k].set(vmap.getUV(vm_pt.index));
                    }
                }
            }
        }
        if (!bResult) break;
    }
    return bResult;
}

BOOL SceneBuilder::BuildObject(CSceneObject* obj){
	CEditableObject *O = obj->GetReference();
    AnsiString temp; temp.sprintf("Building object: %s",obj->Name);
    UI.SetStatus(temp.c_str());

    const Fmatrix& T 	= obj->_Transform();
    // build LOD           
    Fbox bb;                
    Fvector C;
    float r;
    obj->GetBox			(bb);
    bb.getsphere		(C,r);
	int	lod_id 			= BuildObjectLOD(T,O,CalculateSector(C,r));
    if (lod_id==-2)    	return FALSE;
	// parse mesh data
    for(EditMeshIt M=O->FirstMesh();M!=O->LastMesh();M++){
		CSector* S = PortalUtils.FindSector(obj,*M);
	    int sect_num = S?S->sector_num:m_iDefaultSectorNum;
    	if (!BuildMesh(T,O,*M,sect_num,lod_id)) return FALSE;
    }
    return TRUE;
}

//------------------------------------------------------------------------------
// light build functions
//------------------------------------------------------------------------------
int	SceneBuilder::BuildLightControl(CLight* e)
{
	if (l_light_control.empty()){
    	l_light_control.push_back(sb_light_control());
    	sb_light_control& b = l_light_control.back();
        b.name[0] = 0;
    }
	return l_light_control.size()-1;
}

struct SBuildLight{
	Flight	light;
    float	energy;
};
DEFINE_VECTOR(SBuildLight,BLVec,BLIt);

struct SHemiData
{
    BLVec* 		dest;
    SBuildLight	T;
};
void __stdcall 	hemi_callback(float x, float y, float z, float E, LPVOID P)
{
    SHemiData*	H		= (SHemiData*)P;
    H->T.energy     	= E;
    H->T.light.direction.set(x,y,z);
    H->dest->push_back  (H->T);
}
void SceneBuilder::BuildHemiLights()
{
    BLVec 				dest;
    Flight				RL;
    b_params& P			= Scene.m_LevelOp.m_BuildParams;
    // set def params
    RL.type				= D3DLIGHT_DIRECTIONAL;
    RL.diffuse			= P.area_color;
    if (P.area_quality){
        SHemiData		h_data;
        h_data.dest 	= &dest;
        h_data.T.light	= RL;
        xrHemisphereBuild(P.area_quality,FALSE,0.5f,P.area_energy_summary,hemi_callback,&h_data);
        for (BLIt it=dest.begin(); it!=dest.end(); it++){
            l_light_static.push_back(b_light_static());
            b_light_static& sl	= l_light_static.back();
            sl.controller_ID 	= 0; //?
            sl.data			    = it->light;
        }
    }
}
BOOL SceneBuilder::BuildSun(b_light* b, DWORD usage, svector<WORD,16>* sectors)
{
    if (usage&CLight::flAffectStatic){
	    b_params& P			= Scene.m_LevelOp.m_BuildParams;
    	if (!P.area_quality){
        	// single light
	        l_light_static.push_back(b_light_static());
    	    b_light_static& sl	= l_light_static.back();
	        sl.controller_ID 	= b->controller_ID;
    	    sl.data			    = b->data;
        }else{
            // soft light
            float base_h,base_p;
            b->data.direction.getHP(base_h,base_p);

            int samples;
            switch(P.area_quality){
            case 1: samples = 3; break;
            case 2: samples = 4; break;
            default:
            	THROW2("Invalid case.");
            }

            Fcolor color		= b->data.diffuse;
            color.normalize_rgb(b->data.diffuse);
            float energy		= (b->data.diffuse.magnitude_rgb())/float(samples*samples);
            color.mul_rgb		(energy);

            float disp			= deg2rad(P.area_dispersion);
            float da 			= disp/float(samples);
            float mn_h  		= base_h-disp/2;
            float mn_p  		= base_p-disp/2;
            for (int x=0; x<samples; x++){
            	float h = mn_h+x*da;
	            for (int y=0; y<samples; y++){
	            	float p = mn_p+y*da;
                    l_light_static.push_back(b_light_static());
                    b_light_static& sl	= l_light_static.back();
                    sl.controller_ID 	= b->controller_ID;
                    sl.data				= b->data;
                    sl.data.diffuse.set	(color);
                    sl.data.direction.setHP(h,p);
                }
            }
        }
    }
    if (usage&CLight::flAffectDynamic){
        R_ASSERT			(sectors);
		l_light_dynamic.push_back(b_light_dynamic());
        b_light_dynamic& dl	= l_light_dynamic.back();
        dl.controller_ID 	= b->controller_ID;
        dl.data			    = b->data;
        dl.sectors			= *sectors;
    }
	
	return TRUE;
}

BOOL SceneBuilder::BuildPointLight(b_light* b, DWORD usage, svector<WORD,16>* sectors, FvectorVec* soft_points)
{
    if (usage&CLight::flAffectStatic){
    	if (soft_points){
        // make soft light
            Fcolor color		= b->data.diffuse;
            color.normalize_rgb(b->data.diffuse);
            float energy		= (b->data.diffuse.magnitude_rgb())/float(soft_points->size());
            color.mul_rgb		(energy);

            for (DWORD k=0; k<soft_points->size(); k++){
                l_light_static.push_back(b_light_static());
                b_light_static& sl	= l_light_static.back();
                sl.controller_ID 	= b->controller_ID;
                sl.data				= b->data;
                sl.data.diffuse.set	(color);
                sl.data.position.set((*soft_points)[k]);
            }
        }else{
	        // make single light
            l_light_static.push_back(b_light_static());
            b_light_static& sl	= l_light_static.back();
            sl.controller_ID 	= b->controller_ID;
            sl.data			    = b->data;
        }
    }
    if (usage&CLight::flAffectDynamic){
        R_ASSERT			(sectors);
		l_light_dynamic.push_back(b_light_dynamic());
        b_light_dynamic& dl	= l_light_dynamic.back();
        dl.controller_ID 	= b->controller_ID;
        dl.data			    = b->data;
        dl.sectors			= *sectors;
    }
	
	return TRUE;
}

BOOL SceneBuilder::BuildLight(CLight* e)
{
    if (!(e->m_dwFlags&CLight::flAffectStatic)&&!(e->m_dwFlags&CLight::flAffectDynamic))
    	return FALSE;

    b_light	L;
    L.data			= e->m_D3D; 
    L.data.mul		(e->m_Brightness);
    L.controller_ID	= BuildLightControl(e);
    
    if (e->m_dwFlags&CLight::flProcedural){
//	    controllerID= ?;
    }

	svector<WORD,16>* lpSectors;
    if (e->m_dwFlags&CLight::flAffectDynamic){
		svector<WORD,16> sectors;
        lpSectors		= &sectors;
        Fvector& pos 	= e->m_D3D.position;
        float& range 	= e->m_D3D.range;
        if (Scene.ObjCount(OBJCLASS_SECTOR)){
            // test fully and partial inside
            ObjectIt _F = Scene.FirstObj(OBJCLASS_SECTOR);
            ObjectIt _E = Scene.LastObj(OBJCLASS_SECTOR);
            for(;_F!=_E;_F++){
                if (sectors.size()>=16) break;
                CSector* _S=(CSector*)(*_F);
                EVisible vis=_S->Intersect(pos,range);
                if ((vis==fvPartialInside)||(vis==fvFully))
                    sectors.push_back(_S->sector_num);
            }
            // test partial outside
            _F = Scene.FirstObj(OBJCLASS_SECTOR);
            for(;_F!=_E;_F++){
                if (sectors.size()>=16) break;
                CSector* _S=(CSector*)(*_F);
                EVisible vis=_S->Intersect(pos,range);
                if (vis==fvPartialOutside)
                    sectors.push_back(_S->sector_num);
            }
            if (sectors.empty()) return FALSE; 
        }else{
            sectors.push_back(m_iDefaultSectorNum);
        }
    }

    
    switch (e->m_D3D.type){
    case D3DLIGHT_DIRECTIONAL: 	return BuildSun			(&L,e->m_dwFlags,lpSectors);
    case D3DLIGHT_POINT:		return BuildPointLight	(&L,e->m_dwFlags,lpSectors,0);
    default:
    	THROW2("Invalid case.");
	    return FALSE;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Glow build functions
//------------------------------------------------------------------------------
void SceneBuilder::BuildGlow(CGlow* e)
{
	l_glows.push_back(b_glow());
    b_glow& b 		= l_glows.back();
// material
    b_material mtl; ZeroMemory(&mtl,sizeof(mtl));
    int mtl_idx;
    VERIFY(!e->m_ShaderName.IsEmpty());
	mtl.surfidx		= BuildTexture		(e->m_TexName.c_str());
    mtl.shader      = BuildShader		(e->m_ShaderName.c_str());
    mtl.sector		= CalculateSector	(e->PPosition,e->m_Range);
    mtl.shader_xrlc	= -1;
    mtl.lod_id		= -1;

    mtl_idx 		= FindInMaterials(&mtl);
    if (mtl_idx<0){
        l_materials.push_back(mtl);
        mtl_idx 	= l_materials.size()-1;
    }

// fill params
	b.P.set        	(e->PPosition);
    b.size        	= e->m_Range;
	b.dwMaterial   	= mtl_idx;
    b.flags			= 0;	// 0x01 - non scalable
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
        if (!Device.Shader._FindBlender(sh.name)){ 
        	ELog.DlgMsg(mtError,"Can't find engine shader: %s",sh.name);
            return -1;
        }
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
        if (!Device.ShaderXRLC.Get(sh.name)){ 
        	ELog.DlgMsg(mtError,"Can't find compiler shader: %s",sh.name);
            return -1;
        }
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
// lod build functions
//------------------------------------------------------------------------------
int	SceneBuilder::BuildObjectLOD(const Fmatrix& parent, CEditableObject* e, int sector_num)
{
	if (!e->IsFlag(CEditableObject::eoUsingLOD)) return -1;
    AnsiString lod_name;
	R_ASSERT(e->GetLODTextureName(lod_name));

	AnsiString fname = lod_name+AnsiString(".tga");
    if (!Engine.FS.Exist(&Engine.FS.m_Textures,fname.c_str())){
		ELog.DlgMsg(mtError,"Can't find object LOD texture: %s",fname.c_str());
    	return -2;
    }    

    b_material 		mtl;
    mtl.surfidx		= BuildTexture		(lod_name.c_str());
    mtl.shader      = BuildShader		(e->GetLODShaderName());
    mtl.sector		= sector_num;
    mtl.shader_xrlc	= -1;
    mtl.lod_id		= -1;

    int mtl_idx		= FindInMaterials(&mtl);
    if (mtl_idx<0){
        l_materials.push_back(mtl);
        mtl_idx 	= l_materials.size()-1;
    }

    l_lods.push_back(b_lod());
    b_lod& b		= l_lods.back();
    Fvector    		p[4];
    Fvector2 		t[4];
    for (int frame=0; frame<LOD_SAMPLE_COUNT; frame++){
        e->GetLODFrame(frame,p,t,&parent);
        for (int k=0; k<4; k++){ 
            b.faces[frame].v[k].set(p[k]); 
            b.faces[frame].t[k].set(t[k]); 
        }
    }
    b.dwMaterial	= mtl_idx;
    return l_lods.size()-1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// material build functions
//------------------------------------------------------------------------------
int SceneBuilder::FindInMaterials(b_material* m)
{
    for (DWORD i=0; i<l_materials.size(); i++){
        if( (l_materials[i].surfidx		== m->surfidx) 		&&
            (l_materials[i].shader		== m->shader) 		&&
            (l_materials[i].shader_xrlc	== m->shader_xrlc) 	&&
            (l_materials[i].sector		== m->sector)		&&
            (l_materials[i].lod_id		== m->lod_id)) return i;
    }
    return -1;
}
//------------------------------------------------------------------------------

int SceneBuilder::BuildMaterial(CSurface* surf, int sector_num, int lod_id){
    b_material mtl; ZeroMemory(&mtl,sizeof(mtl));
    VERIFY(sector_num>=0);
    int mtl_idx;
	DWORD tex_cnt 	= ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT); VERIFY(tex_cnt==1);
    int en_sh_idx	= BuildShader(surf->_ShaderName());
    int cl_sh_idx	= BuildShaderXRLC(surf->_ShaderXRLCName());
    if ((en_sh_idx<0)||(cl_sh_idx<0)) return -1;
    mtl.shader      = en_sh_idx;
    mtl.shader_xrlc	= cl_sh_idx;
    mtl.sector		= sector_num;
	mtl.surfidx		= BuildTexture(surf->_Texture());
    mtl.lod_id		= lod_id;
    mtl_idx 		= FindInMaterials(&mtl);
    if (mtl_idx<0){
        l_materials.push_back(mtl);
        mtl_idx 	= l_materials.size()-1;
    }
    return mtl_idx;
}
//------------------------------------------------------------------------------

BOOL SceneBuilder::ParseStaticObjects(ObjectList& lst, LPCSTR prefix)
{
	BOOL bResult = TRUE;
    for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
        UI.ProgressInc();
        if (UI.NeedAbort()) break;
        switch((*_F)->ClassID){
        case OBJCLASS_LIGHT:
            bResult = BuildLight((CLight*)(*_F));
            break;
        case OBJCLASS_GLOW:
            BuildGlow((CGlow*)(*_F));
            break;
        case OBJCLASS_PORTAL:
            l_portals.push_back(b_portal());
            BuildPortal(&l_portals.back(),(CPortal*)(*_F));
            break;
        case OBJCLASS_SCENEOBJECT:{
            CSceneObject *obj = (CSceneObject*)(*_F);
            if (!obj->IsDynamic()) bResult = BuildObject(obj);
        }break;
        case OBJCLASS_GROUP:{ 
            CGroupObject* group = (CGroupObject*)(*_F);
            bResult = ParseStaticObjects(group->GetObjects(),group->Name);
        }break;
        }// end switch
        if (!bResult) break;
    }
    return bResult;
}
//------------------------------------------------------------------------------

BOOL SceneBuilder::CompileStatic()
{
	BOOL bResult = TRUE;

    ResetStructures();

	int objcount = Scene.ObjCount();
	if( objcount <= 0 )	return FALSE;

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
// make hemisphere
	BuildHemiLights();
// parse scene
    for(ObjectPairIt it=Scene.FirstClass(); it!=Scene.LastClass(); it++)
        if (!ParseStaticObjects((*it).second)){bResult = FALSE; break;}
	UI.ProgressEnd();

    if (bResult&&!UI.NeedAbort()) SaveBuild();

    ResetStructures();

    return bResult;
}


