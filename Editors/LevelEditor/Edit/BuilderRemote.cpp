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
#include "xrLevel.h"
#include "ui_main.h"           
#include "xrHemisphere.h"
#include "ResourceManager.h"
#include "ImageManager.h"
#include "Image.h"

#include "ESceneLightTools.h"

//------------------------------------------------------------------------------
// !!! использовать prefix если нужно имя !!! (Связано с группами)
//------------------------------------------------------------------------------

#define LEVEL_LODS_TEX_NAME "level_lods"
#define LEVEL_DI_TEX_NAME "level_stat"

class CSceneStat{
	Fvector	bb_min;
	u32		bb_sx, bb_sz;

    u32 	max_svert;
	U32Vec	svertices;
    u32&	svertex(u32 ix, u32 iz){VERIFY((ix<bb_sx)&&(iz<bb_sz)); return svertices[iz*bb_sx+ix];}

    u32 	max_muvert;
	U32Vec	muvertices;
    u32&	muvertex(u32 ix, u32 iz){VERIFY((ix<bb_sx)&&(iz<bb_sz)); return muvertices[iz*bb_sx+ix];}
public:
	CSceneStat(const Fbox& bb)
    {
    	Fvector				sz;
    	bb.getsize			(sz);
    	bb_sx				= iFloor(sz.x+1.f);
        bb_sz				= iFloor(sz.z+1.f);
        bb_min.set			(bb.min);
        max_svert			= 0;
        svertices.resize	(bb_sx*bb_sz,0);
        max_muvert			= 0;
        muvertices.resize	(bb_sx*bb_sz,0);
    }
    void add_svert(const Fvector& p)
    {
    	u32 ix	= clampr((u32)iFloor(p.x-bb_min.x),(u32)0,bb_sx-1);
    	u32 iz	= clampr((u32)iFloor(p.z-bb_min.z),(u32)0,bb_sz-1);
        u32& v	= svertex(ix,iz); v++;
        if (v>max_svert) max_svert=v;
    }
    void add_muvert(const Fmatrix& parent, const Fvector& _p)
    {
    	Fvector p;
        parent.transform_tiny(p,_p);
    	u32 ix	= clampr((u32)iFloor(p.x-bb_min.x),(u32)0,bb_sx-1);
    	u32 iz	= clampr((u32)iFloor(p.z-bb_min.z),(u32)0,bb_sz-1);
        u32& v	= muvertex(ix,iz); v++;
        if (v>max_muvert) max_muvert=v;
    }
    bool flush(LPCSTR fn)
    {
    	// flush image
    	u32 sx=bb_sx, sz=bb_sz;
        U32Vec data	(sx*sz);
    	// prepare vertex info
        // find max
        u32 ix,iz;
        u32 total_svert=0;
        u32 total_muvert=0;
        for (ix=0; ix<bb_sx; ix++){
	        for (iz=0; iz<bb_sz; iz++){
            	total_svert			+= svertex(ix,iz);
            	total_muvert		+= muvertex(ix,iz);
                u8 v_s 				= iFloor(float(svertex(ix,iz))/float(max_svert)*255.f+0.5f);
                u8 v_mu 			= iFloor(float(muvertex(ix,iz))/float(max_muvert)*255.f+0.5f);
                data[iz*bb_sx+ix] 	= color_rgba(v_s,v_mu,0,0);
            }
        }
        
        AnsiString image_name = AnsiString(fn)+".tga";
        CImage* I 	= xr_new<CImage>();
        I->Create	(sx,sz,data.begin());
        I->Vflip	();
        I->SaveTGA	(image_name.c_str());
        xr_delete	(I);

        // flush text
        AnsiString txt_name = AnsiString(fn)+".txt";
        IWriter* F	= FS.w_open(txt_name.c_str());
        F->w_string	(AnsiString().sprintf("Map size X x Z:            [%d x %d]",bb_sx,bb_sz).c_str());
        F->w_string	(AnsiString().sprintf("Max static vertex per m^2: %d",max_svert).c_str());
        F->w_string	(AnsiString().sprintf("Total static vertices:     %d",total_svert).c_str());
        F->w_string	(AnsiString().sprintf("Max mu vertex per m^2:     %d",max_muvert).c_str());
        F->w_string	(AnsiString().sprintf("Total mu vertices:         %d",total_muvert).c_str());
        FS.w_close	(F);

        return true;
    }
};

void SceneBuilder::SaveBuild()
{
    std::string fn	= MakeLevelPath("build.prj");
	IWriter* F		= FS.w_open(fn.c_str());
    
    F->open_chunk	(EB_Version);
    F->w_u32   		(XRCL_CURRENT_VERSION);
    F->close_chunk	();

    F->open_chunk	(EB_Parameters);
    F->w	  		(&Scene->m_LevelOp.m_BuildParams,sizeof(b_params));
    F->close_chunk	();

    F->open_chunk	(EB_Vertices);
    F->w		  	(l_verts,sizeof(b_vertex)*l_vert_it); 	//. l_vert_cnt
    F->close_chunk	();

    F->open_chunk	(EB_Faces);
    F->w		  	(l_faces,sizeof(b_face)*l_face_it); 	//. l_face_cnt
    F->close_chunk	();

    F->open_chunk	(EB_Materials);
    F->w	   		(l_materials.begin(),sizeof(b_material)*l_materials.size());
    F->close_chunk	();

    F->open_chunk	(EB_Shaders_Render);
    F->w			(l_shaders.begin(),sizeof(b_shader)*l_shaders.size());
    F->close_chunk	();

    F->open_chunk	(EB_Shaders_Compile);
    F->w			(l_shaders_xrlc.begin(),sizeof(b_shader)*l_shaders_xrlc.size());
    F->close_chunk	();

    F->open_chunk	(EB_Textures);
    F->w			(l_textures.begin(),sizeof(b_texture)*l_textures.size());
    F->close_chunk	();

    F->open_chunk 	(EB_Glows);
    F->w			(l_glows.begin(),sizeof(b_glow)*l_glows.size());
    F->close_chunk	();

    F->open_chunk	(EB_Portals);
    F->w			(l_portals.begin(),sizeof(b_portal)*l_portals.size());
    F->close_chunk	();

    F->open_chunk	(EB_Light_control);
    for (xr_vector<sb_light_control>::iterator lc_it=l_light_control.begin(); lc_it!=l_light_control.end(); lc_it++){
    	F->w		(lc_it->name,sizeof(lc_it->name));
    	F->w_u32 	(lc_it->data.size());
    	F->w	 	(lc_it->data.begin(),sizeof(u32)*lc_it->data.size());
    }
    F->close_chunk	();

    F->open_chunk	(EB_Light_static);
    F->w		 	(l_light_static.begin(),sizeof(b_light_static)*l_light_static.size());
    F->close_chunk	();

    F->open_chunk	(EB_Light_dynamic);
    F->w		  	(l_light_dynamic.begin(),sizeof(b_light_dynamic)*l_light_dynamic.size());
    F->close_chunk	();

    F->open_chunk	(EB_LOD_models);
    for (int k=0; k<(int)l_lods.size(); k++)
	    F->w	  	(&l_lods[k].lod,sizeof(b_lod));
    F->close_chunk	();

    F->open_chunk	(EB_MU_models);
    for (k=0; k<(int)l_mu_models.size(); k++){
    	b_mu_model&	m= l_mu_models[k];
        // name
        F->w_stringZ(m.name);
        // vertices
        F->w_u32	(m.vert_cnt);
	    F->w		(m.verts,sizeof(b_vertex)*m.vert_cnt);
        // faces
        F->w_u32	(m.face_cnt);
	    F->w		(m.faces,sizeof(b_face)*m.face_cnt);
        // lod_id
        F->w_u16	(m.lod_id);
    }
    F->close_chunk	();

    F->open_chunk	(EB_MU_refs);
	F->w			(l_mu_refs.begin(),sizeof(b_mu_reference)*l_mu_refs.size());
    F->close_chunk	();

    FS.w_close		(F);
}

int SceneBuilder::CalculateSector(const Fvector& P, float R)
{
    ObjectIt _F = Scene->FirstObj(OBJCLASS_SECTOR);
    ObjectIt _E = Scene->LastObj(OBJCLASS_SECTOR);
    for(;_F!=_E;_F++){
    	CSector* _S=(CSector*)(*_F);
        EVisible vis=_S->Intersect(P,R);
        if ((vis==fvPartialInside)||(vis==fvFully))
        	if (_S->sector_num!=m_iDefaultSectorNum) return _S->sector_num;
	}
    return m_iDefaultSectorNum; // по умолчанию
}

void SceneBuilder::Clear ()
{
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
    xr_delete				(l_scene_stat);
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
        CSurface* surf 		= sp_it->first;
        int m_id			= BuildMaterial(surf,sect_num);
		int gm_id			= surf->_GameMtl(); 
        if (m_id<0)			{ bResult = FALSE; break; }
        if (gm_id<0){ 
        	ELog.DlgMsg		(mtError,"Surface: '%s' contains bad game material.",surf->_Name());
        	bResult 		= FALSE; 
            break; 
        }
        SGameMtl* M = GMLib.GetMaterialByID(gm_id);
        if (0==M){
        	ELog.DlgMsg		(mtError,"Surface: '%s' contains undefined game material.",surf->_Name());
        	bResult 		= FALSE; 
            break; 
        }
        if (M->Flags.is(SGameMtl::flBreakable)){
        	ELog.Msg		(mtInformation,"Surface: '%s' contains breakable game material.",surf->_Name());
            continue;
        }
        if (M->Flags.is(SGameMtl::flClimable)){
        	ELog.Msg		(mtInformation,"Surface: '%s' contains climable game material.",surf->_Name());
            continue;
        }
        if (M->Flags.is(SGameMtl::flDynamic)){
        	ELog.DlgMsg		(mtError,"Surface: '%s' contains non-static game material.",surf->_Name());
        	bResult 		= FALSE; 
            break; 
        }
		u32 dwTexCnt 		= ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
        if (dwTexCnt!=1){ 
        	ELog.DlgMsg		(mtError,"Surface: '%s' contains more than 1 texture refs.",surf->_Name());
        	bResult 		= FALSE; 
            break; 
        }
	    for (IntIt f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
			st_Face& face = mesh->m_Faces[*f_it];
            R_ASSERT(face_it<face_cnt);
            b_face& first_face 		= faces[face_it++];
        	{
                first_face.dwMaterial 		= m_id;
                first_face.dwMaterialGame 	= gm_id; 
                for (int k=0; k<3; k++){
                    st_FaceVert& fv = face.pv[k];
                    // vertex index
                    R_ASSERT2((fv.pindex+point_offs)<vert_it,"Index out of range");
                    first_face.v[k] = fv.pindex+point_offs;
                    // uv maps
                    int offs = 0;
                    for (u32 t=0; t<dwTexCnt; t++){
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
                    for (u32 t=0; t<dwTexCnt; t++){
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
    UI->SetStatus(temp.c_str());

    const Fmatrix& T 	= obj->_Transform();
	// parse mesh data
    for(EditMeshIt M=O->FirstMesh();M!=O->LastMesh();M++){
		CSector* S = PortalUtils.FindSector(obj,*M);
	    int sect_num = S?S->sector_num:m_iDefaultSectorNum;
    	if (!BuildMesh(T,O,*M,sect_num,l_verts,l_vert_cnt,l_vert_it,l_faces,l_face_cnt,l_face_it)) return FALSE;
        // fill DI vertices
        for (FvectorIt pt_it=(*M)->m_Points.begin(); pt_it!=(*M)->m_Points.end(); pt_it++){
        	Fvector v; T.transform_tiny(v,*pt_it);
            l_scene_stat->add_svert(v);
        }
    }
    return TRUE;
}

BOOL SceneBuilder::BuildMUObject(CSceneObject* obj)
{
	CEditableObject *O = obj->GetReference();
    AnsiString temp; temp.sprintf("Building object: %s",obj->Name);
    UI->SetStatus(temp.c_str());

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

    // scene stats
    b_mu_model& M		= l_mu_models[model_idx];
    for (u32 mu_vi=0; mu_vi<(u32)M.vert_cnt; mu_vi++)
    	l_scene_stat->add_muvert(obj->_Transform(),M.verts[mu_vi]);
    
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
void SceneBuilder::BuildHemiLights(u8 quality, LPCSTR lcontrol)
{
    BLVec 				dest;
    Flight				RL;
    // set def params
    RL.type				= D3DLIGHT_DIRECTIONAL;
    RL.diffuse.set		(1.f,1.f,1.f,1.f);
    if (quality){
        SHemiData		h_data;
        h_data.dest 	= &dest;
        h_data.T.light	= RL;
        xrHemisphereBuild(quality,2.f,hemi_callback,&h_data);
        int control_ID	= BuildLightControl(lcontrol);
        for (BLIt it=dest.begin(); it!=dest.end(); it++){
            l_light_static.push_back(b_light_static());
            b_light_static& sl	= l_light_static.back();
            sl.controller_ID 	= control_ID;
            sl.data			    = it->light;
            sl.data.mul			(it->energy);
        }
    }
}
BOOL SceneBuilder::BuildSun(u8 quality, Fvector2 dir)
{
    int controller_ID		= BuildLightControl(LCONTROL_SUN);
    // static
    {
        // soft light
        int samples;
        switch(quality){
        case 1: samples = 3; break;
        case 2: samples = 4; break;
        case 3: samples = 7; break;
        default:
            THROW2("Invalid case.");
        }

        Fcolor color;		color.set(1.f,1.f,1.f,1.f);
        float sample_energy	= 1.f/float(samples*samples);
        color.mul_rgb		(sample_energy);

        float disp			= deg2rad(3.f); // dispersion of sun
        float da 			= disp/float(samples);
        float mn_x  		= dir.x-disp/2;
        float mn_y  		= dir.y-disp/2;
        for (int x=0; x<samples; x++){
            float x = mn_x+x*da;
            for (int y=0; y<samples; y++){
                float y = mn_y+y*da;
                l_light_static.push_back(b_light_static());
                b_light_static& sl	= l_light_static.back();
                sl.controller_ID 	= controller_ID;
                sl.data.type		= D3DLIGHT_DIRECTIONAL;
                sl.data.position.set(0,0,0);
                sl.data.diffuse.set	(color);
                sl.data.direction.setHP(y,x);
            }
        }
    }
    // dynamic
    {
		l_light_dynamic.push_back(b_light_dynamic());
        b_light_dynamic& dl	= l_light_dynamic.back();
        dl.controller_ID 	= controller_ID;
        dl.data.type		= D3DLIGHT_DIRECTIONAL;
        dl.data.position.set(0,0,0);
        dl.data.diffuse.set	(1.f,1.f,1.f,1.f);
        dl.data.direction.setHP(dir.y,dir.x);
    }

	return TRUE;
}

BOOL SceneBuilder::BuildPointLight(b_light* b, const Flags32& usage, svector<WORD,16>* sectors, FvectorVec* soft_points, const Fmatrix* soft_transform)
{
    if (usage.is(ELight::flAffectStatic)){
    	if (soft_points&&!soft_points->empty()){
        	R_ASSERT(soft_transform);
        // make soft light
            Fcolor color		= b->data.diffuse;
            color.normalize_rgb	(b->data.diffuse);
            float sample_energy	= (b->data.diffuse.magnitude_rgb())/float(soft_points->size());
            color.mul_rgb		(sample_energy);

            for (u32 k=0; k<soft_points->size(); k++){
                l_light_static.push_back(b_light_static());
                b_light_static& sl	= l_light_static.back();
                sl.controller_ID 	= b->controller_ID;
                sl.data				= b->data;
                sl.data.diffuse.set	(color);
                soft_transform->transform_tiny(sl.data.position,(*soft_points)[k]);
            }
        }else{
	        // make single light
            l_light_static.push_back(b_light_static());
            b_light_static& sl	= l_light_static.back();
            sl.controller_ID 	= b->controller_ID;
            sl.data			    = b->data;
        }
    }
    if (usage.is(ELight::flAffectDynamic)){
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
    if (!e->m_Flags.is_any(ELight::flAffectStatic|ELight::flAffectDynamic))
    	return FALSE;

    if (!e->GetLControlName()){
    	ELog.Msg(mtError,"Invalid light control name: light '%s'.",e->Name);
    	return FALSE;
    }
        
    b_light	L;
    L.data.type					= e->m_Type;
    L.data.diffuse.mul_rgb		(e->m_Color,e->m_Brightness);
    L.data.position.set			(e->PPosition);
    Fvector dir;    dir.setHP	(e->PRotation.y,e->PRotation.x);
    L.data.direction.set		(dir);
    L.data.range				= e->m_Range;
    L.data.attenuation0			= e->m_Attenuation0;
    L.data.attenuation1			= e->m_Attenuation1;
    L.data.attenuation2			= e->m_Attenuation2;
    L.data.phi					= e->m_Cone;
    
    L.controller_ID	= BuildLightControl(e->GetLControlName()); //BuildLightControl(LCONTROL_STATIC); 

	svector<WORD,16>* lpSectors;
    if (e->m_Flags.is(ELight::flAffectDynamic)){
		svector<WORD,16> sectors;
        lpSectors		= &sectors;
        Fvector& pos 	= e->PPosition;
        float& range 	= e->m_Range;
        if (Scene->ObjCount(OBJCLASS_SECTOR)){
            // test fully and partial inside
            ObjectIt _F = Scene->FirstObj(OBJCLASS_SECTOR);
            ObjectIt _E = Scene->LastObj(OBJCLASS_SECTOR);
            for(;_F!=_E;_F++){
                if (sectors.size()>=16) break;
                CSector* _S=(CSector*)(*_F);
                EVisible vis=_S->Intersect(pos,range);
                if ((vis==fvPartialInside)||(vis==fvFully))
                    sectors.push_back(_S->sector_num);
            }
            // test partial outside
            _F = Scene->FirstObj(OBJCLASS_SECTOR);
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


    switch (e->m_Type){
    case ELight::ltPoint:		return BuildPointLight	(&L,e->m_Flags,lpSectors,e->m_FuzzyData?&e->m_FuzzyData->m_Positions:0,&e->_Transform());
    default:
    	THROW2("Invalid light type.");
	    return FALSE;
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Glow build functions
//------------------------------------------------------------------------------
BOOL SceneBuilder::BuildGlow(CGlow* e)
{
	l_glows.push_back(b_glow());
    b_glow& b 		= l_glows.back();
// material
    b_material mtl; ZeroMemory(&mtl,sizeof(mtl));
    int mtl_idx;
    VERIFY			(e->m_ShaderName.size());
	mtl.surfidx		= BuildTexture		(*e->m_TexName);		
    mtl.shader      = BuildShader		(*e->m_ShaderName);
    mtl.sector		= CalculateSector	(e->PPosition,e->m_fRadius);
    mtl.shader_xrlc	= -1;
    if ((u16(-1)==mtl.surfidx)||(u16(-1)==mtl.shader)) return FALSE;

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
    return TRUE;
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
    for (u32 i=0; i<l_shaders.size(); i++)
        if(strcmp(l_shaders[i].name, s->name)==0)return i;
    return -1;
}
//------------------------------------------------------------------------------

int SceneBuilder::BuildShader(const char * s)
{
    b_shader sh;
    strcpy(sh.name,s);
    int sh_id = FindInShaders(&sh);
    if (sh_id<0){
        if (!Device.Resources->_FindBlender(sh.name)){
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
    for (u32 i=0; i<l_shaders_xrlc.size(); i++)
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
    for (u32 i=0; i<l_textures.size(); i++)
    	if(stricmp(l_textures[i].name,name)==0) return i;
    return -1;
}
//------------------------------------------------------------------------------

int SceneBuilder::BuildTexture(const char* name)
{
	if (!(name&&name[0])){
		ELog.DlgMsg(mtError,"Invalid texture name found.");
        return -1;
    }
    int tex_idx     	= FindInTextures(name);
    if(tex_idx<0){
		b_texture       tex;
        ZeroMemory(&tex,sizeof(tex));
		strcpy          (tex.name,name);
    	l_textures.push_back(tex);
		tex_idx         = l_textures.size()-1;
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
    std::string lod_name = e->GetLODTextureName();

	std::string fname = lod_name+".tga";
    if (!FS.exist(_textures_,fname.c_str())){
		ELog.DlgMsg(mtError,"Can't find object LOD texture: %s",fname.c_str());
    	return -2;
    }

    b_material 		mtl;
    mtl.surfidx		= BuildTexture		(LEVEL_LODS_TEX_NAME);//lod_name.c_str());
    mtl.shader      = BuildShader		(e->GetLODShaderName());
    mtl.sector		= sector_num;
    mtl.shader_xrlc	= -1;
    if ((u16(-1)==mtl.surfidx)||(u16(-1)==mtl.shader)) return -2;

    int mtl_idx		= FindInMaterials(&mtl);
    if (mtl_idx<0){
        l_materials.push_back(mtl);
        mtl_idx 	= l_materials.size()-1;
    }

    l_lods.push_back(e_b_lod());
    e_b_lod& b		= l_lods.back();
    Fvector    		p[4];
    Fvector2 		t[4];
    for (int frame=0; frame<LOD_SAMPLE_COUNT; frame++){
        e->GetLODFrame(frame,p,t,&parent);
        for (int k=0; k<4; k++){
            b.lod.faces[frame].v[k].set(p[k]);
            b.lod.faces[frame].t[k].set(t[k]);
        }
    }
    b.lod.dwMaterial= mtl_idx;
    b.tex_name		= fname.c_str();
    return l_lods.size()-1;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// material build functions
//------------------------------------------------------------------------------
int SceneBuilder::FindInMaterials(b_material* m)
{
    for (u32 i=0; i<l_materials.size(); i++){
        if( (l_materials[i].surfidx		== m->surfidx) 		&&
            (l_materials[i].shader		== m->shader) 		&&
            (l_materials[i].shader_xrlc	== m->shader_xrlc) 	&&
            (l_materials[i].sector		== m->sector)) return i;
    }
    return -1;
}
//------------------------------------------------------------------------------

int SceneBuilder::BuildMaterial(CSurface* surf, int sector_num)
{
    b_material mtl; ZeroMemory(&mtl,sizeof(mtl));
    VERIFY(sector_num>=0);
    int mtl_idx;
	u32 tex_cnt 	= ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT); VERIFY(tex_cnt==1);
    mtl.shader      = BuildShader(surf->_ShaderName());
    mtl.shader_xrlc	= BuildShaderXRLC(surf->_ShaderXRLCName());
    mtl.sector		= sector_num;
	mtl.surfidx		= BuildTexture(surf->_Texture());
    if ((u16(-1)==mtl.shader)||(u16(-1)==mtl.shader_xrlc)||(u16(-1)==mtl.surfidx)) return -1;

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
    SPBItem* pb	= UI->PBStart(lst.size(),"Parse static objects...");
    for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
        UI->PBInc(pb,(*_F)->Name);
        if (UI->NeedAbort()) break;
        switch((*_F)->ClassID){
        case OBJCLASS_LIGHT:
            bResult = BuildLight((CLight*)(*_F));
            break;
        case OBJCLASS_GLOW:
            bResult = BuildGlow((CGlow*)(*_F));
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
    UI->PBEnd(pb);
    return bResult;
}
//------------------------------------------------------------------------------

BOOL SceneBuilder::CompileStatic()
{
	BOOL bResult = TRUE;

    Clear		();

	int objcount = Scene->ObjCount();
	if( objcount <= 0 )	return FALSE;

// compute vertex/face count
    l_vert_cnt	= 0;
    l_face_cnt 	= 0;
	l_vert_it 	= 0;
	l_face_it	= 0;
    ObjectIt _O = Scene->FirstObj(OBJCLASS_SCENEOBJECT);
    ObjectIt _E = Scene->LastObj(OBJCLASS_SCENEOBJECT);
    for(;_O!=_E;_O++){
    	CSceneObject* obj = (CSceneObject*)(*_O);
		if (obj->IsStatic()){
			l_face_cnt	+= obj->GetFaceCount();
    	    l_vert_cnt  += obj->GetVertexCount();
        }
    }
    _O = Scene->FirstObj	(OBJCLASS_GROUP);
    _E = Scene->LastObj	(OBJCLASS_GROUP);
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

    l_scene_stat= xr_new<CSceneStat>(m_LevelBox);

// make hemisphere
	ESceneLightTools* lt = dynamic_cast<ESceneLightTools*>(Scene->GetOTools(OBJCLASS_LIGHT));
    LPCSTR h_control	= *lt->FindLightControl(lt->m_HemiControl)->name;
	BuildHemiLights		(lt->m_HemiQuality,h_control);
    if (0!=strcmp(LCONTROL_HEMI,h_control))
		BuildHemiLights	(lt->m_HemiQuality,LCONTROL_HEMI);
// make sun
	BuildSun		(lt->m_SunShadowQuality,lt->m_SunShadowDir);
// parse scene
    SPBItem* pb = UI->PBStart(Scene->ObjCount(),"Parse scene objects...");
    SceneToolsMapPairIt t_it 	= Scene->FirstTools();
    SceneToolsMapPairIt t_end 	= Scene->LastTools();
    for (; t_it!=t_end; t_it++)
        if (t_it->second){
            ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(t_it->second);
            if (mt&&!ParseStaticObjects(mt->GetObjects())){bResult = FALSE; break;}
        }
	UI->PBEnd(pb);
// process lods
	if (bResult&&!l_lods.empty()){
        SPBItem* pb = UI->PBStart(l_lods.size()*2,"Merge LOD textures...");
        Fvector2Vec			offsets;
        Fvector2Vec			scales;
        boolVec				rotated;
        U32Vec				remap;
        RStringVec 			textures;
        for (int k=0; k<(int)l_lods.size(); k++){
            textures.push_back(l_lods[k].tex_name);
	        UI->PBInc(pb);
        }                           
        std::string fn 		= MakeLevelPath(LEVEL_LODS_TEX_NAME);
        if (1==ImageLib.CreateMergedTexture(textures,fn.c_str(),STextureParams::tfDXT5,512,2048,256,1024,offsets,scales,rotated,remap)){
	        for (k=0; k<(int)l_lods.size(); k++){        
	            e_b_lod& l	= l_lods[k];         
                for (u32 f=0; f<8; f++){
                	for (u32 t=0; t<4; t++){
                    	Fvector2& uv = l.lod.faces[f].t[t];
                        u32 id 		 = remap[k];
                    	ImageLib.MergedTextureRemapUV(uv.x,uv.y,uv.x,uv.y,offsets[id],scales[id],rotated[id]);
                    }
                }
		        UI->PBInc(pb);
			}
        }else{
            ELog.DlgMsg		(mtError,"Failed to build merged LOD texture.");
        	bResult			= FALSE;
        }
        UI->PBEnd(pb);
    }

// save build    
    if (bResult&&!UI->NeedAbort()) SaveBuild();

    return bResult;
}

BOOL SceneBuilder::BuildSceneStat()
{
    std::string dest_name = MakeLevelPath(LEVEL_DI_TEX_NAME);
    return l_scene_stat->flush(dest_name.c_str());
}
