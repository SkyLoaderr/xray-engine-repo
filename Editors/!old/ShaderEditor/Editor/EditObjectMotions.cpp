//----------------------------------------------------
// file: CEditableObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"

#ifdef _EDITOR
	#include "ui_main.h"
#endif

#include "motion.h"
#include "bone.h"
#include "EditMesh.h"

//----------------------------------------------------
class fBoneNameEQ {
	shared_str	name;
public:
	fBoneNameEQ(shared_str N) : name(N) {};
	IC bool operator() (CBone* B) { return (xr_strcmp(B->Name(),name)==0); }
};
class fBoneWMNameEQ {
	shared_str	wm_name;
public:
	fBoneWMNameEQ(shared_str N) : wm_name(N) {};
	IC bool operator() (CBone* B) { return (xr_strcmp(B->WMap(),wm_name)==0); }
};
//----------------------------------------------------
#ifdef _EDITOR
void CEditableObject::OnFrame()
{
	if (IsSkeleton()){
		BoneVec& lst = m_Bones;
    	if (IsSMotionActive()){
            Fvector R,T;
            int i=0;
            for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++, i++){
	            m_ActiveSMotion->_Evaluate(i,m_SMParam.Frame(),T,R);
                (*b_it)->_Update(T,R);
            }
            m_SMParam.Update(Device.fTimeDelta,m_ActiveSMotion->fSpeed,!m_ActiveSMotion->m_Flags.is(esmStopAtEnd));
        }else{
		    //for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++) (*b_it)->Reset();
        }
        CalculateAnimation(m_ActiveSMotion);
    }
}
#endif

void CEditableObject::OnBindTransformChange()
{
    for(EditMeshIt mesh_it=FirstMesh();mesh_it!=LastMesh();mesh_it++){
        CEditableMesh* MESH = *mesh_it;
        MESH->GenerateSVertices();
    }
}

void CEditableObject::GotoBindPose()
{
    BoneVec& lst = m_Bones;
    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++) (*b_it)->Reset();
    CalculateAnimation(0);
#ifdef _EDITOR
    UI->RedrawScene();
#endif
}

CSMotion* CEditableObject::ResetSAnimation(bool bGotoBindPose)
{
	CSMotion* M=m_ActiveSMotion;
	SetActiveSMotion(0);
    if (bGotoBindPose) GotoBindPose();
    return M;
}

//----------------------------------------------------
// Skeletal motion
//----------------------------------------------------
static void CalculateAnim(CBone* bone, CSMotion* motion, Fmatrix& parent)
{
    Flags8 flags; flags.zero();
    if (motion) flags 	= motion->GetMotionFlags(bone->SelfID);

    Fmatrix& M 		= bone->_MTransform();
    Fmatrix& L 		= bone->_LTransform();
    
    const Fvector& r = bone->_Rotate();
    if (flags.is(st_BoneMotion::flWorldOrient)){
        M.setXYZi	(r.x,r.y,r.z);
        M.c.set		(bone->_Offset());
        L.mul		(parent,M);
        L.i.set		(M.i);
        L.j.set		(M.j);
        L.k.set		(M.k);

        Fmatrix		 LI; LI.invert(parent);
        M.mulA		(LI);
    }else{
        M.setXYZi	(r.x,r.y,r.z);
        M.c.set		(bone->_Offset());
        L.mul		(parent,M);
    }
	bone->_RenderTransform().mul_43(bone->_LTransform(),bone->_RITransform());
    
    // Calculate children
    for (BoneIt b_it=bone->children.begin(); b_it!=bone->children.end(); b_it++)
    	CalculateAnim	(*b_it,motion,bone->_LTransform());
}
static void CalculateRest(CBone* bone, Fmatrix& parent)
{
    Fmatrix& R	= bone->_RTransform();
    R.setXYZi	(bone->_RestRotate());
    R.c.set		(bone->_RestOffset());
    R.mulA_43	(parent);
    bone->_RITransform().invert(bone->_RTransform());

    // Calculate children
    for (BoneIt b_it=bone->children.begin(); b_it!=bone->children.end(); b_it++)
    	CalculateRest	(*b_it,bone->_RTransform());
}

void CEditableObject::CalculateAnimation(CSMotion* motion)
{
	CalculateAnim(m_Bones.front(),motion,Fidentity);
}

void CEditableObject::CalculateBindPose()
{
	CalculateRest(m_Bones.front(),Fidentity);
}

void CEditableObject::SetActiveSMotion(CSMotion* mot)
{
	m_ActiveSMotion=mot;
	if (m_ActiveSMotion) m_SMParam.Set(m_ActiveSMotion);
}

bool CEditableObject::RemoveSMotion(const char* name)
{
    SMotionVec& lst = m_SMotions;
    for(SMotionIt m=lst.begin(); m!=lst.end(); m++)
        if ((stricmp((*m)->Name(),name)==0)){
        	if (m_ActiveSMotion==*m) SetActiveSMotion(0);
            xr_delete(*m);
        	lst.erase(m);
            return true;
        }
    return false;
}                         

//---------------------------------------------------------------------------
/*
bool CEditableObject::LoadSMotions(const char* fname)
{
	IReader* F	= FS.r_open(fname);
    ClearSMotions();
    // object motions
    m_SMotions.resize(F->r_u32());
	SetActiveSMotion(0);
    for (SMotionIt m_it=m_SMotions.begin(); m_it!=m_SMotions.end(); m_it++){
        *m_it = xr_new<CSMotion>();
        if (!(*m_it)->Load(*F)){
            ELog.DlgMsg(mtError,"Motions has different version. Load failed.");
            xr_delete(*m_it);
            m_SMotions.clear();
            FS.r_close(F);
            return false;
        }
	  	if (!CheckBoneCompliance(*m_it)){
        	ClearSMotions();
            ELog.DlgMsg(mtError,"Load failed.",fname);
            xr_delete(&*m_it);
            FS.r_close(F);
            return false;
        }
    }
	FS.r_close(F);
	return true;
}
*/
bool CEditableObject::AppendSMotion(LPCSTR fname, SMotionVec* inserted)
{
	VERIFY(IsSkeleton());

    bool bRes	= true;
    
	LPCSTR ext	= strext(fname);
    if (0==stricmp(ext,".skl")){
        CSMotion* M = xr_new<CSMotion>();
        if (!M->LoadMotion(fname)){
            ELog.Msg(mtError,"Motion '%s' can't load. Append failed.",fname);
            xr_delete(M);
            bRes = false;
        }else{
        	string256 name;
			_splitpath(fname,0,0,name,0);
            if (CheckBoneCompliance(M)){
                M->SortBonesBySkeleton(m_Bones);
                string256 			m_name;
                GenerateSMotionName	(m_name,name,M);
                M->SetName			(m_name);
                m_SMotions.push_back(M);
                if (inserted)		inserted->push_back(M);
                // optimize
                M->Optimize			();
            }else{
                ELog.Msg(mtError,"Append failed.",fname);
                xr_delete(M);
	            bRes = false;
            }
        }
    }else if (0==stricmp(ext,".skls")){
        IReader* F	= FS.r_open(fname);
        if (!F){ 
        	ELog.Msg(mtError,"Can't open file '%s'.",fname);
            bRes = false;
    	}
        if (bRes){
            // object motions
            int cnt 	= F->r_u32();
            for (int k=0; k<cnt; k++){
                CSMotion* M	= xr_new<CSMotion>();
                if (!M->Load(*F)){
                    ELog.Msg(mtError,"Motion '%s' has different version. Load failed.",M->Name());
                    xr_delete(M);
                    bRes = false;
                    break;
                }
                if (!CheckBoneCompliance(M)){
                    xr_delete(M);
                    bRes = false;
                    break;
                }
                if (bRes){
                    M->SortBonesBySkeleton(m_Bones);
                    string256 			m_name;
                    GenerateSMotionName	(m_name,M->Name(),M);
                    M->SetName			(m_name);
                    m_SMotions.push_back(M);
                    if (inserted)		inserted->push_back(M);
                    // optimize
                    M->Optimize			();
                }
            }
        }
        FS.r_close(F);
    }
    return bRes;
}

void CEditableObject::ClearSMotions()
{
	SetActiveSMotion(0);
    for(SMotionIt m_it=m_SMotions.begin(); m_it!=m_SMotions.end();m_it++)xr_delete(*m_it);
    m_SMotions.clear();
}

bool CEditableObject::SaveSMotions(const char* fname)
{
	CMemoryWriter F;
    F.w_u32		(m_SMotions.size());
    for (SMotionIt m_it=m_SMotions.begin(); m_it!=m_SMotions.end(); m_it++) (*m_it)->Save(F);
    F.save_to		(fname);
    return true;
}

bool CEditableObject::RenameSMotion(const char* old_name, const char* new_name)
{
	if (stricmp(old_name,new_name)==0) return true;
    if (FindSMotionByName(new_name)) return false;
	CSMotion* M = FindSMotionByName(old_name); VERIFY(M);
    M->SetName(new_name);
    return true;
}

CSMotion* CEditableObject::FindSMotionByName	(const char* name, const CSMotion* Ignore)
{
	if (name&&name[0]){
        SMotionVec& lst = m_SMotions;
        for(SMotionIt m=lst.begin(); m!=lst.end(); m++)
            if ((Ignore!=(*m))&&(stricmp((*m)->Name(),name)==0)) return (*m);
    }
    return 0;
}

void CEditableObject::GenerateSMotionName(char* buffer, const char* start_name, const CSMotion* M)
{
	strcpy(buffer,start_name);
    int idx = 0;
	while(FindSMotionByName(buffer,M)){
		sprintf( buffer, "%s_%2d", start_name, idx );
    	idx++;
    }
    strlwr(buffer);
}

ICF bool pred_sort_B(CBone* A, CBone* B)	
{
	return (xr_strcmp(A->Name().c_str(),B->Name().c_str())<0);
}
ICF void fill_bones_by_parent(BoneVec& bones,CBone* start)
{
    bones.push_back		(start);
    for (BoneIt b_it=start->children.begin(); b_it!=start->children.end(); b_it++)
    	fill_bones_by_parent(bones,*b_it);
}
void CEditableObject::PrepareBones()
{
	CBone* PARENT		= 0;
    // clear empty parent
    for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
        BoneIt parent	= std::find_if(m_Bones.begin(),m_Bones.end(),fBoneNameEQ((*b_it)->ParentName()));
        if (parent==m_Bones.end()){ 
        	(*b_it)->SetParentName("");
            VERIFY2		(0==PARENT,"Invalid object. Have more than 1 parent.");
            PARENT		= *b_it;
        }else{
            BoneIt parent	= std::find_if(m_Bones.begin(),m_Bones.end(),fBoneNameEQ((*b_it)->ParentName()));
            (*b_it)->parent	= (parent==m_Bones.end())?0:*parent;
        }
    }
    // sort by name
    std::sort(m_Bones.begin(),m_Bones.end(),pred_sort_B);
    // fill children
    for (b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
        BoneIt parent	= std::find_if(m_Bones.begin(),m_Bones.end(),fBoneNameEQ((*b_it)->ParentName()));
        if (parent!=m_Bones.end()) (*parent)->children.push_back(*b_it);
    }
    // manual sort
    u32 b_cnt			= m_Bones.size();
    m_Bones.clear		();
    fill_bones_by_parent(m_Bones,PARENT);
    VERIFY				(b_cnt==m_Bones.size());
    // update SelfID
    for (b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
        (*b_it)->SelfID		= b_it-m_Bones.begin();
    VERIFY(0==m_Bones.front()->parent);
/*    
    for (b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
    	Msg("%20s - %20s",(*b_it)->Name().c_str(),(*b_it)->ParentName().c_str());
*/
    CalculateBindPose		();
}

BoneIt CEditableObject::FindBoneByNameIt(const char* name)
{
	return std::find_if(m_Bones.begin(),m_Bones.end(),fBoneNameEQ(name));
}

int CEditableObject::FindBoneByNameIdx(LPCSTR name)
{
	BoneIt b_it = FindBoneByNameIt(name);
    return (b_it==m_Bones.end())?-1:b_it-m_Bones.begin();
}

CBone* CEditableObject::FindBoneByName(const char* name)
{
	BoneIt b_it = FindBoneByNameIt(name);
    return (b_it==m_Bones.end())?0:*b_it;
}

int CEditableObject::GetRootBoneID()
{
    for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
    	if ((*b_it)->IsRoot()) return b_it-m_Bones.begin();
    THROW;
    return -1;
}

int CEditableObject::PartIDByName(LPCSTR name)
{
	for (BPIt it=m_BoneParts.begin(); it!=m_BoneParts.end(); it++)
    	if (it->alias==name) return it-m_BoneParts.begin();
    return -1;
} 

shared_str CEditableObject::BoneNameByID(int id)
{
	VERIFY((id>=0)&&(id<(int)m_Bones.size()));
    return m_Bones[id]->Name();
}

int	CEditableObject::GetBoneIndexByWMap(const char* wm_name)
{
	BoneIt bone = std::find_if(m_Bones.begin(),m_Bones.end(),fBoneWMNameEQ(wm_name));
    return (bone==m_Bones.end())?-1:bone-m_Bones.begin();
}

void CEditableObject::GetBoneWorldTransform(u32 bone_idx, float t, CSMotion* motion, Fmatrix& matrix)
{
	VERIFY(bone_idx<m_Bones.size());
    int idx	= bone_idx;
    matrix.identity();
    IntVec lst;
    do{ lst.push_back(idx); }while((idx=m_Bones[idx]->Parent()?m_Bones[idx]->Parent()->SelfID:-1)>-1);
    for (int i=lst.size()-1; i>=0; i--){
    	idx = lst[i];
	    Flags8 flags	= motion->GetMotionFlags(idx);
    	Fvector T,R;
        Fmatrix rot, mat;
        motion->_Evaluate(idx,t,T,R);
        if (flags.is(st_BoneMotion::flWorldOrient)){
            rot.setXYZi(R.x,R.y,R.z);
            mat.identity();
            mat.c.set(T);
            mat.mulA(matrix);
            mat.i.set(rot.i);
            mat.j.set(rot.j);
            mat.k.set(rot.k);
        }else{
            mat.setXYZi(R.x,R.y,R.z);
            mat.c.set(T);
            mat.mulA(matrix);
        }
        matrix.set(mat);
    }
}

bool CEditableObject::CheckBoneCompliance(CSMotion* M)
{
	VERIFY(M);
    BoneMotionVec& lst = M->BoneMotions();
/*
	if (m_Bones.size()!=lst.size()){
		Log		("!Different bone count.\nObject has '%d' bones. Motion has '%d' bones.",m_Bones.size(),lst.size());
    	return false;
    }
    for(BoneMotionIt bm_it=lst.begin(); bm_it!=lst.end(); bm_it++)
    	if (!FindBoneByName(*bm_it->name)){
        	Log		("!Can't find bone '%s' in object.",bm_it->name);
        	return false;
        }
*/
    for(BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
    	if (!M->FindBoneMotion((*b_it)->Name())){
        	Log		("!Can't find bone '%s' in motion.",*(*b_it)->Name());
        	return false;
        }
    return true;
}

void CEditableObject::OptimizeSMotions()
{
#ifdef _EDITOR
	SPBItem* pb				= UI->ProgressStart(m_SMotions.size(),"Motions optimizing...");
#endif
	for (SMotionIt s_it=m_SMotions.begin(); s_it!=m_SMotions.end(); s_it++){
        (*s_it)->Optimize	();
#ifdef _EDITOR
		pb->Inc				();
#endif
	}
#ifdef _EDITOR
    UI->ProgressEnd				(pb);
#endif
}
