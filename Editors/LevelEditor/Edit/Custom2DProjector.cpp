//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Custom2DProjector.h"
#include "ImageManager.h"
#include "SceneObject.h"
#include "EditObject.h"
#include "EditMesh.h"

//------------------------------------------------------------------------------
// SBase
//------------------------------------------------------------------------------
#define MAX_BUF_SIZE 0xFFFF

CCustom2DProjector::CCustom2DProjector()
{
	name[0]			= 0;
	shader_overlap	= 0;
    shader_blended	= 0;
}

bool CCustom2DProjector::LoadImage(LPCSTR nm)
{
	strcpy(name,nm);
    ImageManager.LoadTextureData(nm,data,w,h);
    return Valid();
}

void CCustom2DProjector::CreateRMFromObjects(const Fbox& box, ObjectList& lst)
{
	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
    	CSceneObject*	 S = (CSceneObject*)(*it);
    	CEditableObject* O = S->GetReference(); VERIFY(O);

        Fmatrix T; S->GetFullTransformToWorld(T);
        mesh.reserve	(mesh.size()+S->GetFaceCount()*3);
        for (EditMeshIt m_it=O->FirstMesh(); m_it!=O->LastMesh(); m_it++){
		    FaceVec&	faces 	= (*m_it)->GetFaces();
		    FvectorVec&	pts 	= (*m_it)->GetPoints();
        	for (FaceIt f_it=faces.begin(); f_it!=faces.end(); f_it++){
            	FVF::V v;
                for (int k=0; k<3; k++){
                	T.transform_tiny(v.p,pts[f_it->pv[k].pindex]);
					v.t.x = GetUFromX(v.p.x,box);
					v.t.y = GetVFromZ(v.p.z,box);
                    mesh.push_back(v);
                }
            }
        }
    }
	geom	= Device.Shader.CreateGeom(FVF::F_V,RCache.Vertex.Buffer(),0);
}

void CCustom2DProjector::Render()
{
	if (!Valid()) return;
    Device.RenderNearer(0.001f);
	RCache.set_xform_world(Fidentity);
    Device.SetShader	(shader_blended);//((fraBottomBar->miDrawDOBlended->Checked)?shader_blended:shader_overlap);
    div_t cnt 			= div(mesh.size(),MAX_BUF_SIZE);
    u32 vBase;
    _VertexStream* Stream = &RCache.Vertex;
    for (int k=0; k<cnt.quot; k++){
		FVF::V*	pv	 	= (FVF::V*)Stream->Lock(MAX_BUF_SIZE,geom->vb_stride,vBase);
		CopyMemory		(pv,mesh.begin()+k*MAX_BUF_SIZE,sizeof(FVF::V)*MAX_BUF_SIZE);
		Stream->Unlock	(MAX_BUF_SIZE,geom->vb_stride);
		Device.DP		(D3DPT_TRIANGLELIST,geom,vBase,MAX_BUF_SIZE/3);
    }
    if (cnt.rem){
		FVF::V*	pv	 	= (FVF::V*)Stream->Lock(cnt.rem,geom->vb_stride,vBase);
		CopyMemory		(pv,mesh.begin()+cnt.quot*MAX_BUF_SIZE,sizeof(FVF::V)*cnt.rem);
		Stream->Unlock	(cnt.rem,geom->vb_stride);
		Device.DP		(D3DPT_TRIANGLELIST,geom,vBase,cnt.rem/3);
    }
    Device.ResetNearer	();
}

void CCustom2DProjector::CreateShader()
{
	DestroyShader		();
	if (Valid()){
		shader_blended 	= Device.Shader.Create("editor\\do_base",name);
		shader_overlap 	= Device.Shader.Create("default",name);
		geom			= Device.Shader.CreateGeom(FVF::F_V,RCache.Vertex.Buffer(),0);
	}
}

void CCustom2DProjector::DestroyShader()
{
	Device.Shader.DeleteGeom(geom);
	Device.Shader.Delete	(shader_blended);
	Device.Shader.Delete	(shader_overlap);
}
