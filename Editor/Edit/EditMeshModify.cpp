//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditMesh.h"
#include "EditObject.h"
#include "ui_main.h"
//----------------------------------------------------
void CEditMesh::CloneFrom(CEditMesh *source){
	VERIFY(source);
    CEditObject* save_parent = m_Parent;
    // full copy from source object
    *this = *source;
    // restore pointers
    m_LoadState = 0;
    m_Parent 	= save_parent;
    m_CFModel 	= NULL;
    m_RenderBuffers.clear();
	// copy surface links
	m_SurfFaces.clear();
	for (SurfFacesPairIt sp_it=source->m_SurfFaces.begin(); sp_it!=source->m_SurfFaces.end(); sp_it++){
        st_Surface* 	surf= m_Parent->FindSurfaceByName(sp_it->first->name); VERIFY(surf);
        INTVec&			face_lst = m_SurfFaces[surf];
        face_lst		= sp_it->second;
    }
}

//----------------------------------------------------
void CEditMesh::Transform(Fmatrix& parent){
	// transform position
	for(FvectorIt pt=m_Points.begin(); pt!=m_Points.end(); pt++)
		parent.transform_tiny(*pt);
    // RecomputeBBox
	RecomputeBBox();
    // update normals & cform
    UnloadFNormals();
    UnloadPNormals();
    UnloadCForm();
/*
    R_ASSERT(m_LoadState&EMESH_LS_FNORMALS);
    R_ASSERT(m_LoadState&EMESH_LS_PNORMALS);
    VERIFY(m_FNormals.size());
    VERIFY(m_PNormals.size());
    int fn_cnt = m_FNormals.size();
    for (int fn_i=0; fn_i<fn_cnt; fn_i){
    	parent.transform_dir(m_FNormals[fn_i]);
        parent.transform_dir(m_PNormals[fn_i*3+0]);
        parent.transform_dir(m_PNormals[fn_i*3+1]);
        parent.transform_dir(m_PNormals[fn_i*3+2]);
    }
    // recomended regenerate CForm
*/
}
//----------------------------------------------------

bool CEditMesh::UpdateAdjacency(){
	if (m_Faces.empty()) return false;
    UI->SetStatus("Update Adjacency:");
    m_Adjs.clear();
    m_Adjs.resize(m_Points.size());
	for (FaceIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++)
		for (int k=0; k<3; k++) m_Adjs[f_it->pv[k].pindex].push_back(f_it-m_Faces.begin());
	return true;
}

#define MX 25
#define MY 15
#define MZ 25
static Fvector		VMmin, VMscale;
static DWORDVec		VM[MX+1][MY+1][MZ+1];
static Fvector		VMeps;

static FvectorVec m_NewPoints;
bool CEditMesh::OptimizeFace(st_Face& face){
	Fvector points[3];
	int mface[3];
	int k;

	for (k=0; k<3; k++){
    	points[k].set(m_Points[face.pv[k].pindex]);
		mface[k] = -1;
    }
	
	// get similar vert idx list
	for (k=0; k<3; k++){
		DWORDVec* vl; 
		int ix,iy,iz;
		ix = floorf(float(points[k].x-VMmin.x)/VMscale.x*MX);
		iy = floorf(float(points[k].y-VMmin.y)/VMscale.y*MY);
		iz = floorf(float(points[k].z-VMmin.z)/VMscale.z*MZ);
		vl = &(VM[ix][iy][iz]);
		for(DWORDIt it=vl->begin();it!=vl->end(); it++){ 
			FvectorIt v = m_NewPoints.begin()+(*it);
            if( v->similar(points[k],EPS) )
                mface[k] = *it;
		}
	}
	for(k=0; k<3; k++ ){
		if( mface[k] == -1 ){
			mface[k] = m_NewPoints.size();
			m_NewPoints.push_back( points[k] );
			int ix,iy,iz;
			ix = floorf(float(points[k].x-VMmin.x)/VMscale.x*MX);
			iy = floorf(float(points[k].y-VMmin.y)/VMscale.y*MY);
			iz = floorf(float(points[k].z-VMmin.z)/VMscale.z*MZ);
			VM[ix][iy][iz].push_back(mface[k]);
			int ixE,iyE,izE;
			ixE = floorf(float(points[k].x+VMeps.x-VMmin.x)/VMscale.x*MX);
			iyE = floorf(float(points[k].y+VMeps.y-VMmin.y)/VMscale.y*MY);
			izE = floorf(float(points[k].z+VMeps.z-VMmin.z)/VMscale.z*MZ);
			if (ixE!=ix)				
				VM[ixE][iy][iz].push_back(mface[k]);
			if (iyE!=iy)				
				VM[ix][iyE][iz].push_back(mface[k]);
			if (izE!=iz)				
				VM[ix][iy][izE].push_back(mface[k]);
			if ((ixE!=ix)&&(iyE!=iy))	
				VM[ixE][iyE][iz].push_back(mface[k]);
			if ((ixE!=ix)&&(izE!=iz))	
				VM[ixE][iy][izE].push_back(mface[k]);
			if ((iyE!=iy)&&(izE!=iz))	
				VM[ix][iyE][izE].push_back(mface[k]);
			if ((ixE!=ix)&&(iyE!=iy)&&(izE!=iz))
				VM[ixE][iyE][izE].push_back(mface[k]);
		}
	}
	
	if ((mface[0]==mface[1])||(mface[1]==mface[2])||(mface[0]==mface[2])){
		Log->DlgMsg( mtError, "Optimize: Face missing." );
        return false;
	}else{
    	face.pv[0].pindex = mface[0];
    	face.pv[1].pindex = mface[1];
    	face.pv[2].pindex = mface[2];
        return true;
	}
}

void CEditMesh::Optimize(){
	// clear static data
    for (int x=0; x<MX+1; x++)
	    for (int y=0; y<MY+1; y++)
    		for (int z=0; z<MZ+1; z++)
            	VM[x][y][z].clear();
	VMscale.set(m_Box.max.x-m_Box.min.x, m_Box.max.y-m_Box.min.y, m_Box.max.z-m_Box.min.z);
	VMmin.set(m_Box.min.x, m_Box.min.y, m_Box.min.z);
	
	VMeps.set(VMscale.x/MX/2,VMscale.y/MY/2,VMscale.z/MZ/2);
	VMeps.x = (VMeps.x<EPS_L)?VMeps.x:EPS_L;
	VMeps.y = (VMeps.y<EPS_L)?VMeps.y:EPS_L;
	VMeps.z = (VMeps.z<EPS_L)?VMeps.z:EPS_L;

    m_NewPoints.clear();
    m_NewPoints.reserve(m_Points.size());
    
    UI->SetStatus("Optimize...");
    
	INTVec mark_for_del;
	mark_for_del.clear();
    for (DWORD k=0; k<m_Faces.size(); k++){
    	if (!OptimizeFace(m_Faces[k]))
			mark_for_del.push_back(k);
	}

    m_Points.clear();
    m_Points = m_NewPoints;    
    if (mark_for_del.size()>0){
        std::sort	(mark_for_del.begin(),mark_for_del.end());
        std::reverse(mark_for_del.begin(),mark_for_del.end());
        // delete degenerate faces
        for (INTIt i_it=mark_for_del.begin(); i_it!=mark_for_del.end(); i_it++)
            m_Faces.erase(m_Faces.begin()+(*i_it));
        // delete degenerate faces refs
        for (INTIt m_d=mark_for_del.begin(); m_d!=mark_for_del.end(); m_d++){
            for (SurfFacesPairIt plp_it=m_SurfFaces.begin(); plp_it!=m_SurfFaces.end(); plp_it++){
                INTVec& 	pol_lst = plp_it->second;
                for (int k=0; k<int(pol_lst.size()); k++){
                    int& f = pol_lst[k];
                    if (f>*m_d){ f--;
                    }else if (f==*m_d){
                        pol_lst.erase(pol_lst.begin()+k);
                        k--;
                    }
                }
            }
        }
    }

    UpdateAdjacency();
}
 
