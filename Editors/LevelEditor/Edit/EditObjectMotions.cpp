//----------------------------------------------------
// file: CEditableObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"

#include "motion.h"
#include "bone.h"

#ifdef _EDITOR
#include "ui_main.h"
#endif
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
void st_AnimParam::Set(CCustomMotion* M, bool loop){
	t=0;
    min_t=(float)M->FrameStart()/M->FPS();
    max_t=(float)M->FrameEnd()/M->FPS();
    bPlay=false;
	bLooped=loop;
}
void st_AnimParam::Update(float dt){
	if (!bPlay) return;
	t+=dt;
    if (t>max_t){ 
#ifdef _EDITOR
		if (bLooped) t=min_t;
		else 
#endif
			t=max_t; 
	}
}

#ifdef _EDITOR
void CEditableObject::OnFrame(){
	if (IsSkeleton()){
		BoneVec& lst = m_Bones;
    	if (IsSMotionActive()){
            Fvector R,T;
            int i=0;
            for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++, i++){
	            m_ActiveSMotion->Evaluate(i,m_SMParam.Frame(),T,R);
                (*b_it)->Update(T,R);
            }
            m_SMParam.Update(Device.fTimeDelta);
        }else{
		    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++) (*b_it)->Reset();
        }
        CalculateAnimation();
    }
}
#endif

CSMotion* CEditableObject::ResetSAnimation(){
	CSMotion* M=m_ActiveSMotion;
	SetActiveSMotion(0);
    return M;
}

void CEditableObject::ResetAnimation(){
    ResetSAnimation();
}

//----------------------------------------------------
// Skeletal motion
//----------------------------------------------------
void CEditableObject::CalculateAnimation(bool bGenInvMat){
    for(BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
        DWORD flag=0;
        if (m_ActiveSMotion) flag = m_ActiveSMotion->GetMotionFlag(b_it-m_Bones.begin());
//        else if (bGenInvMat) flag = m_SMotions[0]->GetMotionFlag(b_it-m_Bones.begin());
        Fmatrix M,R;
        Fmatrix& parent = ((*b_it)->ParentIndex()>-1)?m_Bones[(*b_it)->ParentIndex()]->LTransform():Fidentity;
        const Fvector& r = (*b_it)->Rotate();
        if (flag&WORLD_ORIENTATION){
	        R.setHPB(-r.x,-r.y,-r.z);
            M.identity();
    	    M.c.set((*b_it)->Offset());
			M.mulA(parent);
            M.i.set(R.i);
            M.j.set(R.j);
            M.k.set(R.k);
        }else{
            M.setHPB(-r.x,-r.y,-r.z);
            M.c.set((*b_it)->Offset());
            M.mulA(parent);
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
        (*b_it)->LTransform().set(M);
        if (bGenInvMat) (*b_it)->LITransform().invert((*b_it)->LTransform());
    }
}

void CEditableObject::SetActiveSMotion(CSMotion* mot){
	m_ActiveSMotion=mot;
	if (m_ActiveSMotion) m_SMParam.Set(m_ActiveSMotion,!m_ActiveSMotion->IsFlag(esmStopAtEnd));
    BoneVec& lst = m_Bones;
    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++) (*b_it)->Reset();
	CalculateAnimation();
#ifdef _EDITOR
    UI.RedrawScene();
#endif
}

bool CEditableObject::RemoveSMotion(const char* name){
    SMotionVec& lst = m_SMotions;
    for(SMotionIt m=lst.begin(); m!=lst.end(); m++)
        if ((stricmp((*m)->Name(),name)==0)){
        	if (m_ActiveSMotion==*m) SetActiveSMotion(0);
            _DELETE(*m);
        	lst.erase(m);
            return true;
        }
    return false;
}

CSMotion* CEditableObject::AppendSMotion(LPCSTR name, LPCSTR fname){
	VERIFY(IsSkeleton());
	CSMotion* M = LoadSMotion(fname);
    if (M){
	  	if (CheckBoneCompliance(M)){
        	M->SortBonesBySkeleton(m_Bones);
        	M->SetName(name);
	     	m_SMotions.push_back(M);
        }else{
        	ELog.DlgMsg(mtError,"Motion file '%s' has different bone names. Append failed.",fname);
	    	_DELETE(M);
        }
    }else{
		ELog.DlgMsg(mtError,"Motion '%s' can't load. Append failed.",fname);
    }
    return M;
}

bool CEditableObject::ReloadSMotion(CSMotion* src, const char* fname){
	VERIFY(IsSkeleton());
	CSMotion* M = LoadSMotion(fname);
    if (M){
	  	if (CheckBoneCompliance(M)){
        	src->CopyMotion(M);
            return true;
        }else                        ELog.DlgMsg(mtError,"Motion file '%s' has different bone names. Reload failed.",fname);
		_DELETE(M);
    }else{
		ELog.DlgMsg(mtError,"Motion '%s' can't load. Append failed.",fname);
    }
    return false;
}

void CEditableObject::ClearSMotions(){
	SetActiveSMotion(0);
    for(SMotionIt m_it=m_SMotions.begin(); m_it!=m_SMotions.end();m_it++)_DELETE(*m_it);
    m_SMotions.clear();
}

bool CEditableObject::LoadSMotions(const char* fname){
	CFileStream F(fname);
    ClearSMotions();
    // object motions
    m_SMotions.resize(F.Rdword());
	SetActiveSMotion(0);
    for (SMotionIt m_it=m_SMotions.begin(); m_it!=m_SMotions.end(); m_it++){
        *m_it = new CSMotion();
        if (!(*m_it)->Load(F)){
            ELog.DlgMsg(mtError,"Motions has different version. Load failed.");
            _DELETE(*m_it);
            m_SMotions.clear();
            return false;
        }
	  	if (!CheckBoneCompliance(*m_it)){
        	ClearSMotions();
            ELog.DlgMsg(mtError,"Motions file '%s' has different bone names. Load failed.",fname);
            _DELETE(m_it);
            return false;
        }
    }
	return true;
}

bool CEditableObject::SaveSMotions(const char* fname){
	CFS_Memory F;
    F.Wdword		(m_SMotions.size());
    for (SMotionIt m_it=m_SMotions.begin(); m_it!=m_SMotions.end(); m_it++) (*m_it)->Save(F);
    F.SaveTo		(fname,0);
    return true;
}

bool CEditableObject::RenameSMotion(const char* old_name, const char* new_name){
	if (stricmp(old_name,new_name)==0) return true;
    if (FindSMotionByName(new_name)) return false;
	CSMotion* M = FindSMotionByName(old_name); VERIFY(M);
    M->SetName(new_name);
    return true;
}

CSMotion* CEditableObject::FindSMotionByName	(const char* name, const CSMotion* Ignore){
    SMotionVec& lst = m_SMotions;
    for(SMotionIt m=lst.begin(); m!=lst.end(); m++)
        if ((Ignore!=(*m))&&(stricmp((*m)->Name(),name)==0)) return (*m);
    return 0;
}

void CEditableObject::GenerateSMotionName(char* buffer, const char* start_name, const CSMotion* M){
	strcpy(buffer,start_name);
    int idx = 0;
	while(FindSMotionByName(buffer,M)){
		sprintf( buffer, "%s_%d", start_name, idx );
    	idx++;
    }
    strlwr(buffer);
}

void CEditableObject::UpdateBoneParenting(){
    // update parenting
    for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
        BoneIt parent=std::find_if(m_Bones.begin(),m_Bones.end(),fBoneNameEQ((*b_it)->Parent()));
        (*b_it)->ParentIndex()=(parent==m_Bones.end())?-1:parent-m_Bones.begin();
    }
}

CBone* CEditableObject::FindBoneByName(const char* name){
	BoneIt parent = std::find_if(m_Bones.begin(),m_Bones.end(),fBoneNameEQ(name));
    return (parent==m_Bones.end())?0:*parent;
}

int CEditableObject::BoneIDByName(LPCSTR name){
	BoneIt bone = std::find_if(m_Bones.begin(),m_Bones.end(),fBoneNameEQ(name));
    return (bone!=m_Bones.end())?(bone-m_Bones.begin()):-1;
}

int CEditableObject::GetRootBoneID(){
    for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
    	if (-1==(*b_it)->ParentIndex()) return b_it-m_Bones.begin();
    THROW;
}

int CEditableObject::PartIDByName(LPCSTR name){
	for (BPIt it=m_BoneParts.begin(); it!=m_BoneParts.end(); it++)
    	if (it->alias==name) return it-m_BoneParts.begin();
    return -1;
}

LPCSTR CEditableObject::BoneNameByID(int id){
	VERIFY((id>=0)&&(id<(int)m_Bones.size()));
    return m_Bones[id]->Name();
}

int	CEditableObject::GetBoneIndexByWMap(const char* wm_name){
	BoneIt bone = std::find_if(m_Bones.begin(),m_Bones.end(),fBoneWMNameEQ(wm_name));
    return (bone==m_Bones.end())?-1:bone-m_Bones.begin();
}

void CEditableObject::GetBoneWorldTransform(DWORD bone_idx, float t, CSMotion* motion, Fmatrix& matrix){
	VERIFY(bone_idx<m_Bones.size());
    int idx	= bone_idx;
    matrix.identity();
    INTVec lst;
    do{ lst.push_back(idx); }while((idx=m_Bones[idx]->ParentIndex())>-1);
    for (int i=lst.size()-1; i>=0; i--){
    	idx = lst[i];
	    DWORD flag	= motion->GetMotionFlag(idx);
    	Fvector T,R;
        Fmatrix rot, mat;
        motion->Evaluate(idx,t,T,R);
        if (flag&WORLD_ORIENTATION){
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

bool CEditableObject::CheckBoneCompliance(CSMotion* M){
	VERIFY(M);
    BoneMotionVec& lst = M->BoneMotions();
	if (m_Bones.size()!=lst.size()) return false;
    for(BoneMotionIt bm_it=lst.begin(); bm_it!=lst.end(); bm_it++)
    	if (!FindBoneByName(bm_it->name)) return false;
    for(BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++)
    	if (!M->FindBoneMotion((*b_it)->Name())) return false;
    return true;
}


