//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditMesh.h"
#include "EditObject.h"
#include "cl_collector.h"
#include "ui_main.h"

/*
void CEditableMesh::CHullPickFaces(PlaneVec& pl, Fmatrix& parent, U32Vec& fl){
	u32 i=0;
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

static IntVec		sml_processed;
static Fvector		sml_normal;
static float		m_fSoftAngle;
//----------------------------------------------------

//----------------------------------------------------
// номер face должен соответствовать списку
//----------------------------------------------------
void CEditableMesh::GenerateCFModel()
{
	SPBItem* pb		= UI->ProgressStart((float)m_SurfFaces.size()*2,"Generating CFModel...");
	UnloadCForm		();
	m_CFModel 		= xr_new<CDB::MODEL>();    VERIFY(m_CFModel);
	// Collect faces
	CDB::Collector CL;
	// double sided
	for (SurfFacesPairIt sp_it=m_SurfFaces.begin(); sp_it!=m_SurfFaces.end(); sp_it++){
    	pb->Inc		();
		IntVec& face_lst = sp_it->second;
		for (IntIt it=face_lst.begin(); it!=face_lst.end(); it++){
			st_Face&	F = m_Faces[*it];
			CL.add_face_D(m_Points[F.pv[0].pindex],m_Points[F.pv[1].pindex],m_Points[F.pv[2].pindex], *it);
			if (sp_it->first->m_Flags.is(CSurface::sf2Sided))
				CL.add_face_D(m_Points[F.pv[2].pindex],m_Points[F.pv[1].pindex],m_Points[F.pv[0].pindex], *it);
		}
	}
    m_CFModel->build(CL.getV(), CL.getVS(), CL.getT(), CL.getTS());
	m_LoadState.set	(LS_CF_MODEL,TRUE);
    UI->ProgressEnd		(pb);
}

void CEditableMesh::RayQuery(const Fmatrix& parent, const Fmatrix& inv_parent, SPickQuery& pinf)
{
    if (!m_CFModel) GenerateCFModel();
    XRC.ray_query	(inv_parent, m_CFModel, pinf.m_Start, pinf.m_Direction, pinf.m_Dist);
    for (int r=0; r<XRC.r_count(); r++)
        pinf.append_mtx(parent,XRC.r_begin()+r);
}

void CEditableMesh::BoxQuery(const Fmatrix& parent, const Fmatrix& inv_parent, SPickQuery& pinf)
{
    if (!m_CFModel) GenerateCFModel();
    XRC.box_query(inv_parent, m_CFModel, pinf.m_BB);
    for (int r=0; r<XRC.r_count(); r++)
        pinf.append_mtx(parent,XRC.r_begin()+r);
}

static const float _sqrt_flt_max = _sqrt(flt_max*0.5f);

bool CEditableMesh::RayPick(float& distance, const Fvector& start, const Fvector& direction, const Fmatrix& inv_parent, SRayPickInfo* pinf)
{
	if (!m_Flags.is(flVisible)) return false;

    if (!m_CFModel) GenerateCFModel();
//.	float m_r 		= pinf?pinf->inf.range+EPS_L:UI->ZFar();// (bugs: не всегда выбирает) //S ????

	XRC.ray_options	(CDB::OPT_ONLYNEAREST | CDB::OPT_CULL);
	XRC.ray_query	(inv_parent, m_CFModel, start, direction, _sqrt_flt_max);

    if (XRC.r_count()){
		CDB::RESULT* I	= XRC.r_begin	();
		if (I->range<distance) {
	        if (pinf){
            	pinf->SetRESULT	(m_CFModel,I);
    	        pinf->e_obj 	= m_Parent;
        	    pinf->e_mesh	= this;
	            pinf->pt.mul	(direction,pinf->inf.range);
    	        pinf->pt.add	(start);
            }
            distance = I->range;
            return true;
		}
    }
	return false;
}
//----------------------------------------------------
#ifdef _EDITOR
bool CEditableMesh::CHullPickMesh(PlaneVec& pl, const Fmatrix& parent){
	u32 i=0;
	Fvector p;
    xr_vector<bool> inside(m_Points.size(),true);
    for(FvectorIt v_it=m_Points.begin();v_it!=m_Points.end();v_it++){
        parent.transform_tiny(p,*v_it);
        for(PlaneIt p_it=pl.begin(); p_it!=pl.end(); p_it++)
        	if (p_it->classify(p)>EPS_L) { inside[v_it-m_Points.begin()]=false; break; }
    }
    for(FaceIt f_it=m_Faces.begin();f_it!=m_Faces.end();f_it++,i++)
    	if (inside[f_it->pv[0].pindex]&&inside[f_it->pv[1].pindex]&&inside[f_it->pv[2].pindex]) return true;
    return false;
}
//----------------------------------------------------

void CEditableMesh::RecurseTri(int id)
{
	// Check if triangle already processed
	if (std::find(sml_processed.begin(),sml_processed.end(),id)!=sml_processed.end())
		return;

	sml_processed.push_back(id);

    // recurse
    for (int k=0; k<3; k++){
	    IntVec& PL = m_Adjs[m_Faces[id].pv[k].pindex];
        for (IntIt pl_it=PL.begin(); pl_it!=PL.end(); pl_it++){
            Fvector &test_normal = m_FNormals[*pl_it];
            float cosa = test_normal.dotproduct(sml_normal);
            if (cosa<m_fSoftAngle) continue;
            RecurseTri(*pl_it);
        }
    }
}
//----------------------------------------------------

void CEditableMesh::GetTiesFaces(int start_id, U32Vec& fl, float fSoftAngle, bool bRecursive){
    R_ASSERT(start_id<int(m_Faces.size()));
    m_fSoftAngle=cosf(deg2rad(fSoftAngle));
    if (!m_LoadState.is(LS_FNORMALS)) GenerateFNormals();
    VERIFY(m_FNormals.size());
    if (bRecursive){
    	sml_processed.clear();
        sml_normal.set(m_FNormals[start_id]);
    	RecurseTri(start_id);
        fl.insert(fl.begin(),sml_processed.begin(),sml_processed.end());
    }else{
    	for (int k=0; k<3; k++)
        	fl.insert(fl.end(),m_Adjs[m_Faces[start_id].pv[k].pindex].begin(),m_Adjs[m_Faces[start_id].pv[k].pindex].end());
        std::sort(fl.begin(),fl.end());
        fl.erase(std::unique(fl.begin(),fl.end()),fl.end());
    }
}
//----------------------------------------------------

bool CEditableMesh::BoxPick(const Fbox& box, const Fmatrix& inv_parent, SBoxPickInfoVec& pinf)
{
    if (!m_CFModel) GenerateCFModel();

    XRC.box_query(inv_parent, m_CFModel, box);
    if (XRC.r_count()){
    	pinf.push_back(SBoxPickInfo());
		pinf.back().e_obj 	= m_Parent;
	    pinf.back().e_mesh	= this;
	    for (CDB::RESULT* I=XRC.r_begin(); I!=XRC.r_end(); I++) pinf.back().AddRESULT(m_CFModel,I);
        return true;
    }
    return false;
}
//----------------------------------------------------

bool CEditableMesh::FrustumPick(const CFrustum& frustum, const Fmatrix& parent)
{
	if (!m_Flags.is(flVisible)) return false;

	Fvector p[3];
	for(u32 i=0;i<m_Faces.size();i++){
		for( int k=0;k<3;k++)
            parent.transform_tiny(p[k],m_Points[m_Faces[i].pv[k].pindex]);
		if (frustum.testPolyInside(p,3)) return true;
	}
	return false;
}
//---------------------------------------------------------------------------

void CEditableMesh::FrustumPickFaces(const CFrustum& frustum, const Fmatrix& parent, U32Vec& fl)
{
	if (!m_Flags.is(flVisible)) return;

	u32 i=0;
	Fvector p[3];
    bool bCulling=EPrefs.bp_cull;
    for(FaceIt p_it=m_Faces.begin();p_it!=m_Faces.end();p_it++,i++){
        for( int k=0;k<3;k++) parent.transform_tiny(p[k],m_Points[p_it->pv[k].pindex]);
        if (bCulling){
	        Fplane P; P.build(p[0],p[1],p[2]);
    	    if (P.classify(Device.m_Camera.GetPosition())<0) continue;
        }
        if (frustum.testPolyInside(p,3))
            fl.push_back(i);
    }
}
#endif //



