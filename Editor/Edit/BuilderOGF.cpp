//----------------------------------------------------
// file: BuilderRModel.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "fmesh.h"
#include "std_classes.h"
//----------------------------------------------------
// some types
bool SceneBuilder::SaveObjectOGF(const char* name, CEditObject* obj){
	CFS_Memory F;
    m_iDefaultSectorNum = 0;
    UI->ProgressStart(3,"Building OGF...");
	UI->ProgressUpdate(1);
    bool bRes = BuildObjectOGF(F, obj, name, FS.m_GameMeshes);
	UI->ProgressUpdate(2);
    F.SaveTo(name,0);
    UI->ProgressEnd();
    return bRes;
}

bool SceneBuilder::BuildOGFModels(){
	// build sky ogf
    if (Scene->m_SkyDome){
	    AnsiString ogf_name;
        ogf_name.sprintf("%s.ogf",Scene->m_SkyDome->GetName());
        m_LevelPath.Update(ogf_name);
        CFS_Memory F;
        BuildObjectOGF(F, Scene->m_SkyDome, Scene->m_SkyDome->GetName(),m_LevelPath);
        F.SaveTo(ogf_name.c_str(),0);
        // add textures
        AddUniqueTexName(Scene->m_SkyDome);
    }

	// build other scene ogf
/*	int objcount = Scene->ObjCount(OBJCLASS_EDITOBJECT);
	if( objcount <= 0 )
		return true;

	float object_cost = 100.f / (float)objcount;

    AnsiString temp;
	// -- mobile objects --
	ObjectIt i = Scene->FirstObj(OBJCLASS_EDITOBJECT);
	for(;i!=Scene->LastObj(OBJCLASS_EDITOBJECT);i++){
        CEditObject *obj = (CEditObject*)(*i);
        if(obj->IsDynamic() && !obj->IsReference()){
            temp.sprintf("%s.ogf",obj->GetName());
            m_LevelPath.Update(temp);
			CFS_Memory F;
    		BuildObjectOGF(F, obj, obj->GetName());
		    F.SaveTo(temp.c_str(),0);
        }
        frmBuildProgress->AddProgress( object_cost );
	}
*/
	return true;
}

bool fmat_predicate_less(b_face& F1, b_face& F2){
    return F1.dwMaterial<F2.dwMaterial;
}

bool SceneBuilder::BuildObjectOGF( CFS_Base& F, CEditObject *O, const char* name, FSPath& path ){
	CEditObject* obj = (O->IsReference())?O->GetRef():O;
    
	if( obj->MeshCount() == 0 ) return false;

    AnsiString base_name; base_name=ExtractFileName(name); base_name=ChangeFileExt(base_name,"");

    ResetStructures();
    BuildObjectDynamic(obj);

    sort(l_faces,l_faces+l_faces_cnt,fmat_predicate_less);

    if (l_materials.size()>1){
        // save hierrarhy visual
        int cur_mid=l_faces[0].dwMaterial;
        DWORD fid_start=0;
        DWORD cnt;
        DWORD sub_index=0;
    	SSTRLIST children;
        char sub_name[MAX_PATH];
        for (int i=0; i<=l_faces_cnt; i++){
            if ((cur_mid!=l_faces[i].dwMaterial)||((i==l_faces_cnt)&&(i-fid_start))){
                sprintf(sub_name, "%s#%d.ogf", base_name,sub_index);
                children.push_back( SSTR(sub_name) );
                path.Update(sub_name);
				CFS_Memory FM;
                BuildSingleOGF( FM, fid_start, i-fid_start, cur_mid );
				FM.SaveTo(sub_name,0);
                sub_index++;
                cur_mid=l_faces[i].dwMaterial;
                fid_start=i;
            }        
        }

    // write root OGF
        ogf_header header;
        header.format_version 	= xrOGF_FormatVersion;
        header.type 			= MT_HIERRARHY;
        header.flags 			= 0;

        F.write_chunk	(OGF_HEADER,&header,sizeof(header));

        F.open_chunk	(OGF_CHIELDS);
        F.Wdword		(children.size());
        for(DWORD k=0; k<children.size(); k++)
            F.WstringZ	(children[k].filename);
        F.close_chunk	();

        Fbox box;
        obj->GetBox		(box);
        F.write_chunk	(OGF_BBOX,&box,sizeof(box));

        Fvector C;
        float R;
        box.getsphere	(C,R);
        F.open_chunk	(OGF_BSPHERE);
        F.Wvector		(C);
        F.Wfloat 		(R);
        F.close_chunk	();
    }else{
        // save normal visual
        BuildSingleOGF	( F, 0, l_faces_cnt, l_faces[0].dwMaterial );
    }

    WriteTextures();
    
    ResetStructures();
	return true;
}

void SceneBuilder::GetBBox(DWORD st_fid, DWORD cnt, Fbox& box)
{
    VERIFY(cnt);
    box.set( l_vertices[st_fid], l_vertices[st_fid] );
    for(DWORD fid=st_fid; fid<st_fid+cnt; fid++){
        box.modify(l_vertices[l_faces[fid].v[0]] );
        box.modify(l_vertices[l_faces[fid].v[1]] );
        box.modify(l_vertices[l_faces[fid].v[2]] );
    }
}

#pragma pack(push)
#pragma pack(1)
struct ogf_vertex{
    vector<Fvector> point;  // может содержать 1-позицию(обязательно!) и 2-нормаль(если lighting)
    vector<b_uvmap> uv_maps;// переменное кол-во uv-координат
};
struct ogf_face{
    WORD	v[3];
};
#pragma pack(pop)

typedef vector<ogf_vertex> ogf_vertex_list;

bool similar(b_uvmap &t, b_uvmap &p, float eu, float ev){
    return fabsf(t.tu-p.tu)<eu && fabsf(t.tv-p.tv)<ev;
}
int FindVertex(ogf_vertex_list& V, ogf_vertex& v, b_material& mat, vector<b_texture>& textures){
    for (DWORD vid=0; vid<V.size(); vid++){
        if(!V[vid].point[0].similar(v.point[0])) continue; // position
        if(!V[vid].point[1].similar(v.point[1])) continue; // normal

        bool bContinue=false;
        for (int i=0; i<mat.dwTexCount; i++) {
            b_texture   *B = &(textures[mat.surfidx[i]]);
            float		eu = 1.f/float(B->dwWidth );
            float		ev = 1.f/float(B->dwHeight);
            if (!similar(v.uv_maps[i],V[vid].uv_maps[i],eu,ev)) {bContinue=true;continue;}
        }
        if (bContinue) continue;
        return vid;
    }
    return -1;
}

//----------------------------------------------------
bool SceneBuilder::BuildSingleOGF( CFS_Base& FM, DWORD fid_start, DWORD f_cnt, int mid){
	VERIFY( f_cnt );

	// header
	ogf_header header;
	header.format_version = xrOGF_FormatVersion;
	header.type = MT_NORMAL;
	header.flags = 0;

	FM.write_chunk(OGF_HEADER,&header, sizeof(header));

	// bounding box
	Fbox box;
    GetBBox			(fid_start,f_cnt,box);
	FM.write_chunk	(OGF_BBOX,&box,sizeof(box));

	// bounding sphere
    Fvector C;
    float R;
    box.getsphere	(C,R);
    FM.open_chunk	(OGF_BSPHERE);
    FM.Wvector		(C);
    FM.Wfloat 		(R);
	FM.close_chunk	();

	// Texture
    FM.open_chunk	(OGF_TEXTURE);
	AnsiString Tname;
    b_material& mat = l_materials[mid];
	for (WORD t_i=0; t_i<mat.dwTexCount; t_i++){
		if (!Tname.IsEmpty()) Tname+=',';
        Tname		= Tname.TrimRight();
		char fname[MAX_PATH];
        strcpy		(fname,l_textures[mat.surfidx[t_i]].name);
		if (strchr(fname,'.')) *strchr(fname,'.')=0;
		Tname+=fname;
	}
	FM.WstringZ		(Tname.c_str());
 	FM.WstringZ		(l_shaders[mat.shader].name);
	FM.close_chunk	();

    // vertices & faces
    ogf_vertex_list     V;
    vector<ogf_face>    F;

    R_ASSERT2(l_vnormals.size(),"Non-dynamic object convert to dynamic OGF.")
	
    for(DWORD fid=fid_start; fid<fid_start+f_cnt; fid++){
        F.push_back(ogf_face());
        for (DWORD k=0; k<3; k++){
            ogf_vertex v;
            v.point.push_back(l_vertices[l_faces[fid].v[k]]);
            v.point.push_back(l_vnormals[l_faces[fid].v[k]]);
        	for (WORD tid=0; tid<mat.dwTexCount; tid++){
                v.uv_maps.push_back(b_uvmap());
                v.uv_maps.back().tu=l_faces[fid].t[tid][k].tu;
                v.uv_maps.back().tv=l_faces[fid].t[tid][k].tv;
            }
            int v_idx = FindVertex(V, v, mat, l_textures);
            if (v_idx==-1){
                F.back().v[k] = V.size();
                V.push_back(v);
            }else{
                F.back().v[k] = v_idx;
            }
        }
    }
	// faces
	FM.open_chunk	(OGF_INDICES);
	FM.Wdword		(f_cnt*3);
    FM.write		(F.begin(),sizeof(ogf_face)*F.size());
	FM.close_chunk	();

	// point list
    DWORD dwFVF=D3DFVF_XYZ|D3DFVF_NORMAL|(mat.dwTexCount<<D3DFVF_TEXCOUNT_SHIFT);
	FM.open_chunk	(OGF_VERTICES);
    FM.Wdword		(dwFVF);
    FM.Wdword		(V.size());
    for(DWORD p_i=0;p_i<V.size();p_i++){
        FM.write	(V[p_i].point.begin(),V[p_i].point.size()*sizeof(Fvector));
        FM.write	(V[p_i].uv_maps.begin(),mat.dwTexCount*sizeof(b_uvmap));
    }
	FM.close_chunk	();
	return true;
}
