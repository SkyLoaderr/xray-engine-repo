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
	AnsiString	name;
public:
	fBoneNameEQ(const char *N) : name(N) {};
	IC bool operator() (CBone* B) { return (stricmp(B->Name(),name.c_str())==0); }
};
class fBoneWMNameEQ {
	AnsiString	wm_name;
public:
	fBoneWMNameEQ(const char *N) : wm_name(N) {};
	IC bool operator() (CBone* B) { return (stricmp(B->WMap(),wm_name.c_str())==0); }
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
static void CalculateAnim(CBone* bone, CSMotion* motion)
{
    Flags8 flags; flags.zero();
    if (motion) flags = motion->GetMotionFlags(bone->index);
    
    CBone* parent_bone = bone->Parent();

    if (parent_bone&&!parent_bone->flags.is(CBone::flCalculate))
	    CalculateAnim(parent_bone,motion); 

    Fmatrix& M 		= bone->_MTransform();
    Fmatrix& L 		= bone->_LTransform();
    
    const Fvector& r = bone->_Rotate();
    if (flags.is(st_BoneMotion::flWorldOrient)){
        M.setXYZi	(r.x,r.y,r.z);
        M.c.set		(bone->_Offset());
        L.mul		(parent_bone?parent_bone->_LTransform():Fidentity,M);
        L.i.set		(M.i);
        L.j.set		(M.j);
        L.k.set		(M.k);
		if (parent_bone){ 
            Fmatrix LI; LI.invert(parent_bone->_LTransform());
        	M.mulA	(LI);
        }
    }else{
        M.setXYZi	(r.x,r.y,r.z);
        M.c.set		(bone->_Offset());
        L.mul		(parent_bone?parent_bone->_LTransform():Fidentity,M);
    }
	bone->_RenderTransform().mul_43(bone->_LTransform(),bone->_RITransform());

    bone->flags.set(CBone::flCalculate,TRUE);
}
static void CalculateRest(CBone* bone)
{
    CBone* parent_bone = bone->Parent();

    if (parent_bone&&!parent_bone->flags.is(CBone::flCalculate))
	    CalculateRest(parent_bone); 

    Fmatrix& R	= bone->_RTransform();
    R.setXYZi	(bone->_RestRotate());
    R.c.set		(bone->_RestOffset());
    if (parent_bone) R.mulA_43(parent_bone->_RTransform());
    bone->_RITransform().invert(bone->_RTransform());

    bone->flags.set(CBone::flCalculate,TRUE);
}

void CEditableObject::CalculateAnimation(CBone* bone, CSMotion* motion)
{
    for(BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
		(*b_it)->flags.set(CBone::flCalculate,FALSE);
	CalculateAnim(bone,motion);
}

void CEditableObject::CalculateAnimation(CSMotion* motion)
{
    for(BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
		(*b_it)->flags.set(CBone::flCalculate,FALSE);
    for(b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
		if (!(*b_it)->flags.is(CBone::flCalculate)) CalculateAnim(*b_it,motion);
}

void CEditableObject::CalculateBindPose()
{
    for(BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
		(*b_it)->flags.set(CBone::flCalculate,FALSE);
    for(b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
		if (!(*b_it)->flags.is(CBone::flCalculate)) CalculateRest(*b_it);
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
            ELog.DlgMsg(mtError,"Motion '%s' can't load. Append failed.",fname);
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
                ELog.DlgMsg(mtError,"Append failed.",fname);
                xr_delete(M);
	            bRes = false;
            }
        }
    }else if (0==stricmp(ext,".skls")){
        IReader* F	= FS.r_open(fname);
        if (!F){ 
        	ELog.DlgMsg(mtError,"Can't open file '%s'.",fname);
            bRes = false;
    	}
        if (bRes){
            // object motions
            int cnt 	= F->r_u32();
            for (int k=0; k<cnt; k++){
                CSMotion* M	= xr_new<CSMotion>();
                if (!M->Load(*F)){
                    ELog.DlgMsg(mtError,"Motion '%s' has different version. Load failed.",M->Name());
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

void CEditableObject::PrepareBones()
{
    // update parenting
    for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
        (*b_it)->index 		= b_it-m_Bones.begin();
        BoneIt parent		= std::find_if(m_Bones.begin(),m_Bones.end(),fBoneNameEQ((*b_it)->ParentName()));
        (*b_it)->parent 	= (parent==m_Bones.end())?0:*parent;
    }
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

LPCSTR CEditableObject::BoneNameByID(int id)
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
    do{ lst.push_back(idx); }while((idx=m_Bones[idx]->Parent()?m_Bones[idx]->Parent()->index:-1)>-1);
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
        	Log		("!Can't find bone '%s' in motion.",(*b_it)->Name());
        	return false;
        }
    return true;
}

void CEditableObject::OptimizeSMotions()
{
#ifdef _EDITOR
	UI->ProgressStart		(m_SMotions.size(),"Motions optimizing...");
#endif
	for (SMotionIt s_it=m_SMotions.begin(); s_it!=m_SMotions.end(); s_it++){
        (*s_it)->Optimize	();
#ifdef _EDITOR
	    UI->ProgressInc		();
#endif
	}
#ifdef _EDITOR
    UI->ProgressEnd			();
#endif
}
