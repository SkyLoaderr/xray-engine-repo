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
bool SceneBuilder::SaveObjectDO(const char* name, CEditObject* obj){
	CFS_Memory F;
    m_iDefaultSectorNum = 0;
    UI->ProgressStart(3,"Building DO...");
	UI->ProgressInc();
    bool bRes = BuildObjectDO(F, obj, name, FS.m_GameMeshes);
	UI->ProgressInc();
    if (bRes) F.SaveTo(name,0);
    UI->ProgressEnd();
    return bRes;
}

struct fvfVertexIn
{
    Fvector P;
    float	u,v;
};

DEFINE_VECTOR(fvfVertexIn,DOVertVec,DOVertIt);

bool SceneBuilder::BuildObjectDO( CFS_Base& F, CEditObject *O, const char* name, FSPath& path ){
	CEditObject* obj = (O->IsReference())?O->GetRef():O;

    if(obj->SurfaceCount()!=1){
    	Log->DlgMsg(mtError,"Object must contain 1 material.");
    	return false;
    }
	if(obj->MeshCount()==0){
    	Log->DlgMsg(mtError,"Object must contain 1 mesh.");
    	return false;
    }

    ResetStructures();
    BuildObjectDynamic(obj);

	VERIFY(l_materials.size()==1);

    b_material& M = l_materials[0];

	if(M.dwTexCount!=1){
    	Log->DlgMsg(mtError,"Object must contain 1 texture.");
        ResetStructures();
    	return false;
    }
/*
    // fill data
	DOVertVec do_verts;
	do_verts.resize(l_faces_cnt*3);
    DOVertIt v_it=do_verts.begin();
    for (int f=0; f<l_faces_cnt; f++){
    	b_face& F = l_faces[f];
		v_it->P.set(l_vertices[F.v[0]]); v_it->u=F.t[0][0].tu; v_it->v=F.t[0][0].tv; v_it++;
		v_it->P.set(l_vertices[F.v[1]]); v_it->u=F.t[0][1].tu; v_it->v=F.t[0][1].tv; v_it++;
		v_it->P.set(l_vertices[F.v[2]]); v_it->u=F.t[0][2].tu; v_it->v=F.t[0][2].tv; v_it++;
    }

    // write data
	F.WstringZ	(l_shaders	[M.shader].name);
	F.WstringZ	(l_textures	[M.surfidx[0]].name);

    F.Wdword	(0); // flags

    F.Wdword	(do_verts.size());
    F.write		(do_verts.begin(), do_verts.size()*sizeof(fvfVertexIn));
*/
    // fill data
	DOVertVec do_verts;
	do_verts.resize(l_vertices_cnt);
    DOVertIt v_it=do_verts.begin();
    for (int v=0; v<l_vertices_cnt; v++){
		v_it->P.set(l_vertices[v]);
        for (int f=0; f<l_faces_cnt; f++){
	    	b_face& face = l_faces[f];
            if (face.v[0]==v){
                v_it->u=face.t[0][0].tu;
                v_it->v=face.t[0][0].tv;
                break;
            }else if (face.v[1]==v){
                v_it->u=face.t[0][1].tu;
                v_it->v=face.t[0][1].tv;
                break;
            }else if (face.v[2]==v){
                v_it->u=face.t[0][2].tu;
                v_it->v=face.t[0][2].tv;
                break;
            }
        }
        v_it++;
    }
    // write data
	F.WstringZ	(l_shaders	[M.shader].name);
	F.WstringZ	(l_textures	[M.surfidx[0]].name);

    F.Wdword	(0); // flags
    F.Wdword	(l_vertices_cnt);
    if (1){
    F.Wdword	(l_faces_cnt/2*3);
	}

    F.write		(do_verts.begin(), do_verts.size()*sizeof(fvfVertexIn));
    for (int f=0; f<l_faces_cnt; f++)
    	if (!(f%2))
	    	for (int k=0; k<3; k++)
		        F.Wword(l_faces[f].v[k]);

    WriteTextures();

    ResetStructures();
	return true;
}

