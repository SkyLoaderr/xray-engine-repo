//----------------------------------------------------
// file: ExportSkeleton.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ExportObjectOGF.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "fmesh.h"
#include "std_classes.h"
#include "bone.h"
#include "motion.h"
#include "ui_main.h"
#include "MgcCont3DMinBox.h"

CObjectOGFCollectorPacked::CObjectOGFCollectorPacked(const Fbox &bb, int apx_vertices, int apx_faces)
{
    // Params
    m_VMscale.set	(bb.max.x-bb.min.x, bb.max.y-bb.min.y, bb.max.z-bb.min.z);
    m_VMmin.set		(bb.min);
    m_VMeps.set		(m_VMscale.x/clpOGFMX/2,m_VMscale.y/clpOGFMY/2,m_VMscale.z/clpOGFMZ/2);
    m_VMeps.x		= (m_VMeps.x<EPS_L)?m_VMeps.x:EPS_L;
    m_VMeps.y		= (m_VMeps.y<EPS_L)?m_VMeps.y:EPS_L;
    m_VMeps.z		= (m_VMeps.z<EPS_L)?m_VMeps.z:EPS_L;

    // Preallocate memory
    m_Verts.reserve	(apx_vertices);
    m_Faces.reserve	(apx_faces);

    int		_size	= (clpOGFMX+1)*(clpOGFMY+1)*(clpOGFMZ+1);
    int		_average= (apx_vertices/_size)/2;
    for (int ix=0; ix<clpOGFMX+1; ix++)
        for (int iy=0; iy<clpOGFMY+1; iy++)
            for (int iz=0; iz<clpOGFMZ+1; iz++)
                m_VM[ix][iy][iz].reserve	(_average);
}
//----------------------------------------------------

u32 CObjectOGFCollectorPacked::VPack(SOGFVert& V)
{
    u32 P 	= 0xffffffff;

    u32 ix,iy,iz;
    ix = iFloor(float(V.P.x-m_VMmin.x)/m_VMscale.x*clpOGFMX);
    iy = iFloor(float(V.P.y-m_VMmin.y)/m_VMscale.y*clpOGFMY);
    iz = iFloor(float(V.P.z-m_VMmin.z)/m_VMscale.z*clpOGFMZ);
    R_ASSERT(ix<=clpOGFMX && iy<=clpOGFMY && iz<=clpOGFMZ);

	int similar_pos=-1;
    {
        U32Vec& vl=m_VM[ix][iy][iz];
        for(U32It it=vl.begin();it!=vl.end(); it++){
        	SOGFVert& src=m_Verts[*it];
        	if(src.similar_pos(V)){
	            if(src.similar(V)){
                    P = *it;
                    break;
            	}
                similar_pos=*it;
            }
        }
    }
    if (0xffffffff==P)
    {
    	if (similar_pos>=0) V.P.set(m_Verts[similar_pos].P);
        P = m_Verts.size();
    	if (P>=0xFFFF) return 0xffffffff;
        m_Verts.push_back(V);

        m_VM[ix][iy][iz].push_back(P);

        u32 ixE,iyE,izE;
        ixE = iFloor(float(V.P.x+m_VMeps.x-m_VMmin.x)/m_VMscale.x*clpOGFMX);
        iyE = iFloor(float(V.P.y+m_VMeps.y-m_VMmin.y)/m_VMscale.y*clpOGFMY);
        izE = iFloor(float(V.P.z+m_VMeps.z-m_VMmin.z)/m_VMscale.z*clpOGFMZ);

        R_ASSERT(ixE<=clpOGFMX && iyE<=clpOGFMY && izE<=clpOGFMZ);

        if (ixE!=ix)							m_VM[ixE][iy][iz].push_back	(P);
        if (iyE!=iy)							m_VM[ix][iyE][iz].push_back	(P);
        if (izE!=iz)							m_VM[ix][iy][izE].push_back	(P);
        if ((ixE!=ix)&&(iyE!=iy))				m_VM[ixE][iyE][iz].push_back(P);
        if ((ixE!=ix)&&(izE!=iz))				m_VM[ixE][iy][izE].push_back(P);
        if ((iyE!=iy)&&(izE!=iz))				m_VM[ix][iyE][izE].push_back(P);
        if ((ixE!=ix)&&(iyE!=iy)&&(izE!=iz))	m_VM[ixE][iyE][izE].push_back(P);
    }
    return P;
}
//----------------------------------------------------
void CObjectOGFCollectorPacked::ComputeBounding()
{
	m_Box.invalidate		();
//	FvectorVec				split_verts;
//    split_verts.resize		(m_Verts.size());
//    FvectorIt r_it=split_verts.begin();
    for (OGFVertIt v_it=m_Verts.begin(); v_it!=m_Verts.end(); v_it++/*,r_it++*/){
//        r_it->set			(v_it->P);
        m_Box.modify		(v_it->P);
    }
/*
    // compute OBB
    if (split_verts.size()<3) { m_OBB.invalidate(); return; }
    Mgc::Box3	BOX			= Mgc::MinBox(split_verts.size(), (const Mgc::Vector3*) split_verts.begin());
    m_OBB.m_rotate.i.set	(BOX.Axis(0));
    m_OBB.m_rotate.j.set	(BOX.Axis(1));
    m_OBB.m_rotate.k.set	(BOX.Axis(2));
    m_OBB.m_translate.set	(BOX.Center());
    m_OBB.m_halfsize.set	(BOX.Extents()[0],BOX.Extents()[1],BOX.Extents()[2]);
*/
}
//----------------------------------------------------

CExportObjectOGF::SSplit::SSplit(CSurface* surf, const Fbox& bb)
{
    apx_box				= bb;
	m_Surf 				= surf;
//    I_Current=V_Minimal=-1;
}
//----------------------------------------------------

CExportObjectOGF::SSplit::~SSplit()
{
	for (COGFCPIt it=m_Parts.begin(); it!=m_Parts.end(); it++)
    	xr_delete(*it);
}
//----------------------------------------------------

void CExportObjectOGF::SSplit::AppendPart(int apx_vertices, int apx_faces)
{
	m_Parts.push_back	(new CObjectOGFCollectorPacked(apx_box,apx_vertices, apx_faces));
    m_CurrentPart		= m_Parts.back();
}
//----------------------------------------------------

void CExportObjectOGF::SSplit::Save(IWriter& F, int& chunk_id)
{
	m_Box.invalidate();
	for (COGFCPIt it=m_Parts.begin(); it!=m_Parts.end(); it++){
	    CObjectOGFCollectorPacked* part = *it;
        part->ComputeBounding	();
		m_Box.merge				(part->m_Box);
        F.open_chunk			(chunk_id);
        {
            // Header
            F.open_chunk		(OGF_HEADER);
            ogf_header			H;
            H.format_version	= xrOGF_FormatVersion;
            H.type				= /*(I_Current>=0)?MT_PROGRESSIVE:*/MT_NORMAL;
            H.flags				= 0;
            F.w					(&H,sizeof(H));
            F.close_chunk		();

            // Texture
            F.open_chunk		(OGF_TEXTURE);
            F.w_stringZ			(m_Surf->_Texture());
            F.w_stringZ			(m_Surf->_ShaderName());
            F.close_chunk		();

            // Vertices
            u32 dwFVF			= D3DFVF_XYZ|D3DFVF_NORMAL|(1<<D3DFVF_TEXCOUNT_SHIFT);
            F.open_chunk		(OGF_VERTICES);
            F.w_u32				(dwFVF);
            F.w_u32				(part->m_Verts.size());
            for (OGFVertIt v_it=part->m_Verts.begin(); v_it!=part->m_Verts.end(); v_it++){
                SOGFVert& pV 	= *v_it;
                F.w				(&(pV.P),sizeof(float)*3);		// position (offset)
                F.w				(&(pV.N),sizeof(float)*3);		// normal
                F.w_float		(pV.UV.x); F.w_float(pV.UV.y);	// tu,tv
            }
            F.close_chunk		();

            // Faces
            F.open_chunk(OGF_INDICES);
            F.w_u32(part->m_Faces.size()*3);
            F.w(part->m_Faces.begin(),part->m_Faces.size()*3*sizeof(u16));
            F.close_chunk();
/*
            // PMap
            if (I_Current>=0) {
                F.open_chunk(OGF_P_MAP);
                {
                    F.open_chunk(0x1);
                    F.w_u32(V_Minimal);
                    F.w_u32(I_Current);
                    F.close_chunk();
                }
                {
                    F.open_chunk(0x2);
                    F.w(pmap_vsplit.begin(),pmap_vsplit.size()*sizeof(Vsplit));
                    F.close_chunk();
                }
                {
                    F.open_chunk(0x3);
                    F.w_u32(pmap_faces.size());
                    F.w(pmap_faces.begin(),pmap_faces.size()*sizeof(WORD));
                    F.close_chunk();
                }
                F.close_chunk();
            }
*/
            // BBox (already computed)
            F.open_chunk		(OGF_BBOX);
            F.w					(&part->m_Box,sizeof(part->m_Box));
            F.close_chunk		();

/*
            F.open_chunk		(OGF_OBB);
            F.w					(&part->m_OBB,sizeof(part->m_OBB));
            F.close_chunk		();
*/
        }
        F.close_chunk();
        chunk_id++;
    }
}
//----------------------------------------------------
/*
void CExportObjectOGF::SSplit::MakeProgressive(){
    // Progressive
    I_Current=V_Minimal=-1;
    if (m_Faces.size()>1) {
        // Options
        PM_Init(1,1,4,0.1f,1,1,120,0.15f,0.95f);

        // Transfer vertices
        for (OGFVertIt vert_it=m_Verts.begin(); vert_it!=m_Verts.end(); vert_it++){
            SOGFVert	&iV = *vert_it;
            PM_CreateVertex(iV.P.x,iV.P.y,iV.P.z,vert_it - m_Verts.begin(),(P_UV*)(&iV.UV));
        }

        // Convert
        PM_Result R;
        I_Current = PM_Convert((LPWORD)m_Faces.begin(),m_Faces.size()*3, &R);
        if (I_Current>=0) {
            u32 progress_diff = m_Verts.size()-R.minVertices;
            if (progress_diff!=R.splitSIZE){
                ELog.Msg(mtError,"PM_Convert return wrong indices.");
                I_Current = -1;
                return;
            }
            // Permute vertices
            OGFVertVec temp_list = m_Verts;

            // Perform permutation
            for(u32 i=0; i<temp_list.size(); i++)
                m_Verts[R.permutePTR[i]]=temp_list[i];

            // Copy results
            pmap_vsplit.resize(R.splitSIZE);
            CopyMemory(pmap_vsplit.begin(),R.splitPTR,R.splitSIZE*sizeof(Vsplit));

            pmap_faces.resize(R.facefixSIZE);
            CopyMemory(pmap_faces.begin(),R.facefixPTR,R.facefixSIZE*sizeof(WORD));

            V_Minimal = R.minVertices;
        }
    }
}
//----------------------------------------------------
*/
CExportObjectOGF::CExportObjectOGF(CEditableObject* object)
{
	m_Source=object;
}
//----------------------------------------------------

CExportObjectOGF::~CExportObjectOGF()
{
	for (SplitIt it=m_Splits.begin(); it!=m_Splits.end(); it++)
    	xr_delete(*it);
}
//----------------------------------------------------

CExportObjectOGF::SSplit* CExportObjectOGF::FindSplit(CSurface* surf)
{
	for (SplitIt it=m_Splits.begin(); it!=m_Splits.end(); it++)
    	if ((*it)->m_Surf==surf) return *it;
    return 0;
}
//----------------------------------------------------

bool CExportObjectOGF::ExportGeometry(IWriter& F)
{
    if( m_Source->MeshCount() == 0 ) return false;

    AnsiString capt = AnsiString("Build OGF: '")+m_Source->GetName()+"'";
    UI.ProgressStart(m_Source->MeshCount()+m_Source->SurfaceCount(),capt.c_str());
    UI.ProgressInc();

    CTimer tm;
    tm.Start();
    for(EditMeshIt mesh_it=m_Source->FirstMesh();mesh_it!=m_Source->LastMesh();mesh_it++){
        CEditableMesh* MESH = *mesh_it;
        // generate normals
        if (!MESH->m_LoadState.is(CEditableMesh::LS_PNORMALS)) MESH->GeneratePNormals();
        // fill faces
        for (SurfFacesPairIt sp_it=MESH->m_SurfFaces.begin(); sp_it!=MESH->m_SurfFaces.end(); sp_it++){
            IntVec& face_lst= sp_it->second;
        	U8Vec mark;		mark.resize(face_lst.size(),0);
            CSurface* surf = sp_it->first;
            u32 dwTexCnt 	= ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);	R_ASSERT(dwTexCnt==1);
            SSplit* split	= FindSplit(surf);
            if (0==split){
            	m_Splits.push_back(new SSplit(surf,m_Source->GetBox()));
                split		= m_Splits.back();
            }
            int elapsed_faces = face_lst.size();
            
            do{
                if (elapsed_faces>0) split->AppendPart(elapsed_faces>0xffff?0xffff:elapsed_faces,elapsed_faces>0xffff?0xffff:elapsed_faces);
                for (IntIt f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
                    st_Face& face = MESH->m_Faces[*f_it];
                    {
                        SOGFVert v[3];
                        for (int k=0; k<3; k++){
                            st_FaceVert& 	fv = face.pv[k];
                            int offs = 0;
                            Fvector2* 		uv = 0;
                            for (u32 t=0; t<dwTexCnt; t++){
                                st_VMapPt& vm_pt 	= MESH->m_VMRefs[fv.vmref][t+offs];
                                st_VMap& vmap		= *MESH->m_VMaps[vm_pt.vmap_index];
                                if (vmap.type!=vmtUV){ offs++; t--; continue; }
                                uv					= &vmap.getUV(vm_pt.index);
                            }
                            R_ASSERT2(uv,"uv empty");
                            v[k].set(MESH->m_Points[fv.pindex],MESH->m_PNormals[fv.pindex],*uv);
                        }                     
                        int idx = f_it-face_lst.begin();
                        if (!mark[idx]){
                            if (split->m_CurrentPart->add_face(v[0], v[1], v[2])){
                            	mark[idx]			= 1; 
                                elapsed_faces--;
                                if (surf->m_Flags.is(CSurface::sf2Sided)){
                                    v[0].N.invert(); v[1].N.invert(); v[2].N.invert();
                                    split->m_CurrentPart->add_face(v[0], v[2], v[1]);
                                }
                            }
                        }
                    }
                }
            }while(elapsed_faces>0);
		    UI.ProgressInc();
        }
        // mesh fin
        MESH->UnloadPNormals();
        MESH->UnloadFNormals();
	    UI.ProgressInc();
    }

	// create OGF
	// Saving geometry...
    Fbox BBox;   	BBox.invalidate	();

    // Header
    ogf_header 		H;
    H.format_version= xrOGF_FormatVersion;
    H.type			= MT_HIERRARHY;
    H.flags			= 0;
    F.w_chunk		(OGF_HEADER,&H,sizeof(H));

    // OGF_CHILDREN
    F.open_chunk	(OGF_CHILDREN);
    int chunk=0;
    for (SplitIt split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++){
        (*split_it)->Save(F,chunk);
		BBox.merge	((*split_it)->m_Box);
    }
    F.close_chunk	();
    UI.ProgressInc	();

    // BBox (already computed)
    F.open_chunk	(OGF_BBOX);
    F.w				(&BBox,sizeof(Fbox));
    F.close_chunk	();

    UI.ProgressEnd	();

    tm.Stop			();
    ELog.Msg		(mtInformation,"Build time: %3.2f sec",float(tm.GetElapsed_ms())/1000.f);
    
    return true;
}
//------------------------------------------------------------------------------

