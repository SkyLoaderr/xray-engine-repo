//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditMesh.h"
#include "EditObject.h"
#include "Bone.h"

CEditableMesh::~CEditableMesh(){
	Clear();
#ifdef _EDITOR
    R_ASSERT2(m_RenderBuffers.empty(),"Render buffer still referenced.");
#endif
}

void CEditableMesh::Construct()
{
	m_Box.set		(0,0,0,0,0,0);
	m_Flags.assign	(flVisible);
    m_Name[0]		= 0;
    m_LoadState.zero();
#ifdef _EDITOR
    m_CFModel		= 0;
#endif
}

void CEditableMesh::Clear()           
{
#ifdef _EDITOR
	ClearRenderBuffers	();
#endif
	m_Points.clear 		();
    m_Adjs.clear		();
	m_Faces.clear		();
	for (VMapIt vm_it=m_VMaps.begin(); vm_it!=m_VMaps.end(); vm_it++)
		xr_delete(*vm_it);
    m_VMaps.clear		();
    m_SurfFaces.clear	();
    m_VMRefs.clear		();
    UnloadCForm			();
    UnloadFNormals		();
    UnloadPNormals		();
}

void CEditableMesh::UnloadCForm     ()
{
	if (!m_LoadState.is(LS_CF_MODEL)) return;
#ifdef _EDITOR
	xr_delete(m_CFModel);
    m_CFModel = 0;
#endif
    m_LoadState.set(LS_CF_MODEL,FALSE);
}

void CEditableMesh::UnloadFNormals   ()
{
	if (!m_LoadState.is(LS_FNORMALS)) return;
	m_FNormals.clear	();
    m_LoadState.set(LS_FNORMALS,FALSE);
}

void CEditableMesh::UnloadPNormals   ()
{
	if (!m_LoadState.is(LS_PNORMALS)) return;
	m_PNormals.clear	();
    m_LoadState.set(LS_PNORMALS,FALSE);
}

void CEditableMesh::UnloadSVertices	 ()
{
	if (!m_LoadState.is(LS_SVERTICES)) return;
	m_SVertices.clear	();
    m_LoadState.set(LS_SVERTICES,FALSE);
}

void CEditableMesh::RecomputeBBox()
{
	if( m_Points.empty() ){
		m_Box.set(0,0,0, 0,0,0);
		return;
    }
	m_Box.set( m_Points[0], m_Points[0] );
	for(FvectorIt pt=m_Points.begin()+1; pt!=m_Points.end(); pt++)
		m_Box.modify(*pt);
}

void CEditableMesh::GenerateFNormals()
{
    m_FNormals.resize	(m_Faces.size());
    // face normals
	FaceIt 		f_it	= m_Faces.begin();
	FvectorIt 	fn_it	= m_FNormals.begin();
    for (f_it; f_it!=m_Faces.end(); f_it++, fn_it++ ){
        fn_it->mknormal(	m_Points[f_it->pv[0].pindex],
                			m_Points[f_it->pv[1].pindex],
                            m_Points[f_it->pv[2].pindex]);
    }
/*
    for (int i=0; i<m_Faces.size(); i++){
    	st_Face& F = m_Faces[i];
        Fvector& N = m_FNormals[i];
        N.mknormal(	m_Points[F.pv[0].pindex],
        			m_Points[F.pv[1].pindex],
                    m_Points[F.pv[2].pindex]);
    }
*/
    m_LoadState.set(LS_FNORMALS,TRUE);
}

void CEditableMesh::GeneratePNormals()
{
	if (!m_LoadState.is(LS_FNORMALS)) GenerateFNormals();

	// vertex normals
    m_PNormals.resize	(m_Faces.size()*3);
	if (m_Flags.is(flSGMask)){
		for (u32 f_i=0; f_i<m_Faces.size(); f_i++ ){
			u32 sg			= m_SGs[f_i];
			Fvector& FN 	= m_FNormals[f_i];
			for (int k=0; k<3; k++){
				Fvector& N 	= m_PNormals[f_i*3+k];
				if (sg){
					N.set		(0,0,0);
					IntVec& a_lst=m_Adjs[m_Faces[f_i].pv[k].pindex];
					VERIFY(a_lst.size());
					for (IntIt i_it=a_lst.begin(); i_it!=a_lst.end(); i_it++)
						if (sg&m_SGs[*i_it]) N.add	(m_FNormals[*i_it]);
					N.normalize_safe();
				}else{
					N.set		(FN);
				}
			}
		}
	}else{
		for (u32 f_i=0; f_i<m_Faces.size(); f_i++ ){
			u32 sg			= m_SGs[f_i];
			Fvector& FN 	= m_FNormals[f_i];
			for (int k=0; k<3; k++){
				Fvector& N 	= m_PNormals[f_i*3+k];
				if (sg!=-1){
					N.set		(0,0,0);
					IntVec& a_lst=m_Adjs[m_Faces[f_i].pv[k].pindex];
					VERIFY(a_lst.size());
					for (IntIt i_it=a_lst.begin(); i_it!=a_lst.end(); i_it++){
						if (sg != m_SGs[*i_it]) continue;
						N.add	(m_FNormals[*i_it]);
					}
					N.normalize_safe();
				}else{
					N.set		(0,0,0);
					IntVec& a_lst=m_Adjs[m_Faces[f_i].pv[k].pindex];
					VERIFY(a_lst.size());
					for (IntIt i_it=a_lst.begin(); i_it!=a_lst.end(); i_it++)
						N.add	(m_FNormals[*i_it]);
					N.normalize_safe();
				}
			}
		}
	}
    m_LoadState.set(LS_PNORMALS,TRUE);
}

void CEditableMesh::GenerateSVertices()
{
	if (!m_Parent->IsSkeleton()) return;

    CSMotion* active_motion=m_Parent->ResetSAnimation();
    m_Parent->CalculateAnimation(0);
	m_SVertices.resize(m_Faces.size()*3);

    // generate normals
    if (!m_LoadState.is(CEditableMesh::LS_FNORMALS)) GenerateFNormals();
    if (!m_LoadState.is(CEditableMesh::LS_PNORMALS)) GeneratePNormals();

    for (FaceIt f_it = m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
        st_Face& F = *f_it;
        for (int k=0; k<3; k++){
	    	st_SVert& SV			= m_SVertices[(f_it-m_Faces.begin())*3+k];
	    	Fvector&  N				= m_PNormals[(f_it-m_Faces.begin())*3+k];
            st_FaceVert& fv 		= F.pv[k];
	    	Fvector&  P 			= m_Points[fv.pindex];
            VMapPtSVec& vmpt_lst 	= m_VMRefs[fv.vmref];
            st_VertexWB		wb;
            for (VMapPtIt vmpt_it=vmpt_lst.begin(); vmpt_it!=vmpt_lst.end(); vmpt_it++){
                st_VMap& VM = *m_VMaps[vmpt_it->vmap_index];
                if (VM.type==vmtWeight){
                    wb.push_back(st_WB(m_Parent->GetBoneIndexByWMap(VM.name),VM.getW(vmpt_it->index)));
                    if (wb.back().bone<=-1){
                        ELog.DlgMsg(mtError,"Can't find bone assigned to weight map %s",VM.name);
                        m_SVertices.clear();
                        Debug.fatal("Editor crashed.");
                        return;
                    }
                }else if(VM.type==vmtUV){	
//                 	R_ASSERT2(!uv,"More than 1 uv per vertex found.");
                    SV.uv.set(VM.getUV(vmpt_it->index));
                }
            }
            wb.normalize_weights(2);
            int cnt = wb.size();
            switch (cnt){
                case 0:
                	Debug.fatal("Vertex has't any weights attached.");
                break;
                case 1:{
                    SV.bone0 	= wb[0].bone;
                    SV.bone1 	= BI_NONE;
                    SV.w	   	= 0.f;
                    SV.offs		= P;
                    SV.norm		= N;
                }break;
                case 2:{
                    SV.bone0 	= wb[0].bone;
                    SV.bone1 	= wb[1].bone;
                    SV.w	   	= wb[1].weight/(wb[0].weight+wb[1].weight);
                    SV.offs		= P;
                    SV.norm		= N;
                }break;
                default:
                    Debug.fatal("More than 2 weight per vertex found!");
            }
        }
	}

    m_LoadState.set(LS_SVERTICES,TRUE);
    // restore active motion
    m_Parent->SetActiveSMotion(active_motion);
}

CSurface*	CEditableMesh::GetSurfaceByFaceID(u32 fid)
{
	R_ASSERT(fid<m_Faces.size());
    for (SurfFacesPairIt sp_it=m_SurfFaces.begin(); sp_it!=m_SurfFaces.end(); sp_it++){
		IntVec& face_lst = sp_it->second;
        if (std::find(face_lst.begin(),face_lst.end(),fid)!=face_lst.end()) return sp_it->first;
	}
    return 0;
}

void CEditableMesh::GetFaceTC(u32 fid, const Fvector2* tc[3])
{
	R_ASSERT(fid<m_Faces.size());
	st_Face& F = m_Faces[fid];
    for (int k=0; k<3; k++){
	    st_VMapPt& vmr = m_VMRefs[F.pv[k].vmref][0];
    	tc[k] = &(m_VMaps[vmr.vmap_index]->getUV(vmr.index));
    }
}

void CEditableMesh::GetFacePT(u32 fid, const Fvector* pt[3])
{
	R_ASSERT(fid<m_Faces.size());
	st_Face& F = m_Faces[fid];
    for (int k=0; k<3; k++)
    	pt[k] = &m_Points[F.pv[k].pindex];
}

int CEditableMesh::GetFaceCount(bool bMatch2Sided){
	int f_cnt = 0;
    for (SurfFacesPairIt sp_it=m_SurfFaces.begin(); sp_it!=m_SurfFaces.end(); sp_it++){
    	if (bMatch2Sided){
	    	if (sp_it->first->m_Flags.is(CSurface::sf2Sided))	f_cnt+=sp_it->second.size()*2;
    	    else												f_cnt+=sp_it->second.size();
        }else{
        	f_cnt+=sp_it->second.size();
        }
	}
    return f_cnt;
}

int CEditableMesh::GetSurfFaceCount(CSurface* surf, bool bMatch2Sided){
	SurfFacesPairIt sp_it = m_SurfFaces.find(surf);
    if (sp_it==m_SurfFaces.end()) return 0;
	int f_cnt = sp_it->second.size();
    if (bMatch2Sided&&sp_it->first->m_Flags.is(CSurface::sf2Sided)) f_cnt*=2;
    return f_cnt;
}

void CEditableMesh::DumpAdjacency(){
	Log("Adjacency dump.");
	Log("------------------------------------------------------------------------");
/*    for (u32 i=0; i<m_Adjs.size(); i++){
        IntVec& a_lst	= m_Adjs[i];
        AnsiString s; s = "Point "; s+=AnsiString(i); s+=":";
        AnsiString s1;
        for (u32 j=0; j<a_lst.size(); j++){ s1=a_lst[j]; s+=" "+s1; }
		Log(s.c_str());
    }
*/
}

//----------------------------------------------------------------------------

int CEditableMesh::FindVMapByName(VMapVec& vmaps, const char* name, EVMType t, BOOL polymap){
	for (VMapIt vm_it=vmaps.begin(); vm_it!=vmaps.end(); vm_it++){
		if (((*vm_it)->type==t)&&(stricmp((*vm_it)->name,name)==0)&&(polymap==(*vm_it)->polymap)) return vm_it-vmaps.begin();
	}
	return -1;
}
