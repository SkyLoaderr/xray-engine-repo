//----------------------------------------------------
// file: CEditObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"

#include "motion.h"
#include "bone.h"
#include "ui_main.h"
#include "BottomBar.h"

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
void st_AnimParam::Set(CCustomMotion* M){ 
	t=0; 
    min_t=(float)M->FrameStart()/M->FPS(); 
    max_t=(float)M->FrameEnd()/M->FPS();
}
void st_AnimParam::Update(float dt){ 
	t+=dt; 
    if (t>max_t){ if (fraBottomBar->miObjectLoopedAnimation->Checked) t=min_t; else t=max_t; }
}
    
void CEditObject::RTL_Update( float dT ){
    if (IsOMotionActive()){
        Fvector R,T,r;
        m_ActiveOMotion->Evaluate(m_OMParam.Frame(),T,r);
        R.set(r.y,r.x,r.z);
        T.add(vPosition);
        R.add(vRotate);
        UpdateTransform(T, R, vScale);
        m_OMParam.Update(dT);
    }
}

void CEditObject::ResetAnimation(bool upd_t){
	SetActiveOMotion(0,false);
	SetActiveSMotion(0);
    if (upd_t) UpdateTransform();
}

//----------------------------------------------------
// Object motion
//----------------------------------------------------
void CEditObject::SetActiveOMotion(COMotion* mot, bool upd_t){
	m_ActiveOMotion=mot;
    if (m_ActiveOMotion) m_OMParam.Set(m_ActiveOMotion);
    if (upd_t) UpdateTransform();
    UI->RedrawScene();
}

COMotion* CEditObject::FindOMotionByName	(const char* name, const COMotion* Ignore){
    CEditObject* _O = m_LibRef?m_LibRef:this;    VERIFY(_O);
    OMotionVec& lst = _O->m_OMotions;
    for(OMotionIt m=lst.begin(); m!=lst.end(); m++)
        if ((Ignore!=(*m))&&(stricmp((*m)->Name(),name)==0)) return (*m);
    return 0;
}

void CEditObject::RemoveOMotion(const char* name){
    CEditObject* _O = m_LibRef?m_LibRef:this;    VERIFY(_O);
    OMotionVec& lst = _O->m_OMotions;
    for(OMotionIt m=lst.begin(); m!=lst.end(); m++)
        if ((stricmp((*m)->Name(),name)==0)){ 
        	if (m_ActiveOMotion==*m) SetActiveOMotion(0);
            _DELETE(*m);
        	_O->m_OMotions.erase(m); 
            break; 
        }
}

bool CEditObject::RenameOMotion(const char* old_name, const char* new_name){
	if (stricmp(old_name,new_name)==0) return true;
    if (FindOMotionByName(new_name)) return false;
	COMotion* M = FindOMotionByName(old_name); VERIFY(M);
    M->SetName(new_name);
    return true;
}

void CEditObject::GenerateOMotionName(char* buffer, const char* start_name, const COMotion* M){
	strcpy(buffer,start_name);
    int idx = 0;
	while(FindOMotionByName(buffer,M)){
		sprintf( buffer, "%s_%d", start_name, idx );
    	idx++;
    }
    strlwr(buffer);
}

COMotion* CEditObject::AppendOMotion(const char* fname){
	COMotion* M = LoadOMotion(fname);
    if (M) m_OMotions.push_back(M);
    return M;
}

void CEditObject::ClearOMotions(){
	SetActiveOMotion(0);
	if (m_LibRef){ m_LibRef->ClearOMotions(); return; }
    for(OMotionIt m_it=m_OMotions.begin(); m_it!=m_OMotions.end();m_it++)_DELETE(*m_it);
    m_OMotions.clear();
}

void CEditObject::LoadOMotions(const char* fname){
	if (m_LibRef){ m_LibRef->LoadOMotions(fname); return; }
	CFileStream F(fname);
    // object motions
    m_OMotions.resize(F.Rdword());
	SetActiveOMotion(0);
    for (OMotionIt m_it=m_OMotions.begin(); m_it!=m_OMotions.end(); m_it++){ 
        *m_it = new COMotion();
        (*m_it)->Load(F);
    }
}

void CEditObject::SaveOMotions(const char* fname){
	if (m_LibRef){ m_LibRef->SaveOMotions(fname); return; }
	CFS_Memory F;
    F.Wdword		(m_OMotions.size());
    for (OMotionIt m_it=m_OMotions.begin(); m_it!=m_OMotions.end(); m_it++) (*m_it)->Save(F);
    F.SaveTo		(fname,0);
}

//----------------------------------------------------
// Skeletal motion
//----------------------------------------------------
void CEditObject::CalculateAnimation(bool bGenInvMat){
	if (m_LibRef){ m_LibRef->CalculateAnimation(bGenInvMat); return; }
    for(BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
        DWORD flag=0;
        if (m_ActiveSMotion) flag = m_ActiveSMotion->GetMotionFlag(b_it-m_Bones.begin());
//        else if (bGenInvMat) flag = m_SMotions[0]->GetMotionFlag(b_it-m_Bones.begin());
        Fmatrix M,R;
        Fmatrix& parent = ((*b_it)->ParentIndex()>-1)?m_Bones[(*b_it)->ParentIndex()]->LTransform():precalc_identity;
        const Fvector& r = (*b_it)->Rotate();
        if (flag&WORLD_ORIENTATION){
	        R.setHPB(-r.x,-r.y,r.z);
            M.identity();
    	    M.c.set((*b_it)->Offset());
			M.mul2(parent);
            M.i.set(R.i);
            M.j.set(R.j);
            M.k.set(R.k);
        }else{
            M.setHPB(-r.x,-r.y,r.z);
            M.c.set((*b_it)->Offset());
            M.mul2(parent);
        }
/*
        {
            Log->Msg(mtInformation,"A: [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f]",
                        M.i.x,M.i.y,M.i.z,M._14_,
                        M.j.x,M.j.y,M.j.z,M._24_,
                        M.k.x,M.k.y,M.k.z,M._34_,
                        M.c.x,M.c.y,M.c.z,M._44_);
        }
        if (m_ActiveSMotion){
			GetBoneWorldTransform(b_it-m_Bones.begin(),m_SMParam.Frame(),m_ActiveSMotion,M);
    	    {
        	    Log->Msg(mtInformation,"B: [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f], [%3.2f, %3.2f, %3.2f, %3.2f]",
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

void CEditObject::SetActiveSMotion(CSMotion* mot){
	m_ActiveSMotion=mot;
    if (m_ActiveSMotion) m_SMParam.Set(m_ActiveSMotion);
    CEditObject* _O = m_LibRef?m_LibRef:this;
    BoneVec& lst = _O->m_Bones;
    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++) (*b_it)->Reset();
	CalculateAnimation();
    UI->RedrawScene();
}

void CEditObject::RemoveSMotion(const char* name){
    CEditObject* _O = m_LibRef?m_LibRef:this;    VERIFY(_O);
    SMotionVec& lst = _O->m_SMotions;
    for(SMotionIt m=lst.begin(); m!=lst.end(); m++)
        if ((stricmp((*m)->Name(),name)==0)){
        	if (m_ActiveSMotion==*m) SetActiveSMotion(0);
            _DELETE(*m);
        	_O->m_SMotions.erase(m);
            break;
        }
}

CSMotion* CEditObject::AppendSMotion(const char* fname){
	VERIFY(IsSkeleton());
	CSMotion* M = LoadSMotion(fname);
    if (M){
	  	if (CheckBoneCompliance(M)){
	     	m_SMotions.push_back(M);
        }else{
        	Log->DlgMsg(mtError,"Motion file '%s' has different bone names. Append failed.",fname);
	    	_DELETE(M);
        }
    }else{
		Log->DlgMsg(mtError,"Motion '%s' can't load. Append failed.",fname);
    }
    return M;
}

bool CEditObject::ReloadSMotion(CSMotion* src, const char* fname){
	VERIFY(IsSkeleton());
	CSMotion* M = LoadSMotion(fname);
    if (M){
	  	if (CheckBoneCompliance(M)){
        	src->CopyMotion(M);
            return true;
        }else                        Log->DlgMsg(mtError,"Motion file '%s' has different bone names. Reload failed.",fname);
		_DELETE(M);
    }else{
		Log->DlgMsg(mtError,"Motion '%s' can't load. Append failed.",fname);
    }
    return false;
}

void CEditObject::ClearSMotions(){
	SetActiveSMotion(0);
	if (m_LibRef){ m_LibRef->ClearSMotions(); return; }
    for(SMotionIt m_it=m_SMotions.begin(); m_it!=m_SMotions.end();m_it++)_DELETE(*m_it);
    m_SMotions.clear();
}

void CEditObject::LoadSMotions(const char* fname){
	if (m_LibRef){ m_LibRef->LoadSMotions(fname); return; }
	CFileStream F(fname);
    // object motions
    m_SMotions.resize(F.Rdword());
	SetActiveSMotion(0);
    for (SMotionIt m_it=m_SMotions.begin(); m_it!=m_SMotions.end(); m_it++){ 
        *m_it = new CSMotion();
        if (!(*m_it)->Load(F)){
            Log->DlgMsg(mtError,"Motions has different version. Load failed.");
            _DELETE(*m_it);
            m_SMotions.clear();
            break;
        }
	  	if (!CheckBoneCompliance(*m_it)){
        	ClearSMotions();
            Log->DlgMsg(mtError,"Motions file '%s' has different bone names. Load failed.",fname);
            _DELETE(m_it);
            return;
        }
    }
}

void CEditObject::SaveSMotions(const char* fname){
	if (m_LibRef){ m_LibRef->SaveSMotions(fname); return; }
	CFS_Memory F;
    F.Wdword		(m_SMotions.size());
    for (SMotionIt m_it=m_SMotions.begin(); m_it!=m_SMotions.end(); m_it++) (*m_it)->Save(F);
    F.SaveTo		(fname,0);
}

bool CEditObject::RenameSMotion(const char* old_name, const char* new_name){
	if (stricmp(old_name,new_name)==0) return true;
    if (FindSMotionByName(new_name)) return false;
	CSMotion* M = FindSMotionByName(old_name); VERIFY(M);
    M->SetName(new_name);
    return true;
}

CSMotion* CEditObject::FindSMotionByName	(const char* name, const CSMotion* Ignore){
    CEditObject* _O = m_LibRef?m_LibRef:this;    VERIFY(_O);
    SMotionVec& lst = _O->m_SMotions;
    for(SMotionIt m=lst.begin(); m!=lst.end(); m++)
        if ((Ignore!=(*m))&&(stricmp((*m)->Name(),name)==0)) return (*m);
    return 0;
}

void CEditObject::GenerateSMotionName(char* buffer, const char* start_name, const CSMotion* M){
	strcpy(buffer,start_name);
    int idx = 0;
	while(FindSMotionByName(buffer,M)){
		sprintf( buffer, "%s_%d", start_name, idx );
    	idx++;
    }
    strlwr(buffer);
}

void CEditObject::UpdateBoneParenting(){
    // update parenting
    for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){ 
        BoneIt parent=find_if(m_Bones.begin(),m_Bones.end(),fBoneNameEQ((*b_it)->Parent()));
        (*b_it)->ParentIndex()=(parent==m_Bones.end())?-1:parent-m_Bones.begin();
    }
}

CBone* CEditObject::FindBoneByName(const char* name){
    CEditObject* _O = m_LibRef?m_LibRef:this;
	BoneIt parent = find_if(_O->m_Bones.begin(),_O->m_Bones.end(),fBoneNameEQ(name));
    return (parent==m_Bones.end())?0:*parent;
}

int	CEditObject::GetBoneIndexByWMap(const char* wm_name){
    CEditObject* _O = m_LibRef?m_LibRef:this;
	BoneIt bone = find_if(_O->m_Bones.begin(),_O->m_Bones.end(),fBoneWMNameEQ(wm_name));
    return (bone==m_Bones.end())?-1:bone-m_Bones.begin();
}

void CEditObject::GetBoneWorldTransform(DWORD bone_idx, float t, CSMotion* motion, Fmatrix& matrix){
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
            rot.setHPB(-R.x,-R.y,R.z);
            mat.identity();
            mat.c.set(T);
            mat.mul2(matrix);
            mat.i.set(rot.i);
            mat.j.set(rot.j);
            mat.k.set(rot.k);
        }else{
            mat.setHPB(-R.x,-R.y,R.z);
            mat.c.set(T);
            mat.mul2(matrix);
        }
        matrix.set(mat);
    }
}

bool CEditObject::CheckBoneCompliance(CSMotion* M){
	VERIFY(M);
    BoneMotionVec& lst = M->BoneMotions();
	if (m_Bones.size()!=lst.size()) return false;
    for(BoneMotionIt bm_it=lst.begin(); bm_it!=lst.end(); bm_it++){
    	if (!FindBoneByName(bm_it->name)) return false;
    }
    return true;
}


