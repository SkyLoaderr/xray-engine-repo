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
// !!! ������������ prefix ���� ����� ��� !!! (������� � ��������)
//------------------------------------------------------------------------------

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
    F.write			(l_verts,sizeof(b_vertex)*l_vert_cnt);
    F.close_chunk	();
    
    F.open_chunk	(EB_Faces);
    F.write			(l_faces,sizeof(b_face)*l_face_cnt);
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

    F.open_chunk	(EB_MU_models);
    for (int k=0; k<(int)l_mu_models.size(); k++){
    	b_mu_model&	m= l_mu_models[k];
        // name
        F.WstringZ	(m.name);
        // vertices
        F.Wdword	(m.vert_cnt);
	    F.write		(m.verts,sizeof(b_vertex)*m.vert_cnt);
        // faces
        F.Wdword	(m.face_cnt);
	    F.write		(m.faces,sizeof(b_face)*m.face_cnt);
        // lod_id
        F.Wword		(m.lod_id);
    }
    F.close_chunk	();
    
    F.open_chunk	(EB_MU_refs);
	F.write			(l_mu_refs.begin(),sizeof(b_mu_reference)*l_mu_refs.size());
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
    return m_iDefaultSectorNum; // �� ���������
}

void SceneBuilder::ResetStructures (){
    l_vert_cnt 				= 0;
	l_face_cnt				= 0;
	l_vert_it 				= 0;
	l_face_it				= 0;
    xr_free					(l_verts);
    xr_free					(l_faces);
    for (int k=0; k<(int)l_mu_models.size(); k++){
    	b_mu_model&	m 		= l_mu_models[k];
        xr_free				(m.verts);
        xr_free				(m.faces);
    }
    l_mu_models.clear		();
    l_mu_refs.clear			();
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
BOOL SceneBuilder::BuildMesh(const Fmatrix& parent, CEditableObject* object, CEditableMesh* mesh, int sect_num, 
							b_vertex* verts, int& vert_cnt, int& vert_it, b_face* faces, int& face_cnt, int& face_it)
{
	BOOL bResult = TRUE;
    int point_offs;
    point_offs = vert_it;  // save offset

    // fill vertices
	for (FvectorIt pt_it=mesh->m_Points.begin(); pt_it!=mesh->m_Points.end(); pt_it++){
    	R_ASSERT(vert_it<vert_cnt);
    	parent.transform_tiny(verts[vert_it++],*pt_it);
    }

    if (object->IsDynamic()){
	    // update mesh
	    if (!mesh->m_LoadState.is(CEditableMesh::LS_FNORMALS)) mesh->GenerateFNormals();
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
            R_ASSERT(face_it<face_cnt);
            b_face& first_face 		= faces[face_it++];
        	{
                int m_id			= BuildMaterial(surf,sect_num);
                if (m_id<0){
                	bResult 		= FALSE;
                    break;
                }
                first_face.dwMaterial 		= m_id;
                first_face.dwMaterialGame 	= surf->_GameMtl();
                for (int k=0; k<3; k++){
                    st_FaceVert& fv = face.pv[k];
                    // vertex index
                    R_ASSERT2((fv.pindex+point_offs)<vert_it,"Index out of range");
                    first_face.v[k] = fv.pindex+point_offs;
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
                        first_face.t[k].set(vmap.getUV(vm_pt.index));
                    }
                }
            }

	        if (surf->m_Flags.is(CSurface::sf2Sided)){
		    	R_ASSERT(face_it<face_cnt);
                b_face& second_face 		= faces[face_it++];
                second_face.dwMaterial 		= first_face.dwMaterial;
                second_face.dwMaterialGame 	= first_face.dwMaterialGame;
                for (int k=0; k<3; k++){
                    st_FaceVert& fv = face.pv[2-k];
                    // vertex index
                    second_face.v[k]=fv.pindex+point_offs;
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
                        second_face.t[k].set(vmap.getUV(vm_pt.index));
                    }
                }
            }
        }
        if (!bResult) break;
    }
    return bResult;
}

BOOL SceneBuilder::BuildObject(CSceneObject* obj)
{
	CEditableObject *O = obj->GetReference();
    AnsiString temp; temp.sprintf("Building object: %s",obj->Name);
    UI.SetStatus(temp.c_str());

    const Fmatrix& T 	= obj->_Transform();
	// parse mesh data
    for(EditMeshIt M=O->FirstMesh();M!=O->LastMesh();M++){
		CSector* S = PortalUtils.FindSector(obj,*M);
	    int sect_num = S?S->sector_num:m_iDefaultSectorNum;
    	if (!BuildMesh(T,O,*M,sect_num,l_verts,l_vert_cnt,l_vert_it,l_faces,l_face_cnt,l_face_it)) return FALSE;
    }
    return TRUE;
}

BOOL SceneBuilder::BuildMUObject(CSceneObject* obj)
{
	CEditableObject *O = obj->GetReference();
    AnsiString temp; temp.sprintf("Building object: %s",obj->Name);
    UI.SetStatus(temp.c_str());

    int model_idx		= -1;
    
    for (int k=0; k<(int)l_mu_models.size(); k++){
    	b_mu_model&	m 	= l_mu_models[k];
    	if (0==strcmp(m.name,O->GetName())){
        	model_idx	= k;
            break;
        }
    }

    // detect sector
    CSector* S 			= PortalUtils.FindSector(obj,*O->FirstMesh());
    int sect_num 		= S?S->sector_num:m_iDefaultSectorNum;

    // build model
    if (-1==model_idx){
	    // build LOD           
        int	lod_id 		= BuildObjectLOD(Fidentity,O,sect_num);
        if (lod_id==-2) return FALSE;
        // build model
        model_idx		= l_mu_models.size();
	    l_mu_models.push_back(b_mu_model());
		b_mu_model&	M	= l_mu_models.back();
        M.lod_id		= lod_id;
        int vert_it=0, face_it=0;
        M.face_cnt		= obj->GetFaceCount();
        M.vert_cnt		= obj->GetVertexCount();
        strcpy			(M.name,O->GetName());
        M.verts			= xr_alloc<b_vertex>(M.vert_cnt);
        M.faces			= xr_alloc<b_face>(M.face_cnt);
		// parse mesh data
	    for(EditMeshIt MESH=O->FirstMesh();MESH!=O->LastMesh();MESH++)
	    	if (!BuildMesh(Fidentity,O,*MESH,sect_num,M.verts,M.vert_cnt,vert_it,M.faces,M.face_cnt,face_it)) return FALSE;
    }
    
    l_mu_refs.push_back	(b_mu_reference());
	b_mu_reference&	R	= l_mu_refs.back();
    R.model_index		= model_idx;
    R.transform			= obj->_Transform();
    R.flags.zero		();
	R.sector			= sect_num;

    return TRUE;
}

//------------------------------------------------------------------------------
// light build functions
//------------------------------------------------------------------------------
int	SceneBuilder::BuildLightControl(LPCSTR name)
{
	for (u32 k=0; k<l_light_control.size(); k++){
    	sb_light_control& b = l_light_control[k];
    	if (0==strcmp(b.name,name)) return k;
    }
    l_light_control.push_back(sb_light_control());
    sb_light_control& b = l_light_control.back();
    strcpy(b.name,name);
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
        int control_ID	= BuildLightControl(LCONTROL_HEMI);
        for (BLIt it=dest.begin(); it!=dest.end(); it++){
            l_light_static.push_back(b_light_static());
            b_light_static& sl	= l_light_static.back();
            sl.controller_ID 	= control_ID;
            sl.data			    = it->light;
            sl.data.mul			(it->energy);
        }
    }
}
BOOL SceneBuilder::BuildSun(b_light* b, const Flags32& usage, svector<WORD,16>* sectors)
{
    if (usage.is(CLight::flAffectStatic)){
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
            float sample_energy	= (b->data.diffuse.magnitude_rgb())/float(samples*samples);
            color.mul_rgb		(sample_energy);

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
    if (usage.is(CLight::flAffectDynamic)){
        R_ASSERT			(sectors);
		l_light_dynamic.push_back(b_light_dynamic());
        b_light_dynamic& dl	= l_light_dynamic.back();
        dl.controller_ID 	= b->controller_ID;
        dl.data			    = b->data;
        dl.sectors			= *sectors;
    }
	
	return TRUE;
}

BOOL SceneBuilder::BuildPointLight(b_light* b, const Flags32& usage, svector<WORD,16>* sectors, FvectorVec* soft_points)
{
    if (usage.is(CLight::flAffectStatic)){
    	if (soft_points){
        // make soft light
            Fcolor color		= b->data.diffuse;
            color.normalize_rgb(b->data.diffuse);
            float sample_energy	= (b->data.diffuse.magnitude_rgb())/float(soft_points->size());
            color.mul_rgb		(sample_energy);

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
    if (usage.is(CLight::flAffectDynamic)){
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
    if (!e->m_Flags.is_any(CLight::flAffectStatic|CLight::flAffectDynamic))
    	return FALSE;

    b_light	L;
    L.data			= e->m_D3D; 
    L.data.mul		(e->m_Brightness);
    L.controller_ID	= BuildLightControl("all"); // e->controller_name?e->controller_name:"all"
    
	svector<WORD,16>* lpSectors;
    if (e->m_Flags.is(CLight::flAffectDynamic)){
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
    case D3DLIGHT_DIRECTIONAL: 	return BuildSun			(&L,e->m_Flags,lpSectors);
    case D3DLIGHT_POINT:		return BuildPointLight	(&L,e->m_Flags,lpSectors,0);
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
    mtl.sector		= CalculateSector	(e->PPosition,e->m_fRadius);
    mtl.shader_xrlc	= -1;

    mtl_idx 		= FindInMaterials(&mtl);
    if (mtl_idx<0){
        l_materials.push_back(mtl);
        mtl_idx 	= l_materials.size()-1;
    }

// fill params
	b.P.set        	(e->PPosition);
    b.size        	= e->m_fRadius;
	b.dwMaterial   	= mtl_idx;
    b.flags			= e->m_Flags.is(CGlow::gfFixedSize)?0x01:0x00;	// 0x01 - non scalable
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
	if (!e->m_Flags.is(CEditableObject::eoUsingLOD)) return -1;
    AnsiString lod_name = e->GetLODTextureName();

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
            (l_materials[i].sector		== m->sector)) return i;
    }
    return -1;
}
//------------------------------------------------------------------------------

int SceneBuilder::BuildMaterial(CSurface* surf, int sector_num){
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
        UI.ProgressInc((*_F)->Name);
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
            if (obj->IsStatic()) 		bResult = BuildObject(obj);
            else if (obj->IsMUStatic()) bResult = BuildMUObject(obj);
        }break;
        case OBJCLASS_GROUP:{ 
            CGroupObject* group = (CGroupObject*)(*_F);
            bResult = ParseStaticObjects(group->GetObjects(),group->Name);
        }break;
        }// end switch
        if (!bResult){ 
            ELog.DlgMsg(mtError,"Failed to build object: '%s'",(*_F)->Name);
        	break;
        }
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
    l_vert_cnt	= 0;
    l_face_cnt 	= 0;
	l_vert_it 	= 0;
	l_face_it	= 0;
    ObjectIt _O = Scene.FirstObj(OBJCLASS_SCENEOBJECT);
    ObjectIt _E = Scene.LastObj(OBJCLASS_SCENEOBJECT);
    for(;_O!=_E;_O++){
    	CSceneObject* obj = (CSceneObject*)(*_O);
		if (obj->IsStatic()){
			l_face_cnt	+= obj->GetFaceCount();
    	    l_vert_cnt  += obj->GetVertexCount();
        }
    }
    _O = Scene.FirstObj	(OBJCLASS_GROUP);
    _E = Scene.LastObj	(OBJCLASS_GROUP);
    for(;_O!=_E;_O++){
    	CGroupObject* group = (CGroupObject*)(*_O);
	    ObjectIt _O1 = group->GetObjects().begin();
    	ObjectIt _E1 = group->GetObjects().end();
	    for(;_O1!=_E1;_O1++){
	    	CSceneObject* obj = dynamic_cast<CSceneObject*>(*_O1);
			if (obj&&obj->IsStatic()){
				l_face_cnt	+= obj->GetFaceCount();
    	    	l_vert_cnt  += obj->GetVertexCount();
	        }
        }
    }
	l_faces		= xr_alloc<b_face>(l_face_cnt);
	l_verts		= xr_alloc<b_vertex>(l_vert_cnt);

    UI.ProgressStart(Scene.ObjCount(),"Parse static objects...");
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


