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
	for (COGFCPIt it=m_Parts.begin(); it!=m_Parts.end(); it++){
	    CObjectOGFCollectorPacked* part = *it;
        F.open_chunk			(chunk_id);
        {
            // Header
            F.open_chunk		(OGF_HEADER);
            ogf_header			H;
            H.format_version	= xrOGF_FormatVersion;
            H.type				= (part->m_SWR.size())?MT_PROGRESSIVE:MT_NORMAL;
            H.shader_id			= 0;
            H.bb.min			= part->m_Box.min;
            H.bb.max			= part->m_Box.max;
            part->m_Box.getsphere(H.bs.c,H.bs.r);
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

            // PMap
            if (part->m_SWR.size()) {
                F.open_chunk(OGF_P_LODS);
                F.w_u32			(0);			// reserved space 16 bytes
                F.w_u32			(0);
                F.w_u32			(0);
                F.w_u32			(0);
                F.w_u32			(part->m_SWR.size()); // num collapses
                for (u32 swr_idx=0; swr_idx<part->m_SWR.size(); swr_idx++)
                    F.w			(&part->m_SWR[swr_idx],sizeof(VIPM_SWR));
                F.close_chunk();
            }
/*
			AnsiString r=AnsiString("x:\\import\\test")+chunk_id+".smf";
            IWriter* W 	= FS.w_open(r.c_str());
// optimize
/*
			FvectorVec 			pts;
            U32Vec 				remap_idx	(part->m_Verts.size(),0);
            for (v_it=part->m_Verts.begin(); v_it!=part->m_Verts.end(); v_it++){
                SOGFVert& pV 	= *v_it;
                BOOL bFound		= FALSE;
                for (FvectorIt vert_it=pts.begin(); vert_it!=pts.end(); vert_it++){
                	if (vert_it->similar(pV.P,EPS_L)){
                    	remap_idx[v_it-part->m_Verts.begin()]=vert_it-pts.begin();
		                bFound	= TRUE;
                    	break;
                    }
                }
                if (!bFound){
                    remap_idx[v_it-part->m_Verts.begin()]=pts.size();
                	pts.push_back(pV.P);
                }
            }
			
            for (FvectorIt vert_it=pts.begin(); vert_it!=pts.end(); vert_it++){
            	AnsiString 		tmp;
                tmp.sprintf		("v %f %f %f",vert_it->x,vert_it->y,-vert_it->z);
                W->w_string		(tmp.c_str());
            }
            for (OGFFaceIt f_it=part->m_Faces.begin(); f_it!=part->m_Faces.end(); f_it++){
                SOGFFace& pF 	= *f_it;
            	AnsiString 		tmp;
                tmp.sprintf		("f %d %d %d",remap_idx[pF.v[0]]+1,remap_idx[pF.v[2]]+1,remap_idx[pF.v[1]]+1);
                W->w_string		(tmp.c_str());
            }
*/
/*
			// vertices
            for (v_it=part->m_Verts.begin(); v_it!=part->m_Verts.end(); v_it++){
                SOGFVert& pV 	= *v_it;
            	AnsiString 		tmp;
                tmp.sprintf		("v %f %f %f",pV.P.x,pV.P.y,-pV.P.z);
                W->w_string		(tmp.c_str());
            }
            // face
            for (OGFFaceIt f_it=part->m_Faces.begin(); f_it!=part->m_Faces.end(); f_it++){
                SOGFFace& pF 	= *f_it;
            	AnsiString 		tmp;
                tmp.sprintf		("f %d %d %d",pF.v[0]+1,pF.v[2]+1,pF.v[1]+1);
                W->w_string		(tmp.c_str());
            }
            // normals
            W->w_string			("bind n vertex");
            for (v_it=part->m_Verts.begin(); v_it!=part->m_Verts.end(); v_it++){
                SOGFVert& pV 	= *v_it;
            	AnsiString 		tmp;
                tmp.sprintf		("n %f %f %f",pV.N.x,pV.N.y,-pV.N.z);
                W->w_string		(tmp.c_str());
            }
			FS.w_close	(W);
//*/            
        }
        F.close_chunk();
        chunk_id++;
    }
}
//----------------------------------------------------

void CObjectOGFCollectorPacked::MakeProgressive()
{
	VIPM_Init	();
    for (OGFVertIt vert_it=m_Verts.begin(); vert_it!=m_Verts.end(); vert_it++)
    	VIPM_AppendVertex(vert_it->P,vert_it->UV);
    for (OGFFaceIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++)
    	VIPM_AppendFace(f_it->v[0],f_it->v[1],f_it->v[2]);

    VIPM_Result* R = VIPM_Convert(u32(-1),1.f,1);

    // Permute vertices
    OGFVertVec temp_list = m_Verts;
    for(u32 i=0; i<temp_list.size(); i++)
        m_Verts[R->permute_verts[i]]=temp_list[i];
    
    // Fill indices
    m_Faces.resize	(R->indices.size()/3);
    for (u32 f_idx=0; f_idx<m_Faces.size(); f_idx++){
	    SOGFFace& F		= m_Faces[f_idx];
    	F.v[0]			= R->indices[f_idx*3+0];
    	F.v[1]			= R->indices[f_idx*3+1];
    	F.v[2]			= R->indices[f_idx*3+2];
    }

    // Fill SWR
    m_SWR.resize		(R->swr_records.size());
    for (u32 swr_idx=0; swr_idx!=m_SWR.size(); swr_idx++)
    	m_SWR[swr_idx]	= R->swr_records[swr_idx];

    // cleanup
    VIPM_Destroy		();
}
//----------------------------------------------------

void CExportObjectOGF::SSplit::MakeProgressive()
{
	for (COGFCPIt it=m_Parts.begin(); it!=m_Parts.end(); it++)
    	(*it)->MakeProgressive();
}

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

bool CExportObjectOGF::Export(IWriter& F)
{
    if( m_Source->MeshCount() == 0 ) return false;

    AnsiString capt = AnsiString	("Build OGF: '")+m_Source->GetName()+"'";
    SPBItem* pb		= UI->PBStart	(m_Source->MeshCount()+m_Source->SurfaceCount(),capt.c_str());
    UI->PBInc		(pb);

    CTimer tm;
    tm.Start();
    BOOL bResult = TRUE;
    for(EditMeshIt mesh_it=m_Source->FirstMesh();mesh_it!=m_Source->LastMesh();mesh_it++){
        CEditableMesh* MESH = *mesh_it;
//        // generate normals
        if (!MESH->m_LoadState.is(CEditableMesh::LS_PNORMALS)) MESH->GeneratePNormals();
        // fill faces
        for (SurfFacesPairIt sp_it=MESH->m_SurfFaces.begin(); sp_it!=MESH->m_SurfFaces.end(); sp_it++){
            IntVec& face_lst= sp_it->second;
        	U8Vec mark;		mark.resize(face_lst.size(),0);
            CSurface* surf 	= sp_it->first;
            u32 dwTexCnt 	= ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);	R_ASSERT(dwTexCnt==1);
            SSplit* split	= FindSplit(surf);
            if (0==split){
                SGameMtl* M = GMLib.GetMaterialByID(surf->_GameMtl());
                if (0==M){
                    ELog.DlgMsg		(mtError,"Surface: '%s' contains undefined game material.",surf->_Name());
                    bResult 		= FALSE; 
                    break; 
                }
                if (!M->Flags.is(SGameMtl::flDynamic)){
                    ELog.DlgMsg		(mtError,"Surface: '%s' contains non-dynamic game material.",surf->_Name());
                    bResult 		= FALSE; 
                    break; 
                }
            	m_Splits.push_back(new SSplit(surf,m_Source->GetBox()));
                split		= m_Splits.back();
            }
            int 	elapsed_faces 	= surf->m_Flags.is(CSurface::sf2Sided)?face_lst.size()*2:face_lst.size();
            const 	u8 	mark_mask	= surf->m_Flags.is(CSurface::sf2Sided)?0x03:0x01;
            
            do{
                if (elapsed_faces>0) split->AppendPart(elapsed_faces>0xffff?0xffff:elapsed_faces,elapsed_faces>0xffff?0xffff:elapsed_faces);
                for (IntIt f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
                    st_Face& face 	= MESH->m_Faces[*f_it];
                    {
                        int mark_idx= f_it-face_lst.begin();
                        if (mark_mask!=mark[mark_idx]){
                            SOGFVert v[3];
                            for (int k=0; k<3; k++){
                                st_FaceVert& 	fv = face.pv[k];
                                int offs 		= 0;
                                Fvector2* 		uv = 0;
                                for (u32 t=0; t<dwTexCnt; t++){
                                    st_VMapPt& vm_pt 	= MESH->m_VMRefs[fv.vmref][t+offs];
                                    st_VMap& vmap		= *MESH->m_VMaps[vm_pt.vmap_index];
                                    if (vmap.type!=vmtUV){ offs++; t--; continue; }
                                    uv					= &vmap.getUV(vm_pt.index);
                                }
                                R_ASSERT2		(uv,"uv empty");
                                u32 norm_id 	= (*f_it)*3+k;	R_ASSERT2(norm_id<MESH->m_PNormals.size(),"Normal index out of range.");
								v[k].set(MESH->m_Points[fv.pindex],MESH->m_PNormals[norm_id],*uv);
                            }                     
                            if (!(mark[mark_idx]&0x01)&&split->m_CurrentPart->add_face(v[0], v[1], v[2])){
                                mark[mark_idx]	|= 0x01; 
                                elapsed_faces--;
                            }
                            if ((mark_mask==0x03)&&!(mark[mark_idx]&0x02)){
								v[0].N.invert(); v[1].N.invert(); v[2].N.invert();
                            	if (split->m_CurrentPart->add_face(v[2], v[1], v[0])){
	                                mark[mark_idx]	|= 0x02; 
    	                            elapsed_faces--;
                                }
                            }
                        }
                    }
                }
            }while(elapsed_faces>0);
		    UI->PBInc(pb);
        }
        // mesh fin
        MESH->UnloadPNormals();
        MESH->UnloadFNormals();
	    UI->PBInc(pb);
    }

    if (!bResult){	
	    UI->PBEnd		(pb);
    	return 			false;
    }
    
    UI->SetStatus("Make progressive...");
    // fill per bone vertices
	if (m_Source->m_Flags.is(CEditableObject::eoProgressive)){
        for (SplitIt split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++){
            (*split_it)->MakeProgressive();
            UI->PBInc	(pb);
        }
    }
    UI->PBInc			(pb);

	// Compute bounding...
    ComputeBounding		();

	// create OGF
	// Saving geometry...

    // Header
    ogf_header 		H;
    H.format_version= xrOGF_FormatVersion;
    H.type			= MT_HIERRARHY;
    H.shader_id		= 0;
    H.bb.min		= m_Box.min;
    H.bb.max		= m_Box.max;
    m_Box.getsphere(H.bs.c,H.bs.r);
    F.w_chunk		(OGF_HEADER,&H,sizeof(H));

    // Desc
    ogf_desc		desc;
    m_Source->PrepareOGFDesc(desc);
    F.open_chunk	(OGF_S_DESC);
    desc.Save		(F);
    F.close_chunk	();

    // OGF_CHILDREN
    F.open_chunk	(OGF_CHILDREN);
    int chunk=0;
    for (SplitIt split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++)
        (*split_it)->Save(F,chunk);
    F.close_chunk	();
    UI->PBInc		(pb);

    UI->PBEnd		(pb);

    tm.Stop			();
    ELog.Msg		(mtInformation,"Build time: %3.2f sec",float(tm.GetElapsed_ms())/1000.f);
    
    return bResult;
}
//------------------------------------------------------------------------------

