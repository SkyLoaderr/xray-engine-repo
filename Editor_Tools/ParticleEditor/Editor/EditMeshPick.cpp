//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditMesh.h"
#include "UI_Main.h"
#include "frustum.h"
#include "EditorPref.h"
#include "bottombar.h"

/*
void CEditableMesh::CHullPickFaces(PlaneVec& pl, Fmatrix& parent, DWORDVec& fl){
	DWORD i=0;
	Fvector p;
    vector<bool> inside(m_Points.size(),true);
    for(FvectorIt v_it=m_Points.begin();v_it!=m_Points.end();v_it++){
        parent.transform_tiny(p,*v_it);
        for(PlaneIt p_it=pl.begin(); p_it!=pl.end(); p_it++)
        	if (p_it->classify(p)>EPS_L) { inside[v_it-m_Points.begin()]=false; break; }
    }
    for(FaceIt f_it=m_Faces.begin();f_it!=m_Faces.end();f_it++,i++)
    	if (inside[f_it->pv[0].pindex]&&inside[f_it->pv[1].pindex]&&inside[f_it->pv[2].pindex]) fl.push_back(i);
}
*/
//----------------------------------------------------

static INTVec		sml_processed;
static Fvector		sml_normal;
static float		m_fSoftAngle;
//----------------------------------------------------

//----------------------------------------------------
// ����� face ������ ��������������� ������
//----------------------------------------------------
void CEditableMesh::GenerateCFModel(){
	UnloadCForm();

	m_CFModel = new RAPID::Model();    VERIFY(m_CFModel);
	// Collect faces

	RAPID::Collector CL;
	// double sided
/*	�� ��������� �������� � ����������
	for (SurfFacesPairIt sp_it=m_SurfFaces.begin(); sp_it!=m_SurfFaces.end(); sp_it++){
		INTVec& face_lst = sp_it->second;
		for (INTIt it=face_lst.begin(); it!=face_lst.end(); it++){
			st_Face&	F = m_Faces[*it];
			CL.add_face(m_Points[F.pv[0].pindex],m_Points[F.pv[1].pindex],m_Points[F.pv[2].pindex], 0,0,0, 0,0,0);
			if (sp_it->first->sideflag)
				CL.add_face(m_Points[F.pv[2].pindex],m_Points[F.pv[1].pindex],m_Points[F.pv[0].pindex], 0,0,0, 0,0,0);
		}
	}
*/
	// without double sided
	for (FaceIt P = m_Faces.begin(); P!=m_Faces.end(); P++){
		st_Face&	F = *P;
		CL.add_face(
       		m_Points[F.pv[0].pindex],m_Points[F.pv[1].pindex],m_Points[F.pv[2].pindex],
			0,0,0,
			0,0,0 );
	}

	m_CFModel->BuildModel(CL.getV(), CL.getVS(), CL.getT(), CL.getTS());
	m_LoadState |= EMESH_LS_CF_MODEL;
}

bool CEditableMesh::RayPick(float& distance, Fvector& start, Fvector& direction, Fmatrix& parent, SRayPickInfo* pinf){
	if (!m_Visible) return false;

    if (!m_CFModel) GenerateCFModel();
    float m_r = pinf?pinf->rp_inf.range:UI.ZFar();

    XRC.RayPick	(&parent, m_CFModel, start, direction, m_r);
    float new_dist;
    if (XRC.GetMinRayPickDistance(new_dist)){
    	if (new_dist<distance){
	        if (pinf){
	            pinf->rp_inf= *XRC.GetMinRayPickInfo();
    	        pinf->e_obj 	= m_Parent;
        	    pinf->e_mesh	= this;
	            pinf->pt.mul(direction,pinf->rp_inf.range);
    	        pinf->pt.add(start);
            }
            distance = new_dist;
            return true;
        }
    }
	return false;
}
//----------------------------------------------------
#ifdef _LEVEL_EDITOR

bool CEditableMesh::CHullPickMesh(PlaneVec& pl, Fmatrix& parent){
	DWORD i=0;
	Fvector p;
    vector<bool> inside(m_Points.size(),true);
    for(FvectorIt v_it=m_Points.begin();v_it!=m_Points.end();v_it++){
        parent.transform_tiny(p,*v_it);
        for(PlaneIt p_it=pl.begin(); p_it!=pl.end(); p_it++)
        	if (p_it->classify(p)>EPS_L) { inside[v_it-m_Points.begin()]=false; break; }
    }
    for(FaceIt f_it=m_Faces.begin();f_it!=m_Faces.end();f_it++,i++)
    	if (inside[f_it->pv[0].pindex]&&inside[f_it->pv[1].pindex]&&inside[f_it->pv[2].pindex]) return true;
}
//----------------------------------------------------

void CEditableMesh::RecurseTri(int id)
{
	// Check if triangle already processed
	if (find(sml_processed.begin(),sml_processed.end(),id)!=sml_processed.end())
		return;

	sml_processed.push_back(id);

    // recurse
    for (int k=0; k<3; k++){
	    INTVec& PL = m_Adjs[m_Faces[id].pv[k].pindex];
        for (INTIt pl_it=PL.begin(); pl_it!=PL.end(); pl_it++){
            Fvector &test_normal = m_FNormals[*pl_it];
            float cosa = test_normal.dotproduct(sml_normal);
            if (cosa<m_fSoftAngle) continue;
            RecurseTri(*pl_it);
        }
    }
}
//----------------------------------------------------

void CEditableMesh::GetTiesFaces(int start_id, DWORDVec& fl, float fSoftAngle, bool bRecursive){
    R_ASSERT(start_id<int(m_Faces.size()));
    m_fSoftAngle=cosf(deg2rad(fSoftAngle));
    if (!(m_LoadState&EMESH_LS_FNORMALS)) GenerateFNormals();
    VERIFY(m_FNormals.size());
    if (bRecursive){
    	sml_processed.clear();
        sml_normal.set(m_FNormals[start_id]);
    	RecurseTri(start_id);
        fl.insert(fl.begin(),sml_processed.begin(),sml_processed.end());
    }else{
    	for (int k=0; k<3; k++)
        	fl.insert(fl.end(),m_Adjs[m_Faces[start_id].pv[k].pindex].begin(),m_Adjs[m_Faces[start_id].pv[k].pindex].end());
        sort(fl.begin(),fl.end());
        unique(fl.begin(),fl.end());
    }
}
//----------------------------------------------------

void CEditableMesh::BoxPick(const Fbox& box, Fmatrix& parent, SBoxPickInfoVec& pinf){
    if (!m_CFModel) GenerateCFModel();

    XRC.BBoxCollide(parent, m_CFModel, precalc_identity, box);
    int cc=XRC.GetBBoxContactCount();
    if (cc){
    	for (int i=0; i<cc; i++){
        	pinf.push_back(SBoxPickInfo());
            pinf.back().bp_inf	= XRC.BBoxContact[i];
            pinf.back().e_mesh	= this;
        }
    }
}
//----------------------------------------------------

bool CEditableMesh::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
	if (!m_Visible) return false;

	Fvector p[3];
	for(DWORD i=0;i<m_Faces.size();i++){
		for( int k=0;k<3;k++)
            parent.transform_tiny(p[k],m_Points[m_Faces[i].pv[k].pindex]);
	    sPoly s(p,3);
		if (frustum.testPoly(s)) return true;
	}
	return false;
}
//---------------------------------------------------------------------------

void CEditableMesh::FrustumPickFaces(const CFrustum& frustum, Fmatrix& parent, DWORDVec& fl){
	if (!m_Visible) return;

	DWORD i=0;
	Fvector p[3];
    bool bCulling=frmEditorPreferences->cbBoxPickFaceCulling->Checked;
    for(FaceIt p_it=m_Faces.begin();p_it!=m_Faces.end();p_it++,i++){
        for( int k=0;k<3;k++) parent.transform_tiny(p[k],m_Points[p_it->pv[k].pindex]);
        if (bCulling){
	        Fplane P; P.build(p[0],p[1],p[2]);
    	    if (P.classify(Device.m_Camera.GetPosition())<0) continue;
        }
        sPoly s(p,3);
        if (frustum.testPoly(s))
            fl.push_back(i);
    }
}
#endif //_LEVEL_EDITOR



