#include "stdafx.h"      
#pragma hdrstop          
                              
#include "Builder.h"  
#include "ImageManager.h"
//------------------------------------------------------------------------------
#define LEVEL_LODS_TEX_NAME "level_lods"
#define LEVEL_LODS_NRM_NAME "level_lods_nm"
//------------------------------------------------------------------------------

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
        M.face_cnt		= face_it;
        M.vert_cnt		= vert_it;
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
// lod build functions
//------------------------------------------------------------------------------
static const float 	LOD_CALC_SAMPLES 		= 5.f;
static const s32	LOD_CALC_SAMPLES_LIM 	= LOD_CALC_SAMPLES/2;

int	SceneBuilder::BuildObjectLOD(const Fmatrix& parent, CEditableObject* E, int sector_num)
{
	if (!E->m_Flags.is(CEditableObject::eoUsingLOD)) return -1;
    xr_string lod_name = E->GetLODTextureName();

    b_material 		mtl;
    mtl.surfidx		= BuildTexture		(LEVEL_LODS_TEX_NAME);
    mtl.shader      = BuildShader		(E->GetLODShaderName());
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
        E->GetLODFrame(frame,p,t,&parent);
        for (int k=0; k<4; k++){
            b.lod.faces[frame].v[k].set(p[k]);
            b.lod.faces[frame].t[k].set(t[k]);
        }
    }
    b.lod.dwMaterial= mtl_idx;
    b.lod_name		= lod_name.c_str();

    // make lod
    Fbox bb						= E->GetBox();
    E->m_Flags.set				(CEditableObject::eoUsingLOD,FALSE);
    object_for_render			= E;
    ImageLib.CreateLODTexture	(bb, b.data, LOD_IMAGE_SIZE,LOD_IMAGE_SIZE,LOD_SAMPLE_COUNT);
    E->m_Flags.set				(CEditableObject::eoUsingLOD,TRUE);

    // build lod normals
    b.ndata.resize				(LOD_IMAGE_SIZE*LOD_IMAGE_SIZE*LOD_SAMPLE_COUNT);
    Fvector C,S;
    Fmatrix M,Mi;
    bb.getradius				(S);
    bb.getcenter				(C);
    float dW 					= _max(S.x,S.z)/(LOD_IMAGE_SIZE/2);
    float dH 					= S.y/(LOD_IMAGE_SIZE/2);
	XRC.ray_options				(CDB::OPT_CULL);
    for (u32 sample_idx=0; sample_idx<LOD_SAMPLE_COUNT; sample_idx++){
    	float angle 			= sample_idx*(PI_MUL_2/LOD_SAMPLE_COUNT);
        M.setXYZ				(0,angle,0);
        M.translate_over		(C);
        Mi.invert				(M);
	    for (s32 iH=0; iH<LOD_IMAGE_SIZE; iH++){
        	float Y				= (iH-(LOD_IMAGE_SIZE-1)/2)*dH;
		    for (s32 iW=0; iW<LOD_IMAGE_SIZE; iW++){
	        	float X			= (iW-LOD_IMAGE_SIZE/2)*dW;

                u32& base_c		= b.data[(LOD_IMAGE_SIZE-iH-1)*LOD_SAMPLE_COUNT*LOD_IMAGE_SIZE+LOD_IMAGE_SIZE*sample_idx+iW];
                u32& tgt_n		= b.ndata[(LOD_IMAGE_SIZE-iH-1)*LOD_SAMPLE_COUNT*LOD_IMAGE_SIZE+LOD_IMAGE_SIZE*sample_idx+iW];
                u8 base_a		= color_get_A	(base_c);
                base_c			= 0x00000000;
                FvectorVec		n_vec;
                FvectorVec		c_vec;
                for (s32 iiH=-LOD_CALC_SAMPLES_LIM; iiH<=LOD_CALC_SAMPLES_LIM; iiH++){
                	float dY	= iiH*(dH/LOD_CALC_SAMPLES);
                    for (s32 iiW=-LOD_CALC_SAMPLES_LIM; iiW<=LOD_CALC_SAMPLES_LIM; iiW++){
	                	float dX= iiW*(dW/LOD_CALC_SAMPLES);
                        S.set			(X+dX,Y+dY,0);
                        M.transform_tiny(S);
                        S.mad			(M.k,-1000.f);
                        SPickQuery 		PQ;
                        PQ.prepare_rq	(S,M.k,2000.f,CDB::OPT_CULL);
                        E->RayQuery		(Fidentity,Fidentity,PQ);
                        if (PQ.r_count()){
                            PQ.r_sort();
                            Fvector N	= {0,0,0};
                            Fcolor C	= {0,0,0,0};
                            for (s32 k=PQ.r_count()-1; k>=0; k--){
                                SPickQuery::SResult* R 	= PQ.r_begin()+k;
                                CSurface* surf			= R->e_mesh->GetSurfaceByFaceID(R->tag); VERIFY(surf);
                                const Fvector2*			cuv[3];
                                R->e_mesh->GetFaceTC	(R->tag,cuv);
                                Shader_xrLC* c_sh		= Device.ShaderXRLC.Get(surf->_ShaderXRLCName());
                                if (!c_sh->flags.bRendering) continue;
                                if (0==surf->m_ImageData)surf->CreateImageData();

                                // barycentric coords
                                // note: W,U,V order
                                Fvector B;
                                B.set	(1.0f - R->u - R->v,R->u,R->v);

                                // calc UV
                                Fvector2	uv;
                                uv.x = cuv[0]->x*B.x + cuv[1]->x*B.y + cuv[2]->x*B.z;
                                uv.y = cuv[0]->y*B.x + cuv[1]->y*B.y + cuv[2]->y*B.z;

                                int U = iFloor(uv.x*float(surf->m_ImageData->w) + .5f);
                                int V = iFloor(uv.y*float(surf->m_ImageData->h)+ .5f);
                                U %= surf->m_ImageData->w;	if (U<0) U+=surf->m_ImageData->w;
                                V %= surf->m_ImageData->h;	if (V<0) V+=surf->m_ImageData->h;

                                // color
                                Fcolor Cn;
                                Cn.set			(surf->m_ImageData->layers.back()[V*surf->m_ImageData->w+U]);
                                float a			= Cn.a;
                                C.lerp			(C,Cn,a);

                                // normal
                                Fvector Nn;
                                Nn.mknormal		(R->verts[0],R->verts[1],R->verts[2]);
                                Nn.mul			(a);

                                N.mul			(1.f-a);
                                N.add			(Nn);
                            }
                            float n_mag			= N.magnitude();
                            if (!fis_zero(n_mag,EPS))
                                n_vec.push_back	(N.div(n_mag));

                            Fvector x; x.set	(C.r,C.g,C.b);
                            c_vec.push_back		(x);
                        }
                    }
                }
                Fvector C={0,0,0};
                for (FvectorIt c_it=c_vec.begin(); c_it!=c_vec.end(); c_it++) C.add(*c_it);
                C.div				(c_vec.size());
                C.mul				(255.f);
                base_c				= color_rgba(iFloor(C.x),iFloor(C.y),iFloor(C.z),base_a);

                Fvector N={0,0,0};
                for (FvectorIt it=n_vec.begin(); it!=n_vec.end(); it++) N.add(*it);
                N.div				(n_vec.size());
                N.normalize_safe	();
                Mi.transform_dir	(N);
                N.mul				(0.5f);
                N.add				(0.5f);
                N.mul				(255.f);
                tgt_n				= color_rgba(iFloor(N.x),iFloor(N.y),iFloor(N.z),base_a);
            }
        }
    }

    return l_lods.size()-1;
}
//------------------------------------------------------------------------------

