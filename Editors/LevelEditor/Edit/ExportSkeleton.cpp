//----------------------------------------------------
// file: ExportSkeleton.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ExportSkeleton.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "fmesh.h"
#include "std_classes.h"
#include "bone.h"
#include "motion.h"
#include "xr_ini.h"
#include "MgcCont3DMinBox.h"
#include "ui_main.h"

DWORD CSkeletonCollectorPacked::VPack(SSkelVert& V){
    DWORD P 	= 0xffffffff;

    DWORD ix,iy,iz;
    ix = iFloor(float(V.P.x-m_VMmin.x)/m_VMscale.x*clpSMX);
    iy = iFloor(float(V.P.y-m_VMmin.y)/m_VMscale.y*clpSMY);
    iz = iFloor(float(V.P.z-m_VMmin.z)/m_VMscale.z*clpSMZ);
    R_ASSERT(ix<=clpSMX && iy<=clpSMY && iz<=clpSMZ);

	int similar_pos=-1;
    {
        DWORDVec& vl=m_VM[ix][iy][iz];
        for(DWORDIt it=vl.begin();it!=vl.end(); it++){
        	SSkelVert& src=m_Verts[*it];
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
        m_Verts.push_back(V);

        m_VM[ix][iy][iz].push_back(P);

        DWORD ixE,iyE,izE;
        ixE = iFloor(float(V.P.x+m_VMeps.x-m_VMmin.x)/m_VMscale.x*clpSMX);
        iyE = iFloor(float(V.P.y+m_VMeps.y-m_VMmin.y)/m_VMscale.y*clpSMY);
        izE = iFloor(float(V.P.z+m_VMeps.z-m_VMmin.z)/m_VMscale.z*clpSMZ);

        R_ASSERT(ixE<=clpSMX && iyE<=clpSMY && izE<=clpSMZ);

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

CSkeletonCollectorPacked::CSkeletonCollectorPacked(const Fbox &bb, int apx_vertices, int apx_faces){
    // Params
    m_VMscale.set	(bb.max.x-bb.min.x, bb.max.y-bb.min.y, bb.max.z-bb.min.z);
    m_VMmin.set		(bb.min);
    m_VMeps.set		(m_VMscale.x/clpSMX/2,m_VMscale.y/clpSMY/2,m_VMscale.z/clpSMZ/2);
    m_VMeps.x		= (m_VMeps.x<EPS_L)?m_VMeps.x:EPS_L;
    m_VMeps.y		= (m_VMeps.y<EPS_L)?m_VMeps.y:EPS_L;
    m_VMeps.z		= (m_VMeps.z<EPS_L)?m_VMeps.z:EPS_L;

    // Preallocate memory
    m_Verts.reserve	(apx_vertices);
    m_Faces.reserve	(apx_faces);

    int		_size	= (clpSMX+1)*(clpSMY+1)*(clpSMZ+1);
    int		_average= (apx_vertices/_size)/2;
    for (int ix=0; ix<clpSMX+1; ix++)
        for (int iy=0; iy<clpSMY+1; iy++)
            for (int iz=0; iz<clpSMZ+1; iz++)
                m_VM[ix][iy][iz].reserve	(_average);
}
//----------------------------------------------------

CExportSkeleton::SSplit::SSplit(CSurface* surf, const Fbox& bb):CSkeletonCollectorPacked(bb){
	strcpy(m_Texture,surf->_Texture());
	strcpy(m_Shader,surf->_ShaderName());
    I_Current=V_Minimal=-1;
}
//----------------------------------------------------

void CExportSkeleton::SSplit::Save(CFS_Base& F, BOOL b2Link)
{
    // Header
    F.open_chunk		(OGF_HEADER);
    ogf_header			H;
    H.format_version	= xrOGF_FormatVersion;
    H.type				= (I_Current>=0)?MT_SKELETON_PART:MT_SKELETON_PART_STRIPPED;
    H.flags				= 0;
    F.write				(&H,sizeof(H));
    F.close_chunk		();

    // Texture
    F.open_chunk		(OGF_TEXTURE);
    F.WstringZ			(m_Texture);
    F.WstringZ			(m_Shader);
    F.close_chunk		();

    // Vertices
    m_Box.invalidate	();
    F.open_chunk		(OGF_VERTICES);
    F.Wdword			(b2Link?2*0x12071980:1*0x12071980);
    F.Wdword			(m_Verts.size());
    if (b2Link){
        for (SkelVertIt v_it=m_Verts.begin(); v_it!=m_Verts.end(); v_it++){
            SSkelVert& pV 	= *v_it;
            m_Box.modify(pV.P);
			// write vertex
            F.Wword(pV.B0);
            F.Wword(pV.B1);
            F.write(&(pV.O0),sizeof(Fvector));		// position (offset)
            F.write(&(pV.N0),sizeof(Fvector));		// normal
            F.write(&(pV.O1),sizeof(Fvector));		// position (offset)
            F.write(&(pV.N1),sizeof(Fvector));		// normal
            F.Wfloat(pV.w);
            F.Wfloat(pV.UV.x); F.Wfloat(pV.UV.y);	// tu,tv
        }
    }else{
        for (SkelVertIt v_it=m_Verts.begin(); v_it!=m_Verts.end(); v_it++){
            SSkelVert& pV 	= *v_it;
            m_Box.modify(pV.P);
            F. write(&(pV.O0),sizeof(float)*3);		// position (offset)
            F.write(&(pV.N0),sizeof(float)*3);		// normal
            F.Wfloat(pV.UV.x); F.Wfloat(pV.UV.y);		// tu,tv
            F.Wdword(pV.B0);
        }
    }
    F.close_chunk();

    // Faces
    F.open_chunk(OGF_INDICES);
    F.Wdword(m_Faces.size()*3);
    F.write(m_Faces.begin(),m_Faces.size()*3*sizeof(WORD));
    F.close_chunk();

    // PMap
    if (I_Current>=0) {
        F.open_chunk(OGF_P_MAP);
        {
            F.open_chunk(0x1);
            F.Wdword(V_Minimal);
            F.Wdword(I_Current);
            F.close_chunk();
        }
        {
            F.open_chunk(0x2);
            F.write(pmap_vsplit.begin(),pmap_vsplit.size()*sizeof(Vsplit));
            F.close_chunk();
        }
        {
            F.open_chunk(0x3);
            F.Wdword(pmap_faces.size());
            F.write(pmap_faces.begin(),pmap_faces.size()*sizeof(WORD));
            F.close_chunk();
        }
        F.close_chunk();
    }

    // BBox (already computed)
    F.open_chunk(OGF_BBOX);
    F.write(&m_Box,sizeof(Fvector)*2);
    F.close_chunk();
}

void CExportSkeleton::SSplit::MakeProgressive(){
    // Progressive
    I_Current=V_Minimal=-1;
    if (m_Faces.size()>1) {
        // Options
        PM_Init(1,1,4,0.1f,1,1,120,0.15f,0.95f);

        // Transfer vertices
        for (SkelVertIt vert_it=m_Verts.begin(); vert_it!=m_Verts.end(); vert_it++){
            SSkelVert	&iV = *vert_it;
            PM_CreateVertex(iV.P.x,iV.P.y,iV.P.z,vert_it - m_Verts.begin(),(P_UV*)(&iV.UV));
        }

        // Convert
        PM_Result R;
        I_Current = PM_Convert((LPWORD)m_Faces.begin(),m_Faces.size()*3, &R);
        if (I_Current>=0) {
            DWORD progress_diff = m_Verts.size()-R.minVertices;
            if (progress_diff!=R.splitSIZE){
                ELog.Msg(mtError,"PM_Convert return wrong indices.");
                I_Current = -1;
                return;
            }
            // Permute vertices
            SkelVertVec temp_list = m_Verts;

            // Perform permutation
            for(DWORD i=0; i<temp_list.size(); i++)
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

CExportSkeleton::CExportSkeleton(CEditableObject* object)
{
	m_Source=object;
}
//----------------------------------------------------

void CExportSkeleton::ComputeOBB	(Fobb &B, FvectorVec& V)
{
    if (V.size()<3) { B.invalidate(); return; }
    Mgc::Box3	BOX		= Mgc::MinBox(V.size(), (const Mgc::Vector3*) V.begin());
    B.m_rotate.i.set	(BOX.Axis(0));
    B.m_rotate.j.set	(BOX.Axis(1));
    B.m_rotate.k.set	(BOX.Axis(2));
    B.m_translate.set	(BOX.Center());
    B.m_halfsize.set	(BOX.Extents()[0],BOX.Extents()[1],BOX.Extents()[2]);
}
//----------------------------------------------------

int CExportSkeleton::FindSplit(LPCSTR shader, LPCSTR texture)
{
	for (SplitIt it=m_Splits.begin(); it!=m_Splits.end(); it++){
		if ((0==stricmp(it->m_Texture,texture))&&(0==stricmp(it->m_Shader,shader))) return it-m_Splits.begin();
    }
    return -1;
}
//----------------------------------------------------

bool CExportSkeleton::ExportGeometry(CFS_Base& F)
{
    if( m_Source->MeshCount() == 0 ) return false;

    // mem active motion
    CSMotion* active_motion=m_Source->ResetSAnimation();

    R_ASSERT(m_Source->IsFlag(CEditableObject::eoDynamic)&&m_Source->IsSkeleton());
    BOOL b2Link = FALSE;

    UI.ProgressStart(5+m_Source->MeshCount()*2+m_Source->SurfaceCount(),"Export skeleton geometry...");
    UI.ProgressInc();

    vector<FvectorVec>	bone_points;
	bone_points.resize	(m_Source->BoneCount());

    FvectorVec vnormals;
    DWORD mtl_cnt=0;
	UI.SetStatus("Split meshes...");
    for(EditMeshIt mesh_it=m_Source->FirstMesh();mesh_it!=m_Source->LastMesh();mesh_it++){
        CEditableMesh* MESH = *mesh_it;
        // generate vertex offset
        if (!(MESH->m_LoadState&EMESH_LS_SVERTICES)) MESH->GenerateSVertices();
        // generate normals
        if (!(MESH->m_LoadState&EMESH_LS_FNORMALS)) MESH->GenerateFNormals();
        vnormals.clear();
        Fvector N;
        for(FvectorIt pt=MESH->m_Points.begin();pt!=MESH->m_Points.end();pt++){
            N.set(0,0,0);
            IntVec& a_lst = MESH->m_Adjs[pt-MESH->m_Points.begin()];
            VERIFY(a_lst.size());
            for (IntIt i_it=a_lst.begin(); i_it!=a_lst.end(); i_it++)
                N.add(MESH->m_FNormals[*i_it]);
            N.normalize_safe();
            vnormals.push_back(N);
        }
	    UI.ProgressInc();
        // fill faces
        for (SurfFacesPairIt sp_it=MESH->m_SurfFaces.begin(); sp_it!=MESH->m_SurfFaces.end(); sp_it++){
            IntVec& face_lst = sp_it->second;
            CSurface* surf = sp_it->first;
            DWORD dwTexCnt = ((surf->_FVF()&D3DFVF_TEXCOUNT_MASK)>>D3DFVF_TEXCOUNT_SHIFT);
            R_ASSERT(dwTexCnt==1);
            int mtl_idx = FindSplit(surf->_ShaderName(),surf->_Texture());
            if (mtl_idx<0){
            	m_Splits.push_back(SSplit(surf,m_Source->GetBox()));
                mtl_idx=mtl_cnt++;
            }
            SSplit& split=m_Splits[mtl_idx];
            for (IntIt f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
                st_Face& face = MESH->m_Faces[*f_it];
                {
                    SSkelVert v[3];
                    for (int k=0; k<3; k++){
                        st_FaceVert& 	fv = face.pv[k];
                        st_SVert& 		sv = MESH->m_SVertices[fv.pindex];
                        int offs = 0;
                        Fvector2* 		uv = 0;
                        for (DWORD t=0; t<dwTexCnt; t++){
                            st_VMapPt& vm_pt 	= MESH->m_VMRefs[fv.vmref][t+offs];
                            st_VMap& vmap		= *MESH->m_VMaps[vm_pt.vmap_index];
                            if (vmap.type!=vmtUV){ offs++; t--; continue; }
                            uv					= &vmap.getUV(vm_pt.index);
                        }
                        R_ASSERT2(uv,"uv empty");
                        v[k].set(MESH->m_Points[fv.pindex],*uv,sv.w);

                        Fvector N0,N1;
		                CBone* B;
                        B = m_Source->GetBone(sv.bone0);
        		        B->LITransform().transform_dir(N0,vnormals[fv.pindex]);
                        v[k].set0(sv.offs0,N0,sv.bone0);
                        if (sv.bone1!=-1){
                        	b2Link = TRUE;
			                B = m_Source->GetBone(sv.bone1);
    	    		        B->LITransform().transform_dir(N1,vnormals[fv.pindex]);
        	                v[k].set1(sv.offs1,N1,sv.bone1);
                        }else{
        	                v[k].set1(sv.offs0,N0,sv.bone0);
                        }
                    }
                    split.add_face(v[0], v[1], v[2]);
			        if (surf->IsFlag(CSurface::sf2Sided)){
                    	v[0].N0.invert(); v[1].N0.invert(); v[2].N0.invert();
                    	v[0].N1.invert(); v[1].N1.invert(); v[2].N1.invert();
                    	split.add_face(v[0], v[2], v[1]);
                    }
                }
            }
        }
        // mesh fin
        MESH->UnloadSVertices();
        MESH->UnloadFNormals();
	    UI.ProgressInc();
    }
    UI.SetStatus("Make progressive...");
    // fill per bone vertices
    for (SplitIt split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++){
		SkelVertVec& lst = split_it->getV_Verts();
	    for (SkelVertIt sv_it=lst.begin(); sv_it!=lst.end(); sv_it++)
		    bone_points[sv_it->B0].push_back(sv_it->O0);
        if (m_Source->IsFlag(CEditableObject::eoProgressive)) split_it->MakeProgressive();
		UI.ProgressInc();
    }
	UI.ProgressInc();

	// create OGF

	// Saving geometry...
    Fbox rootBB;

    // Header
    ogf_header 		H;
    H.format_version= xrOGF_FormatVersion;
    H.type			= MT_SKELETON;
    H.flags			= 0;
    F.write_chunk	(OGF_HEADER,&H,sizeof(H));

    // OGF_CHILDREN
    F.open_chunk	(OGF_CHILDREN);
    int chield=0;
    for (split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++){
	    F.open_chunk(chield++);
        split_it->Save(F,b2Link);
	    F.close_chunk();
		rootBB.merge(split_it->m_Box);
    }
    F.close_chunk();
    UI.ProgressInc();


    UI.SetStatus("Compute bounding volume...");
    // BBox (already computed)
    F.open_chunk(OGF_BBOX);
    F.write(&rootBB,sizeof(Fbox));
    F.close_chunk();
	UI.ProgressInc();

    // BoneNames
    F.open_chunk(OGF_BONE_NAMES);
    F.Wdword(m_Source->BoneCount());
    int bone_idx=0;
    for (BoneIt bone_it=m_Source->FirstBone(); bone_it!=m_Source->LastBone(); bone_it++,bone_idx++){
        F.WstringZ	((*bone_it)->Name());
        F.WstringZ	(((*bone_it)->ParentIndex()==-1)?"":(*bone_it)->Parent());
        Fobb	obb;
        ComputeOBB	(obb,bone_points[bone_idx]);
        F.write	(&obb,sizeof(Fobb));
    }
    F.close_chunk();
	UI.ProgressInc();
    UI.ProgressEnd();

    // restore active motion
    m_Source->SetActiveSMotion(active_motion);

    return true;
}
//----------------------------------------------------

bool CExportSkeleton::ExportMotions(CFS_Base& F)
{
    if (m_Source->SMotionCount()<=0) return false;

    UI.ProgressStart(3+m_Source->SMotionCount(),"Export skeleton motions...");
    UI.ProgressInc();

    // mem active motion
    CSMotion* active_motion=m_Source->ResetSAnimation();

    // Motions
    F.open_chunk(OGF_MOTIONS);
    F.open_chunk(0);
    F.Wdword(m_Source->SMotionCount());
    F.close_chunk();
    int smot = 1;
    for (SMotionIt motion_it=m_Source->FirstSMotion(); motion_it!=m_Source->LastSMotion(); motion_it++, smot++){
        CSMotion* motion = *motion_it;
        F.open_chunk(smot);
        F.WstringZ(motion->Name());
        F.Wdword(motion->Length());
        BoneMotionVec& lst=motion->BoneMotions();
        int bone_id = 0;
        for (BoneMotionIt bm_it=lst.begin(); bm_it!=lst.end(); bm_it++,bone_id++){
            DWORD flag = motion->GetMotionFlag(bone_id);
            CBone* B = m_Source->GetBone(bone_id);
            int parent_idx = B->ParentIndex();
            for (int frm=motion->FrameStart(); frm<motion->FrameEnd(); frm++){
                float t = (float)frm/motion->FPS();
                Fvector T,R;
                Fmatrix mat;
                Fquaternion q;
                motion->Evaluate	(bone_id,t,T,R);

                if (flag&WORLD_ORIENTATION){
                    Fmatrix 	parent;
                    Fmatrix 	inv_parent;
                    if(parent_idx>-1){
                        m_Source->GetBoneWorldTransform(parent_idx,t,motion,parent);
                        inv_parent.invert(parent);
                    }else{
                        parent 		= Fidentity;
                        inv_parent	= Fidentity;
                    }
                    Fmatrix 	rot;
                    rot.setHPB	(-R.x,-R.y,-R.z);
                    mat.mul	(inv_parent,rot);
                }else{
                    mat.setHPB	(-R.x,-R.y,-R.z);
                }

                q.set		(mat);

                // Quantize quaternion
                int	_x = int(q.x*KEY_Quant); clamp(_x,-32767,32767); short x =  _x; F.write(&x,2);
                int	_y = int(q.y*KEY_Quant); clamp(_y,-32767,32767); short y =  _y; F.write(&y,2);
                int	_z = int(q.z*KEY_Quant); clamp(_z,-32767,32767); short z =  _z; F.write(&z,2);
                int	_w = int(q.w*KEY_Quant); clamp(_w,-32767,32767); short w =  _w; F.write(&w,2);
                F.Wvector(T);
            }
        }
        F.close_chunk();
	    UI.ProgressInc();
    }
    F.close_chunk();
    UI.ProgressInc();

    // save smparams
    F.open_chunk		(OGF_SMPARAMS2);
    F.Wword				(xrOGF_SMParamsVersion);
    // bone parts
    BPVec& bp_lst 		= m_Source->BoneParts();
    if (bp_lst.size()){
		F.Wword(bp_lst.size());
    	for (BPIt bp_it=bp_lst.begin(); bp_it!=bp_lst.end(); bp_it++){
    		F.WstringZ(bp_it->alias.c_str());
            F.Wword(bp_it->bones.size());
//	        F.write(bp_it->bones.begin(),bp_it->bones.size()*sizeof(int));
	        for (int i=0; i<int(bp_it->bones.size()); i++)
            	F.Wdword(bp_it->bones[i]);
    	}
    }else{
		F.Wword(1);
		F.WstringZ("default");
		F.Wword(m_Source->BoneCount());
        for (int i=0; i<m_Source->BoneCount(); i++) F.Wdword(i);
    }
    // motion defs
    SMotionVec& sm_lst 		= m_Source->SMotions();
	F.Wword(sm_lst.size());
    for (motion_it=sm_lst.begin(); motion_it!=sm_lst.end(); motion_it++){
        CSMotion* motion = *motion_it;
        F.WstringZ	(motion->Name());
        F.Wdword	(motion->m_dwFlags);
		F.Wword		(motion->iBoneOrPart);
        F.Wword		(motion_it-sm_lst.begin());
        F.Wfloat	(motion->fSpeed);
        F.Wfloat	(motion->fPower);
        F.Wfloat	(motion->fAccrue);
        F.Wfloat	(motion->fFalloff);
    }
    F.close_chunk();
	UI.ProgressInc();
    UI.ProgressEnd();

    // restore active motion
    m_Source->SetActiveSMotion(active_motion);
    return true;
}
//----------------------------------------------------

bool CExportSkeleton::Export(CFS_Base& F){
    if (!ExportGeometry(F)) return false;
    if (!ExportMotions(F)) return false;
    return true;
};
//----------------------------------------------------

//------------------------------------------------------------------------------

