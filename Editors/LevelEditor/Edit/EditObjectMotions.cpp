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
void st_AnimParam::Set(CCustomMotion* M)
{
	t=0;
    min_t=(float)M->FrameStart()/M->FPS();
    max_t=(float)M->FrameEnd()/M->FPS();
    bPlay=true;
}
void st_AnimParam::Update(float dt, float speed, bool loop)
{
	if (!bPlay) return;
	t+=speed*dt;
    if (t>max_t){
#ifdef _EDITOR
		if (loop) t=t-max_t+min_t; else
#endif
		t=max_t;
	}
}

#ifdef _EDITOR
void CEditableObject::OnFrame()
{
	if (IsSkeleton()){
		BoneVec& lst = m_Bones;
    	if (IsSMotionActive()){
            Fvector R,T;
            int i=0;
            for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++, i++){
	            m_ActiveSMotion->Evaluate(i,m_SMParam.Frame(),T,R);
                (*b_it)->Update(T,R);
            }
            m_SMParam.Update(Device.fTimeDelta,m_ActiveSMotion->fSpeed,!m_ActiveSMotion->m_Flags.is(esmStopAtEnd));
        }else{
		    //for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++) (*b_it)->Reset();
        }
        CalculateAnimation();
    }
}
#endif

void CEditableObject::GotoBindPose()
{
    BoneVec& lst = m_Bones;
    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++) (*b_it)->Reset();
    CalculateAnimation();
#ifdef _EDITOR
    UI.RedrawScene();
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
void CEditableObject::CalculateAnimation(bool bGenInvMat)
{
    for(BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
        Flags8 flags; flags.zero();
        if (m_ActiveSMotion) flags = m_ActiveSMotion->GetMotionFlags(b_it-m_Bones.begin());
//        else if (bGenInvMat) flag = m_SMotions[0]->GetMotionFlag(b_it-m_Bones.begin());
        Fmatrix& M = (*b_it)->MTransform();
        Fmatrix& L = (*b_it)->LTransform();
        Fmatrix& parent = ((*b_it)->ParentIndex()>-1)?m_Bones[(*b_it)->ParentIndex()]->LTransform():Fidentity;
        const Fvector& r = (*b_it)->Rotate();
        if (flags.is(st_BoneMotion::flWorldOrient)){
	        M.setHPB(-r.x,-r.y,-r.z);
            M.c.set((*b_it)->Offset());
			L.mul(parent,M);
            L.i.set(M.i);
            L.j.set(M.j);
            L.k.set(M.k);
        }else{
            M.setHPB(-r.x,-r.y,-r.z);
            M.c.set((*b_it)->Offset());
            L.mul(parent,M);
        }
/*
        {
            ELog.Msg(mtInformation,"A: [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f]",
                        M.i.x,M.i.y,M.i.z,M._14_,
                        M.j.x,M.j.y,M.j.z,M._24_,
                        M.k.x,M.k.y,M.k.z,M._34_,
                        M.c.x,M.c.y,M.c.z,M._44_);
        }
        if (m_ActiveSMotion){
			GetBoneWorldTransform(b_it-m_Bones.begin(),m_SMParam.Frame(),m_ActiveSMotion,M);
    	    {
        	    ELog.Msg(mtInformation,"B: [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f]",
            	            M.i.x,M.i.y,M.i.z,M._14_,
                	        M.j.x,M.j.y,M.j.z,M._24_,
                    	    M.k.x,M.k.y,M.k.z,M._34_,
                        	M.c.x,M.c.y,M.c.z,M._44_);
	        }
        }
*/
        if (bGenInvMat) (*b_it)->LITransform().invert((*b_it)->LTransform());
    }
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

CSMotion* CEditableObject::AppendSMotion(LPCSTR name, LPCSTR fname)
{
	VERIFY(IsSkeleton());
	CSMotion* M = LoadSMotion(fname);
    if (M){
	  	if (CheckBoneCompliance(M)){
        	M->SortBonesBySkeleton(m_Bones);
        	M->SetName(name);
	     	m_SMotions.push_back(M);
        }else{
        	ELog.DlgMsg(mtError,"Append failed.",fname);
	    	xr_delete(M);
        }
    }else{
		ELog.DlgMsg(mtError,"Motion '%s' can't load. Append failed.",fname);
    }
    return M;
}

bool CEditableObject::ReloadSMotion(CSMotion* src, const char* fname)
{
	VERIFY(IsSkeleton());
	CSMotion* M = LoadSMotion(fname);
    if (M){
	  	if (CheckBoneCompliance(M)){
        	src->CopyMotion(M);
            return true;
        }else                        ELog.DlgMsg(mtError,"Reload failed.",fname);
		xr_delete(M);
    }else{
		ELog.DlgMsg(mtError,"Motion '%s' can't load. Append failed.",fname);
    }
    return false;
}

void CEditableObject::ClearSMotions()
{
	SetActiveSMotion(0);
    for(SMotionIt m_it=m_SMotions.begin(); m_it!=m_SMotions.end();m_it++)xr_delete(*m_it);
    m_SMotions.clear();
}

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
    SMotionVec& lst = m_SMotions;
    for(SMotionIt m=lst.begin(); m!=lst.end(); m++)
        if ((Ignore!=(*m))&&(stricmp((*m)->Name(),name)==0)) return (*m);
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

void CEditableObject::UpdateBoneParenting()
{
    // update parenting
    for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
        BoneIt parent=std::find_if(m_Bones.begin(),m_Bones.end(),fBoneNameEQ((*b_it)->Parent()));
        (*b_it)->ParentIndex()=(parent==m_Bones.end())?-1:parent-m_Bones.begin();
    }
}

CBone* CEditableObject::FindBoneByName(const char* name)
{
	BoneIt parent = std::find_if(m_Bones.begin(),m_Bones.end(),fBoneNameEQ(name));
    return (parent==m_Bones.end())?0:*parent;
}

int CEditableObject::BoneIDByName(LPCSTR name)
{
	BoneIt bone = std::find_if(m_Bones.begin(),m_Bones.end(),fBoneNameEQ(name));
    return (bone!=m_Bones.end())?(bone-m_Bones.begin()):-1;
}

int CEditableObject::GetRootBoneID()
{
    for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
    	if ((*b_it)->IsRoot()) return b_it-m_Bones.begin();
    THROW;
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
    do{ lst.push_back(idx); }while((idx=m_Bones[idx]->ParentIndex())>-1);
    for (int i=lst.size()-1; i>=0; i--){
    	idx = lst[i];
	    Flags8 flags	= motion->GetMotionFlags(idx);
    	Fvector T,R;
        Fmatrix rot, mat;
        motion->Evaluate(idx,t,T,R);
        if (flags.is(st_BoneMotion::flWorldOrient)){
            rot.setHPB(-R.x,-R.y,-R.z);
            mat.identity();
            mat.c.set(T);
            mat.mulA(matrix);
            mat.i.set(rot.i);
            mat.j.set(rot.j);
            mat.k.set(rot.k);
        }else{
            mat.setHPB(-R.x,-R.y,-R.z);
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
	if (m_Bones.size()!=lst.size()){
		ELog.Msg(mtError,"Different bone count.\nObject has '%d' bones. Motion has '%d' bones.",m_Bones.size(),lst.size());
    	return false;
    }
    for(BoneMotionIt bm_it=lst.begin(); bm_it!=lst.end(); bm_it++)
    	if (!FindBoneByName(bm_it->name)){
        	ELog.Msg(mtError,"Can't find bone '%s' in object.",bm_it->name);
        	return false;
        }
    for(BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
    	if (!M->FindBoneMotion((*b_it)->Name())){
        	ELog.Msg(mtError,"Can't find bone '%s' in motion.",(*b_it)->Name());
        	return false;
        }
    return true;
}


