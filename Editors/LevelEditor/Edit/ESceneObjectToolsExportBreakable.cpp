#include "stdafx.h"
#pragma hdrstop

#include "ESceneObjectTools.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "SceneObject.h"
#include "scene.h"
#include "ExportSkeleton.h"
#include "xrServer_Objects_ALife.h"
#include "clsid_game.h"

//----------------------------------------------------
struct SBVert{
	Fvector			pos;
public:
    bool			similar(const Fvector& v){return pos.similar(v,EPS_L);}
};
struct SBFace{
// prepare
    int 			vert[3];
    bool			marked;
// geom
    Fvector			o[3];
    Fvector2		uv[3];
    Fvector			n;
    Fvector			b;
    CSurface*		surf;
public:		
					SBFace	(CSurface* _surf, const Fvector2* _uv[3]):surf(_surf),marked(false)
	{
    	vert[0]		= -1;
    	vert[1]		= -1;
    	vert[2]		= -1;
        uv[0]		= *_uv[0];
        uv[1]		= *_uv[1];
        uv[2]		= *_uv[2];
        n.set		(0,0,0);
        b.set		(0,0,0);
    }
};
DEFINE_VECTOR		(SBVert*,SBVertVec,SBVertVecIt);
DEFINE_VECTOR		(SBFace*,SBFaceVec,SBFaceVecIt);

struct SBPart: public CExportSkeletonCustom{
	U32Vec 			part_faces;

    SBVertVec*		m_Verts;
    SBFaceVec*		m_Faces;
    Fbox			m_BBox;
    Fvector			m_Offset;
public:
					SBPart				(SBVertVec* _verts, SBFaceVec* _faces):m_Verts(_verts),m_Faces(_faces){m_BBox.invalidate();m_Offset.set(0,0,0);}
    virtual bool 	Export				(IWriter& F);
    void			append_face			(int id)
    {
    	part_faces.push_back(id);
        SBFace* F	= (*m_Faces)[id];
        for (int k=0; k<3; k++)
	        m_BBox.modify((*m_Verts)[F->vert[k]]->pos);
    }
};
DEFINE_VECTOR		(SBPart*,SBPartVec,SBPartVecIt);
DEFINE_VECTOR		(U32Vec,SBAdjVec,SBAdjVecIt);
//----------------------------------------------------
bool SBPart::Export	(IWriter& F)
{
	VERIFY			(!part_faces.empty());
    if (part_faces.size()>63){
    	ELog.Msg(mtError,"Breakable object cannot handle more than 63 faces.");
     	return false;
    }

    bool bRes = true;

    int bone_count			= part_faces.size()+1;
                    
    xr_vector<FvectorVec>	bone_points;
	bone_points.resize		(bone_count);

    u32 mtl_cnt				= 0;
    m_BBox.getcenter		(m_Offset);
    m_BBox.sub				(m_Offset);
    
    for (U32It pf_it=part_faces.begin(); pf_it!=part_faces.end(); pf_it++){
    	SBFace* face= (*m_Faces)[*pf_it]; VERIFY(face);
        int mtl_idx	= FindSplit(face->surf->_ShaderName(),face->surf->_Texture());
        if (mtl_idx<0){
            m_Splits.push_back(SSplit(face->surf,m_BBox));
            mtl_idx	= mtl_cnt++;
        }
        SSplit& split=m_Splits[mtl_idx];
        SSkelVert v[3];
        for (int k=0; k<3; k++){
        	Fvector p;
            p.sub		((*m_Verts)[face->vert[k]]->pos,m_Offset);
            v[k].set	(p,face->uv[k],1.f);
            v[k].set0	(face->o[k],face->n,pf_it-part_faces.begin()+1); //. нужно взять нормаль для вертекса
        }
        split.add_face(v[0], v[1], v[2]);
        if (face->surf->m_Flags.is(CSurface::sf2Sided)){
            v[0].N0.invert(); v[1].N0.invert(); v[2].N0.invert();
            v[0].N1.invert(); v[1].N1.invert(); v[2].N1.invert();
            split.add_face(v[0], v[2], v[1]);
        }
    }

    // fill per bone vertices
    for (SplitIt split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++){
        if (!split_it->valid()){
            ELog.Msg(mtError,"Degenerate part found (Texture '%s').",*split_it->m_Texture);
            bRes = false;
            break;
        }
		SkelVertVec& lst = split_it->getV_Verts();
	    for (SkelVertIt sv_it=lst.begin(); sv_it!=lst.end(); sv_it++){
		    bone_points[sv_it->B0].push_back(sv_it->O0);
        }
    }

    if (!bRes) return false;

	// create OGF

	// Saving geometry...
    Fbox rootBB;    rootBB.invalidate();

    // Header
    ogf_header 		H;
    H.format_version= xrOGF_FormatVersion;
    H.type			= MT_SKELETON_RIGID;
    H.shader_id		= 0;
    F.w_chunk		(OGF_HEADER,&H,sizeof(H));

    // Desc
    ogf_desc		desc;
    F.open_chunk	(OGF_DESC);
    desc.Save		(F);
    F.close_chunk	();
	
    // OGF_CHILDREN
    F.open_chunk	(OGF_CHILDREN);
    int chield=0;
    for (split_it=m_Splits.begin(); split_it!=m_Splits.end(); split_it++){
	    F.open_chunk(chield++);
        split_it->Save(F,FALSE);
	    F.close_chunk();
		rootBB.merge(split_it->m_Box);
    }
    F.close_chunk();


    // BBox (already computed)
    F.open_chunk(OGF_BBOX);
    F.w(&rootBB,sizeof(Fbox));
    F.close_chunk();

    // BoneNames
    F.open_chunk(OGF_BONE_NAMES);
    F.w_u32		(bone_count);
    // write root bone
    F.w_stringZ	("root");
    F.w_stringZ	("");
    Fobb obb; 	obb.invalidate();
    F.w			(&obb,sizeof(Fobb));
    // write other bones
    for (u32 bone_idx=0; bone_idx<=part_faces.size(); bone_idx++){
        F.w_stringZ	(AnsiString(bone_idx).c_str());
        F.w_stringZ	("root");
        Fobb		obb;
        ComputeOBB	(obb,bone_points[bone_idx]);
        F.w			(&obb,sizeof(Fobb));
    }
    F.close_chunk();

    F.open_chunk(OGF_IKDATA);
    for (bone_idx=0; bone_idx<=part_faces.size(); bone_idx++){
    	SBFace* face= 0;
        if (bone_idx>0){
	    	face		= (*m_Faces)[part_faces[bone_idx-1]];
		    F.w_stringZ (face->surf->_GameMtlName());
        }else{
		    F.w_stringZ ("default");
        }
        // shape
        SBoneShape	shape;
        shape.type	= SBoneShape::stBox;
        shape.flags.set(SBoneShape::sfRemoveAfterBreak);
        ComputeOBB	(shape.box,bone_points[bone_idx]);
	    F.w			(&shape,sizeof(SBoneShape));
	    F.w_u32		(jtRigid);		// joint type
        for (int k=0; k<3; k++){    // limits
            F.w_float(0.f);         // min
            F.w_float(0.f);         // max
            F.w_float(0.f);         // spring_factor
            F.w_float(0.f);         // damping_factor
        }                           
        F.w_float   (0.f);          // spring_factor
        F.w_float   (0.f);			// damping_factor
        F.w_u32     (SJointIKData::flBreakable);
        F.w_float   (0.f);			// break_force
        F.w_float   (0.f);			// break_torque

        Fvector rot={0,0,0},offs=m_Offset;
        F.w_fvector3(rot);
        offs.sub	(face?face->b:offs,m_Offset);
        F.w_fvector3(offs);         //.?
        F.w_float   (0.f);			// mass
        F.w_float   (0.f);			// center of mass        
        F.w_float   (0.f);                         
        F.w_float   (0.f);                         
    }
    F.close_chunk();
/*
    F.open_chunk(OGF_USERDATA);
    F.w(m_Source->GetClassScript().c_str(),m_Source->GetClassScript().Length());
    F.close_chunk();
*/
    return bRes;
}

IC void	append_face(SBVertVec& verts, SBFaceVec& faces, Fvector* v, const Fvector2* uvs[3], CSurface* surf)
{
	SBFace* F		= xr_new<SBFace>(surf,uvs);
	// find similar verts
	for (SBVertVecIt v_it=verts.begin(); v_it!=verts.end(); v_it++){
    	for (int k=0; k<3; k++)
	    	if ((F->vert[k]==-1)&&(*v_it)->similar(v[k])) F->vert[k]=v_it-verts.begin();
    }
    for (int k=0; k<3; k++)
        if (F->vert[k]==-1){
        	SBVert* V	= xr_new<SBVert>(); 
            V->pos.set	(v[k]);
         	F->vert[k]	= verts.size();
            verts.push_back(V);
        }
        
    // make bone
    {
        F->b.add	(v[0]);
        F->b.add	(v[1]);
        F->b.add	(v[2]);
        F->b.div	(3);
        for (int k=0; k<3; k++)
            F->o[k].sub	(verts[F->vert[k]]->pos,F->b);
        // make normal
        F->n.mknormal	(v[0],v[1],v[2]);
    }
    
    faces.push_back		(F);
}

IC bool build_mesh(const Fmatrix& parent, CEditableMesh* mesh, SBVertVec& m_verts, SBFaceVec& m_faces)
{
	bool bResult 			= true;
    // fill faces
    for (SurfFacesPairIt sp_it=mesh->GetSurfFaces().begin(); sp_it!=mesh->GetSurfFaces().end(); sp_it++){
		IntVec& face_lst 	= sp_it->second;
        CSurface* surf 		= sp_it->first;
		int gm_id			= surf->_GameMtl(); 
        if (gm_id<0){ 
        	ELog.DlgMsg		(mtError,"Surface: '%s' contains bad game material.",surf->_Name());
        	bResult 		= FALSE; 
            break; 
        }
        SGameMtl* M 		= GMLib.GetMaterialByID(gm_id);
        if (0==M){
        	ELog.DlgMsg		(mtError,"Surface: '%s' contains undefined game material.",surf->_Name());
        	bResult 		= FALSE; 
            break; 
        }
        if (!M->Flags.is(SGameMtl::flBreakable)) continue;
        
        FaceVec&	faces 	= mesh->GetFaces();
        FvectorVec&	pts 	= mesh->GetPoints();
	    for (IntIt f_it=face_lst.begin(); f_it!=face_lst.end(); f_it++){
			st_Face& face 	= faces[*f_it];
            Fvector 		v[3];
            parent.transform_tiny(v[0],pts[face.pv[0].pindex]);
            parent.transform_tiny(v[1],pts[face.pv[1].pindex]);
            parent.transform_tiny(v[2],pts[face.pv[2].pindex]);
            const Fvector2*	uv[3];
            mesh->GetFaceTC	(*f_it,uv);
            append_face		(m_verts,m_faces,v,uv,surf);
        }
        if (!bResult) break;
    }
    return bResult;
}

IC void recurse_tri(SBPart* P, SBFaceVec& faces, SBAdjVec& adjs, int id)
{
    SBFace* F			= faces[id];
	if (F->marked) 		return;

	P->append_face		(id);
    F->marked			= true;

    // recurse
    for (int k=0; k<3; k++){
	    U32Vec& PL 		= adjs[F->vert[k]];
        for (U32It pl_it=PL.begin(); pl_it!=PL.end(); pl_it++)
            recurse_tri	(P,faces,adjs,*pl_it);
    }
}
//----------------------------------------------------

bool ESceneObjectTools::ExportBreakableObjects(SExportStreams& F)
{
	bool bResult = true;
	SBVertVec 	verts;
    SBFaceVec 	faces;
    SBPartVec 	parts;
    SBAdjVec	adjs;
	// collect verts&&faces
    {
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
            CSceneObject* obj 		= dynamic_cast<CSceneObject*>(*it); VERIFY(obj);
            if (obj->IsStatic()){
                CEditableObject *O 	= obj->GetReference();
                const Fmatrix& T 	= obj->_Transform();
                for(EditMeshIt M=O->FirstMesh();M!=O->LastMesh();M++)
                    if (!build_mesh	(T,*M,verts,faces)){bResult=false;break;}
            }
        }
    }
    // make adjacement
    if (bResult){
    	adjs.resize	(verts.size());
		for (SBFaceVecIt f_it=faces.begin(); f_it!=faces.end(); f_it++)
        	for (int k=0; k<3; k++) adjs[(*f_it)->vert[k]].push_back(f_it-faces.begin());
    }
    // extract parts
    if (bResult){
		for (SBFaceVecIt f_it=faces.begin(); f_it!=faces.end(); f_it++){
        	SBFace* F	= *f_it;
            if (!F->marked){
		    	SBPart* P 		= xr_new<SBPart>(&verts,&faces);
                int	id			= f_it-faces.begin();
			    recurse_tri		(P,faces,adjs,id);
		    	parts.push_back	(P);
            }
        }
    }
    // export parts
    if (bResult){
        for (SBPartVecIt p_it=parts.begin(); p_it!=parts.end(); p_it++){	
        	SBPart*	P			= *p_it;
            // export visual
            AnsiString sn		= AnsiString().sprintf("meshes\\obj_%d.ogf",(p_it-parts.begin()));
            AnsiString fn		= Scene.LevelPath()+sn;
            IWriter* W			= FS.w_open(fn.c_str()); VERIFY(W);
            if (!P->Export(*W)){
            	ELog.DlgMsg		(mtError,"Invalid breakable object.");
            	bResult 		= false;
                break;
            }
            FS.w_close			(W);
            // export spawn object
            {
            	AnsiString entity_ref		= "breakable_object";
				CSE_ALifeBreakable*	m_Data	= dynamic_cast<CSE_ALifeBreakable*>(F_entity_Create(entity_ref.c_str())); VERIFY(m_Data);
                // set params
                strcpy	  					(m_Data->s_name,entity_ref.c_str());
                strcpy	  					(m_Data->s_name_replace,sn.c_str());
                m_Data->o_Position.set		(P->m_Offset);
                m_Data->o_Angle.set			(0,0,0);
                m_Data->set_visual			(sn.c_str(),false);

                NET_Packet					Packet;
                m_Data->Spawn_Write			(Packet,TRUE);

                F.spawn.stream.open_chunk	(F.spawn.chunk++);
                F.spawn.stream.w			(Packet.B.data,Packet.B.count);
                F.spawn.stream.close_chunk	();
            }
        }
    }
    // clean up
    {
        for (SBVertVecIt v_it=verts.begin(); v_it!=verts.end(); v_it++) xr_delete(*v_it);
        for (SBFaceVecIt f_it=faces.begin(); f_it!=faces.end(); f_it++) xr_delete(*f_it);
        for (SBPartVecIt p_it=parts.begin(); p_it!=parts.end(); p_it++) xr_delete(*p_it);
    }
    
    return bResult;
}
//----------------------------------------------------


