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
#include "ui_main.h"

//----------------------------------------------------
struct SBVert{
	Fvector			pos;
public:
    bool			similar(const Fvector& v){return pos.similar(v,EPS_L);}
};
struct SBFace;
DEFINE_VECTOR		(SBVert*,SBVertVec,SBVertVecIt);
DEFINE_VECTOR		(SBFace*,SBFaceVec,SBFaceVecIt);
struct SBFace{
// prepare
    int 			vert_id[3];
    bool			marked;
    SBFaceVec		adjs;
// geom
	Fvector			p[3];
    Fvector			o[3];
    Fvector2		uv[3];
    Fvector			n;
    int				bone_id;
    CSurface*		surf;
public:		
					SBFace	(CSurface* _surf, const Fvector2* _uv[3]):surf(_surf),marked(false),bone_id(-1)
	{
    	vert_id[0]	= -1;
    	vert_id[1]	= -1;
    	vert_id[2]	= -1;
        uv[0]		= *_uv[0];
        uv[1]		= *_uv[1];
        uv[2]		= *_uv[2];
        n.set		(0,0,0);
    }
    float			CalcArea()
    {
    	Fvector V0,V1;
        V0.sub		(p[1],p[0]);
        V1.sub		(p[2],p[0]);
        float sqm0	= V0.square_magnitude();
        float sqm1	= V1.square_magnitude();
        return		0.5f*_sqrt(sqm0*sqm1-_sqr(V0.dotproduct(V1)));
    }
};

struct SBBone
{
	AnsiString		mtl;
	AnsiString		name;
	AnsiString		parent;
	Fvector			offset;
    u32				f_cnt;
    float			area;
				    SBBone				(AnsiString _nm, AnsiString _parent, AnsiString _mtl, u32 _f_cnt, float _area)
                    					:name(_nm),parent(_parent),mtl(_mtl),f_cnt(_f_cnt),area(_area)
    {
    	offset.set	(0,0,0);
    }
};
DEFINE_VECTOR		(SBBone,SBBoneVec,SBBoneVecIt);
DEFINE_VECTOR		(SBFaceVec,SBAdjVec,SBAdjVecIt);

struct SBPart: public CExportSkeletonCustom
{
	SBFaceVec		m_Faces;
    SBBoneVec		m_Bones;

    Fbox			m_BBox;
    Fobb			m_OBB;

    SBPart*			m_Reference;
    
    Fvector			m_RefOffset;
    Fvector			m_RefRotate;
public:
					SBPart				(){m_Reference=0;}
    virtual bool 	Export				(IWriter& F);
	void			append_face			(SBFace* F)
    {
    	m_Faces.push_back				(F);
    }
    void			use_face			(SBFace* F, u32& cnt, u32 bone_id, float& area)
    {
    	VERIFY							(F->bone_id==-1);
        F->marked						= true;
        F->bone_id						= bone_id;
        area							+= F->CalcArea();
        cnt++;
    }
    void			recurse_fragment	(SBFace* F, u32& cnt, u32 bone_id, u32 max_faces, float& area)
    {
    	if (F){
        	if (!F->marked)	use_face	(F,cnt,bone_id,area);
            // fill nearest
            SBFaceVec r_vec;
            for (SBFaceVecIt n_it=F->adjs.begin(); n_it!=F->adjs.end(); n_it++){
                if (cnt>=max_faces)		break;
                if ((*n_it)->marked)	continue;
	        	use_face				(*n_it,cnt,bone_id,area);
                r_vec.push_back			(*n_it);
            }     
            // recurse adjs   	
            for (SBFaceVecIt a_it=r_vec.begin(); a_it!=r_vec.end(); a_it++){
                if (cnt>=max_faces)				break;
                if ((*a_it)->bone_id!=bone_id)	continue;
                recurse_fragment				(*a_it,cnt,bone_id,max_faces,area);
            } 
        }
    }
    void			prepare				(SBAdjVec& adjs)
    {
        // compute OBB
        FvectorVec pts; pts.reserve		(m_Faces.size()*3);
		for (SBFaceVecIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
        	(*f_it)->marked				= false;
        	for (int k=0; k<3; k++)		pts.push_back((*f_it)->p[k]);
        }
        ComputeOBB						(m_OBB,pts);
        // fill adjacent
		for (SBFaceVecIt a_it=m_Faces.begin(); a_it!=m_Faces.end(); a_it++){
        	SBFace* A					= *a_it;
            for (int k=0; k<3; k++){
            	SBFaceVec& b_vec		= adjs[A->vert_id[k]];
                for (SBFaceVecIt b_it=b_vec.begin(); b_it!=b_vec.end(); b_it++){
                    SBFace* B			= *b_it;
                    if (A!=B){
                        int cnt			= 0;
                        for (int a=0; a<3; a++) for (int b=0; b<3; b++) if (A->vert_id[a]==B->vert_id[b]) cnt++;
                        if (cnt>=2){
                            if (std::find(A->adjs.begin(),A->adjs.end(),B)==A->adjs.end()) A->adjs.push_back(B);
                            if (std::find(B->adjs.begin(),B->adjs.end(),A)==B->adjs.end()) B->adjs.push_back(A);
                        }
                    }
                }        
            }
        }
        // prepare transform matrix
    	m_BBox.invalidate				();
		Fmatrix M; M.set				(m_OBB.m_rotate.i,m_OBB.m_rotate.j,m_OBB.m_rotate.k,m_OBB.m_translate);
        m_RefOffset.set					(m_OBB.m_translate);
        M.getXYZ						(m_RefRotate); // не i потому что в движке так
        M.invert						();
        // transform vertices & calculate bounding box
		for (f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
        	SBFace* F					= (*f_it);
        	for (int k=0; k<3; k++){ 
            	M.transform_tiny		(F->p[k]);
                m_BBox.modify			(F->p[k]);
            }
            if (F->adjs.empty())		ELog.Msg(mtError,"Error"); //.
        }   
        // calculate bone params
        int bone_face_min				= 2;
        int bone_cnt_calc				= iFloor(float(m_Faces.size())/bone_face_min);
        int bone_cnt_max				= (bone_cnt_calc<62)?bone_cnt_calc:62;
        int bone_face_max				= iFloor(float(m_Faces.size())/bone_cnt_max+0.5f); bone_face_max *= 4.f;
        int bone_idx					= 0;
        // create big fragment
        u32 face_accum_total			= 0;
        AnsiString parent_bone			= "";
        do{
        	SBFace* F					= 0;
        	// find unused face
            for (SBFaceVecIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
            	if (!(*f_it)->marked){
	            	F					= *f_it;
                	int cnt 			= 0;
		            for (SBFaceVecIt a_it=F->adjs.begin(); a_it!=F->adjs.end(); a_it++) cnt+=(*a_it)->marked?0:1;
                    if ((cnt==0)||(cnt>=2))	break;
                }
            }
            if (!F)						break;
            float area					= 0;
	        u32 face_accum				= 0;
            u32 face_max_count 			= Random.randI(bone_face_min,bone_face_max+1);
            // fill faces
            recurse_fragment			(F,face_accum,bone_idx,face_max_count,area);
            if (face_accum==1){
//            	F->marked				= false;
                F->bone_id				= -1;
            }else{
                m_Bones.push_back		(SBBone(bone_idx,parent_bone,F->surf->_GameMtlName(),face_accum,area));
                parent_bone				= "0";
                bone_idx				++;
                face_accum_total		+= face_accum;
            }
            // create bone
        }while(bone_idx<bone_cnt_max);
        
		// attach small single face to big fragment
        while (face_accum_total<m_Faces.size()){
            for (SBFaceVecIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
            	SBFace* F				= *f_it;
            	if (-1==F->bone_id){
	            	SBFace* P			= 0;
		            for (SBFaceVecIt a_it=F->adjs.begin(); a_it!=F->adjs.end(); a_it++){ 
                    	P				= *a_it;
                    	if (-1!=P->bone_id)	break;
                    }
                    if (P){
                        F->marked		= true;
                        F->bone_id		= P->bone_id;
                        face_accum_total++;
                    }
            	}
            } 
        }
        
        // calculate bone offset
        for (f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
            SBFace* F					= *f_it;
            SBBone& B					= m_Bones[F->bone_id];
            for (int k=0; k<3; k++)		B.offset.add(F->p[k]);
        }
        for (SBBoneVecIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
            SBBone& B					= *b_it;
            VERIFY						(0!=B.f_cnt);
            B.offset.div				(B.f_cnt*3);
        }
        Fvector& offs					= m_Bones.front().offset;
        for (b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
            b_it->offset.sub			(offs);
        
		// calculate vertices offset
		for (f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
        	SBFace* F					= (*f_it);	VERIFY(F->bone_id>=0);	
            SBBone& B					= m_Bones	[F->bone_id]; 
        	for (int k=0; k<3; k++)		F->o[k].sub	(F->p[k],B.offset);
            F->n.mknormal				(F->o[0],F->o[1],F->o[2]);
        }
    }
};
DEFINE_VECTOR		(SBPart*,SBPartVec,SBPartVecIt);
//----------------------------------------------------
bool SBPart::Export	(IWriter& F)
{
	VERIFY			(!m_Bones.empty());
    if (m_Bones.size()>63){
    	ELog.Msg(mtError,"Breakable object cannot handle more than 63 parts.");
     	return false;
    }

    bool bRes = true;

    u32 bone_count			= m_Bones.size();
                    
    xr_vector<FvectorVec>	bone_points;
	bone_points.resize		(bone_count);

    u32 mtl_cnt				= 0;
                                  
    for (SBFaceVecIt pf_it=m_Faces.begin(); pf_it!=m_Faces.end(); pf_it++){
    	SBFace* face		= *pf_it;
        int mtl_idx			= FindSplit(face->surf->_ShaderName(),face->surf->_Texture());
        if (mtl_idx<0){
            m_Splits.push_back(SSplit(face->surf,m_BBox));
            mtl_idx	= mtl_cnt++;
        }
        SSplit& split=m_Splits[mtl_idx];
        SSkelVert v[3];
        for (int k=0; k<3; k++){
            v[k].set	(face->p[k],face->uv[k],1.f);
            v[k].set0	(face->o[k],face->n,face->bone_id); //. нужно вз€ть нормаль дл€ вертекса
        }
        split.add_face		(v[0], v[1], v[2]);
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
    // write other bones
    for (u32 bone_idx=0; bone_idx<bone_count; bone_idx++){
    	SBBone& bone=m_Bones[bone_idx];
        F.w_stringZ	(bone.name.c_str());
        F.w_stringZ	(bone.parent.c_str());
        Fobb		obb;
        ComputeOBB	(obb,bone_points[bone_idx]);
        F.w			(&obb,sizeof(Fobb));
    }
    F.close_chunk();

    F.open_chunk(OGF_IKDATA);
    for (bone_idx=0; bone_idx<bone_count; bone_idx++){
    	SBBone& bone=m_Bones[bone_idx];

        // material
        F.w_stringZ (bone.mtl.c_str());
        // shape
        SBoneShape	shape;
        shape.type	= SBoneShape::stBox;
        shape.flags.set(SBoneShape::sfRemoveAfterBreak);
        ComputeOBB	(shape.box,bone_points[bone_idx]);
	    F.w			(&shape,sizeof(SBoneShape));
	    F.w_u32		(jtNone);		// joint type
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

        Fvector rot={0,0,0};
        F.w_fvector3(rot);
        F.w_fvector3(bone.offset);
        F.w_float   (bone.area);	// mass (дл€  ости посчитал площадь)
        F.w_fvector3(shape.box.m_translate);	// center of mass        
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
	    	if ((F->vert_id[k]==-1)&&(*v_it)->similar(v[k])) F->vert_id[k]=v_it-verts.begin();
    }
    for (int k=0; k<3; k++){
	    F->p[k].set			(v[k]);
        if (F->vert_id[k]==-1){
        	SBVert* V		= xr_new<SBVert>(); 
            V->pos.set		(v[k]);
         	F->vert_id[k]	= verts.size();
            verts.push_back	(V);
        }
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

IC void recurse_tri(SBPart* P, SBFaceVec& faces, SBAdjVec& adjs, SBFace* F)
{
	if (F->marked) 		return;

	P->append_face		(F);
    F->marked			= true;

    // recurse
    for (int k=0; k<3; k++){
	    SBFaceVec& PL 	= adjs[F->vert_id[k]];
        for (SBFaceVecIt pl_it=PL.begin(); pl_it!=PL.end(); pl_it++)
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
	    UI->ProgressStart(m_Objects.size(),"Prepare geometry...");
        for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
            UI->ProgressInc			();
            CSceneObject* obj 		= dynamic_cast<CSceneObject*>(*it); VERIFY(obj);
            if (obj->IsStatic()){
                CEditableObject *O 	= obj->GetReference();
                const Fmatrix& T 	= obj->_Transform();
                for(EditMeshIt M=O->FirstMesh();M!=O->LastMesh();M++)
                    if (!build_mesh	(T,*M,verts,faces)){bResult=false;break;}
            }
        }
	    UI->ProgressEnd();
    }
    // make adjacement
    if (bResult){
    	adjs.resize	(verts.size());
		for (SBFaceVecIt f_it=faces.begin(); f_it!=faces.end(); f_it++)
        	for (int k=0; k<3; k++) adjs[(*f_it)->vert_id[k]].push_back(*f_it);
    }
    // extract parts
    if (bResult){
	    UI->ProgressStart(faces.size(),"Extract Parts...");
		for (SBFaceVecIt f_it=faces.begin(); f_it!=faces.end(); f_it++){
            UI->ProgressInc		();
        	SBFace* F	= *f_it;
            if (!F->marked){
		    	SBPart* P 		= xr_new<SBPart>();
			    recurse_tri		(P,faces,adjs,*f_it);
		    	parts.push_back	(P);
            }
        }
	    UI->ProgressEnd();
    }
    // simplify parts
    if (bResult){
	    UI->ProgressStart(parts.size(),"Simplify Parts...");
        for (SBPartVecIt p_it=parts.begin(); p_it!=parts.end(); p_it++){	
            UI->ProgressInc		();
        	(*p_it)->prepare	(adjs);
        }
	    UI->ProgressEnd();
    }
    // export parts
    if (bResult){
	    UI->ProgressStart(parts.size(),"Export Parts...");
        for (SBPartVecIt p_it=parts.begin(); p_it!=parts.end(); p_it++){	
            UI->ProgressInc		();
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
				CSE_ALifeObjectBreakable*	m_Data	= dynamic_cast<CSE_ALifeObjectBreakable*>(F_entity_Create(entity_ref.c_str())); VERIFY(m_Data);
                // set params
                strcpy	  					(m_Data->s_name,entity_ref.c_str());
                strcpy	  					(m_Data->s_name_replace,sn.c_str());
                m_Data->o_Position.set		(P->m_RefOffset); 
                m_Data->o_Angle.set			(P->m_RefRotate);
                m_Data->set_visual			(sn.c_str(),false);
                m_Data->m_health			= 100.f;

                NET_Packet					Packet;
                m_Data->Spawn_Write			(Packet,TRUE);

                F.spawn.stream.open_chunk	(F.spawn.chunk++);
                F.spawn.stream.w			(Packet.B.data,Packet.B.count);
                F.spawn.stream.close_chunk	();
            }
        }
	    UI->ProgressEnd();
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


