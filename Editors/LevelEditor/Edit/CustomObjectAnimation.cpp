#include "stdafx.h"
#pragma hdrstop

#include "customobject.h"
#include "motion.h"
#include "envelope.h"
#include "d3dutils.h"
#include "ui_main.h"
#include "PropertiesList.h"
#include "PropertiesListHelper.h"

void st_AnimParams::Set(float start_frame, float end_frame, float fps)
{
    min_t=start_frame/fps;
    max_t=end_frame/fps;
}

void st_AnimParams::Set(CCustomMotion* M)
{
    Set((float)M->FrameStart(),(float)M->FrameEnd(),M->FPS());
	t=min_t;
//    bPlay=true;
}
void st_AnimParams::Update(float dt, float speed, bool loop)
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

void __fastcall CCustomObject::OnMotionableChange(PropValue* sender)
{
	if (m_CO_Flags.is(flMotion)){
    	m_Motion	= xr_new<COMotion>();
        UI.Command	(COMMAND_UPDATE_PROPERTIES);
    }else{
    	xr_delete	(m_Motion);
    }
}

void CCustomObject::AnimationCreateKey(float t)
{
    Fvector R;		R.set(-PRotation.x,-PRotation.y,-PRotation.z);
    m_Motion->CreateKey(t,PPosition,R);
}

void CCustomObject::AnimationDeleteKey(float t)
{
    m_Motion->DeleteKey(t);
}

//float speed = 0.f;
void CCustomObject::AnimationUpdate(float t)
{
    Fvector R,P,r;   
    m_Motion->_Evaluate		(t,P,r);
    R.set					(-r.y,-r.x,-r.z);

//    speed					= speed*0.9f+(P.distance_to(PPosition)/Device.fTimeDelta)*0.1f;
//    Log("speed: ",speed);
    PPosition 				= P;
    PRotation 				= R;
    BOOL bAK				= m_CO_Flags.is(flAutoKey);
    m_CO_Flags.set			(flAutoKey,FALSE);
    UpdateTransform			(true);
    m_CO_Flags.set			(flAutoKey,bAK);
    if (m_CO_Flags.is(flCameraView))
    	Device.m_Camera.Set	(-r.x,-r.y,-r.z,P.x,P.y,P.z);
}

void CCustomObject::AnimationOnFrame()
{
	VERIFY (m_Motion);

    if (Selected()&&m_MotionParams.bPlay)
    {
    	AnimationUpdate			(m_MotionParams.Frame());
        m_MotionParams.Update	(Device.fTimeDelta,1.f,true);
    }
}

static FvectorVec path_points;
static FvectorVec path_key_points;
void CCustomObject::AnimationDrawPath()
{
    // motion path
	VERIFY (m_Motion);

	if (fraBottomBar->miDrawObjectAnimPath->Checked){
        float fps 				= m_Motion->FPS();
        float min_t				= (float)m_Motion->FrameStart()/fps;
        float max_t				= (float)m_Motion->FrameEnd()/fps;

        Fvector 				T,r;
        u32 clr					= 0xffffffff;
        path_points.clear		();
        path_key_points.clear	();
        for (float t=min_t; (t<max_t)||fsimilar(t,max_t,EPS_L); t+=1/30.f){
            m_Motion->_Evaluate	(t,T,r);
            path_points.push_back(T);
        }

        Device.SetShader		(Device.m_WireShader);
        RCache.set_xform_world	(Fidentity);
        if (!path_points.empty())DU.DrawPrimitiveL		(D3DPT_LINESTRIP,path_points.size()-1,path_points.begin(),path_points.size(),clr,true,false);
        CEnvelope* E 			= m_Motion->Envelope();
        for (KeyIt k_it=E->keys.begin(); k_it!=E->keys.end(); k_it++){
            m_Motion->_Evaluate	((*k_it)->time,T,r);
            if (Device.m_Camera.GetPosition().distance_to_sqr(T)<50.f*50.f){
                DU.DrawCross		(T,0.1f,0.1f,0.1f, 0.1f,0.1f,0.1f, clr,false);
                DU.DrawTextA		(T,AnsiString().sprintf("K: %3.3f",(*k_it)->time).c_str(),0xffffffff,0x00000000);
            }
        }
    }
}

static float xxx=0;
void __fastcall	CCustomObject::OnMotionControlClick(PropValue* value, bool& bModif)
{
	ButtonValue* B = dynamic_cast<ButtonValue*>(value); R_ASSERT(B);
	switch(B->btn_num){
    case 0:{
		m_MotionParams.t 	= m_MotionParams.min_t;
		m_MotionParams.bPlay= FALSE;
    }break;
    case 1:{
    	float min_k;
    	float max_k;
		m_Motion->FindNearestKey(m_MotionParams.t, min_k, max_k);
        m_MotionParams.t	= min_k;
		m_MotionParams.bPlay= FALSE;
    }break;
    case 2:{
		m_MotionParams.t	-= 1.f/30.f;
		m_MotionParams.bPlay= FALSE;
    }break;
    case 3:{
		m_MotionParams.bPlay= TRUE;
    }break;
    case 4:{
		m_MotionParams.bPlay= FALSE;
    }break;
    case 5:{
		m_MotionParams.t	+= 1.f/30.f;
		m_MotionParams.bPlay= FALSE;
    }break;
    case 6:{
    	float min_k;
    	float max_k;
		m_Motion->FindNearestKey(m_MotionParams.t, min_k, max_k);
        m_MotionParams.t	= max_k;
		m_MotionParams.bPlay= FALSE;
    }break;
    case 7:{
		m_MotionParams.t 	= m_MotionParams.max_t;
		m_MotionParams.bPlay= FALSE;
    }break;
    }
    AnimationUpdate			(m_MotionParams.Frame());
    UI.Command				(COMMAND_UPDATE_PROPERTIES);
    bModif = false;
}

void __fastcall	CCustomObject::OnMotionCommandsClick(PropValue* value, bool& bModif)
{
	ButtonValue* B = dynamic_cast<ButtonValue*>(value); R_ASSERT(B);
	switch(B->btn_num){
    case 0:
    	AnimationCreateKey	(m_MotionParams.t);
    break;
    case 1:
    	AnimationDeleteKey	(m_MotionParams.t);
    break;
    case 2:{
    	TProperties* P 		= TProperties::CreateModalForm();
        PropItemVec items;
        float from_time=m_MotionParams.min_t,to_time=m_MotionParams.max_t,scale_factor=1.f;
		PHelper.CreateFloat	(items,"From Time", 	&from_time, 	from_time, to_time, 	1.f/30.f, 3);
		PHelper.CreateFloat	(items,"To Time",   	&to_time, 		from_time, to_time, 	1.f/30.f, 3);
		PHelper.CreateFloat	(items,"Scale",			&scale_factor, 	-1000.f, 1000.f);
        P->AssignItems		(items,true,"Scale keys");
        if (mrOk==P->ShowPropertiesModal()){
        	m_Motion->ScaleKeys(from_time,to_time,scale_factor);
        }
        TProperties::DestroyForm(P);
        float mx; m_Motion->GetLength(0,&mx);
        if (m_MotionParams.max_t<mx){ 
        	m_MotionParams.max_t=mx;
			m_Motion->SetParam	(m_MotionParams.min_t*30.f,m_MotionParams.max_t*30.f,30.f);
        }
    }break;
    case 3:{
    	TProperties* P 		= TProperties::CreateModalForm();
        PropItemVec items;
        float from_time=m_MotionParams.min_t,to_time=m_MotionParams.max_t,speed=5.f;
//		PHelper.CreateFloat	(items,"From Time", 	&from_time, 	from_time, to_time, 	1.f/30.f, 3);
//		PHelper.CreateFloat	(items,"To Time",   	&to_time, 		from_time, to_time, 	1.f/30.f, 3);
		PHelper.CreateFloat	(items,"Speed (m/sec)", &speed, 		0.f, 100.f);
        P->AssignItems		(items,true,"Normalize keys");
        if (mrOk==P->ShowPropertiesModal()){
        	m_Motion->NormalizeKeys(from_time,to_time,speed);
        }
        TProperties::DestroyForm(P);
        float mx; m_Motion->GetLength(0,&mx);
        if (m_MotionParams.max_t<mx){ 
        	m_MotionParams.max_t=mx;
			m_Motion->SetParam	(m_MotionParams.min_t*30.f,m_MotionParams.max_t*30.f,30.f);
        }
    }break;
	}
    AnimationUpdate			(m_MotionParams.Frame());
	bModif = true;
}

void __fastcall	CCustomObject::OnMotionFilesClick(PropValue* value, bool& bModif)
{
	ButtonValue* B = dynamic_cast<ButtonValue*>(value); R_ASSERT(B);
    bModif = false;
	AnsiString fn;
	switch(B->btn_num){
    case 0:
        if(EFS.GetOpenName(_omotion_, fn)){
            m_Motion->LoadMotion(fn.c_str());
            m_MotionParams.Set	(m_Motion);
            AnimationUpdate		(m_MotionParams.Frame());
			bModif 				= true;
		    UI.Command			(COMMAND_UPDATE_PROPERTIES);
        }
    break;
    case 1:
        if(EFS.GetSaveName(_omotion_, fn))
            m_Motion->SaveMotion(fn.c_str());
    break;
	}
}

void __fastcall	CCustomObject::OnMotionFrameChange(PropValue* value)
{
	m_Motion->SetParam	(m_MotionParams.min_t*30.f,m_MotionParams.max_t*30.f,30.f);
    UI.Command			(COMMAND_UPDATE_PROPERTIES);
}

void __fastcall	CCustomObject::OnMotionCurrentFrameChange(PropValue* value)
{
	if (m_MotionParams.t<m_MotionParams.min_t) 		m_MotionParams.min_t = m_MotionParams.t;
    else if (m_MotionParams.t>m_MotionParams.max_t) m_MotionParams.max_t = m_MotionParams.t;
	m_Motion->SetParam	(m_MotionParams.min_t*30.f,m_MotionParams.max_t*30.f,30.f);
    m_MotionParams.bPlay= FALSE;
    AnimationUpdate		(m_MotionParams.Frame());
    UI.Command			(COMMAND_UPDATE_PROPERTIES);
}

void __fastcall	CCustomObject::OnMotionCameraViewChange(PropValue* value)
{
	if (m_CO_Flags.is(flCameraView))
	    AnimationUpdate	(m_MotionParams.Frame());
}

void __fastcall	CCustomObject::OnTransformChange(PropValue* value)
{
	UpdateTransform();
}

void CCustomObject::AnimationFillProp(LPCSTR pref, PropItemVec& items)
{
    PropValue* V		= PHelper.CreateFlag32		(items,FHelper.PrepareKey(pref,"Flags\\Motionable"),&m_CO_Flags, flMotion);
    V->OnChangeEvent	= OnMotionableChange;
	if (Motionable()){
	    ButtonValue* B	= PHelper.CreateButton		(items,FHelper.PrepareKey(pref,"Motion\\Files"),	"Import,Export", 0);
        B->OnBtnClickEvent= OnMotionFilesClick;
	    B				= PHelper.CreateButton		(items,FHelper.PrepareKey(pref,"Motion\\Commands"),	"+ Key,- Key,Scale,Normalize", 0);
        B->OnBtnClickEvent= OnMotionCommandsClick;
	    B				= PHelper.CreateButton		(items,FHelper.PrepareKey(pref,"Motion\\Controls"),	" |<<, +<<, <<, >, ||, >>, >>+, >>|", 0); 
        B->OnBtnClickEvent= OnMotionControlClick;
        				  PHelper.CreateFlag32		(items,FHelper.PrepareKey(pref,"Motion\\Flags\\Auto Key"), 		&m_CO_Flags, flAutoKey);
        V				= PHelper.CreateFlag32		(items,FHelper.PrepareKey(pref,"Motion\\Flags\\Camera View"), 	&m_CO_Flags, flCameraView);
        V->OnChangeEvent= OnMotionCameraViewChange;
	    V				= PHelper.CreateVector		(items,FHelper.PrepareKey(pref,"Motion\\Position"),				&PPosition, -10000.f, 10000.f, 	0.01f, 3);
        V->OnChangeEvent= OnTransformChange;
	    V				= PHelper.CreateAngle3		(items,FHelper.PrepareKey(pref,"Motion\\Rotation"),				&PRotation);
        V->OnChangeEvent= OnTransformChange;
	    V				= PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Motion\\Start Frame (sec)"),	&m_MotionParams.min_t, -10000.f, m_MotionParams.max_t, 	1.f/30.f, 3);
    	V->OnChangeEvent= OnMotionFrameChange;
		V				= PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Motion\\End Frame (sec)"),		&m_MotionParams.max_t, m_MotionParams.min_t, 10000.f, 	1.f/30.f, 3);
    	V->OnChangeEvent= OnMotionFrameChange;
		V				= PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Motion\\Current Frame (sec)"),	&m_MotionParams.t, -10000.f, 10000.f,	1.f/30.f, 3);
    	V->OnChangeEvent= OnMotionCurrentFrameChange;
        				  PHelper.CreateCaption		(items,FHelper.PrepareKey(pref,"Motion\\Key Count"),			m_Motion->KeyCount());
        				  PHelper.CreateCaption		(items,FHelper.PrepareKey(pref,"Motion\\Length (sec)"),			AnsiString().sprintf("%3.2f",m_Motion->GetLength()));
	}
}


/*
	void 			RenderAnimation			();
	// motions
	OMotionVec		m_OMotions;
	COMotion*		m_ActiveOMotion;
	Fvector			m_vMotionPosition;
	Fvector			m_vMotionRotation;

	COMotion* 		FindOMotionByName		(const char* name, const COMotion* Ignore=0);
	void			GenerateOMotionName		(char* buffer, const char* start_name, const COMotion* M);
	void			RemoveOMotion			(const char* name);
	bool			RenameOMotion			(const char* old_name, const char* new_name);
	COMotion*		AppendOMotion			(const char* fname);
	void			ClearOMotions			();
	void			LoadOMotions			(const char* fname);
	void			SaveOMotions			(const char* fname);
	COMotion*		LoadOMotion				(const char* fname);

    // object motions
	IC OMotionIt	FirstOMotion			()	{return m_OMotions.begin();}
	IC OMotionIt	LastOMotion				()	{return m_OMotions.end();}
	IC int			OMotionCount 			()	{return m_OMotions.size();}
	IC bool			IsOMotionable			()	{return !m_OMotions.empty();}
	IC bool			IsOMotionActive			()	{return IsOMotionable()&&m_ActiveOMotion; }

	void			SetActiveOMotion		(COMotion* mot);
	void			ResetActiveOMotion		(){SetActiveOMotion(0);}

    virtual void 	ResetAnimation			(bool upd_t=true);

	virtual Fvector& GetPosition			()	{return m_ActiveOMotion?m_vMotionPosition:FPosition; }
	virtual Fvector& GetRotation			()	{return m_ActiveOMotion?m_vMotionRotation:FRotation; }
	virtual void 	SetPosition				(const Fvector& pos)	{ if (m_ActiveOMotion) m_vMotionPosition.set(pos); else FPosition.set(pos);	UpdateTransform();}
	virtual void 	SetRotation				(const Fvector& rot)	{ if (m_ActiveOMotion) m_vMotionRotation.set(rot); else FRotation.set(rot);	UpdateTransform();}
//----------------------------------------------------
// Object motion
//----------------------------------------------------
+Construct
	m_ActiveOMotion = 0;
	m_vMotionPosition.set(0,0,0);
	m_vMotionRotation.set(0,0,0);

+~
    // object motions
	for(OMotionIt o_it=m_OMotions.begin(); o_it!=m_OMotions.end();o_it++)xr_delete(*o_it);
	m_OMotions.clear();
	m_ActiveOMotion = 0;

+IsRender
	if (fraBottomBar->miDrawObjectAnimPath->Checked&&IsDynamic()&&IsOMotionActive()) RenderAnimation();
    
+OnFrame
	if (IsDynamic()&&IsOMotionActive()){
        Fvector R,P,r;
		m_ActiveOMotion->Evaluate(m_OMParam.Frame(),P,r);
        R.set(-r.y,-r.x,-r.z);
//        P.add(FPosition);
//        R.add(FRotation);
        PPosition = P;
        PRotation = R;
        m_OMParam.Update(Device.fTimeDelta);
        UpdateTransform(true);
	}

+IO
        // object motions
        if (F.find_chunk(SCENEOBJ_CHUNK_OMOTIONS)){
            m_OMotions.resize(F.r_u32());
            for (OMotionIt o_it=m_OMotions.begin(); o_it!=m_OMotions.end(); o_it++){
                *o_it = xr_new<COMotion>();
                if (!(*o_it)->Load(F)){
                    ELog.Msg(mtError,"SceneObject: '%s' - motions has different version. Load failed.",Name);
                    xr_delete(*o_it);
                    m_OMotions.clear();
                    break;
                }
            }
            if (F.find_chunk(SCENEOBJ_CHUNK_ACTIVE_OMOTION)){
            	string256 buf; F.r_stringZ(buf);
                SetActiveOMotion(FindOMotionByName(buf));
            }
        }

    // object motions
    if (IsOMotionable()){
	    F.open_chunk	(SCENEOBJ_CHUNK_OMOTIONS);
    	F.w_u32		(m_OMotions.size());
	    for (OMotionIt o_it=m_OMotions.begin(); o_it!=m_OMotions.end(); o_it++) (*o_it)->Save(F);
    	F.close_chunk	();

        if (IsOMotionActive()){
	        F.open_chunk	(SCENEOBJ_CHUNK_ACTIVE_OMOTION);
    	    F.w_stringZ		(m_ActiveOMotion->Name());
    		F.close_chunk	();
        }
    }
    
//----------------------------------------------------
COMotion* CSceneObject::LoadOMotion(LPCSTR fname)
{
	if (FS.exist(fname)){
    	COMotion* M = xr_new<COMotion>();
        if (!M->LoadMotion(fname)){
        	xr_delete(M);
        }
        return M;
    }
	return 0;
}
//------------------------------------------------------------------------------

void CSceneObject::RenderAnimation(){
    // motion path
    {
        float fps = m_ActiveOMotion->FPS();
        float min_t=(float)m_ActiveOMotion->FrameStart()/fps;
        float max_t=(float)m_ActiveOMotion->FrameEnd()/fps;

        Fvector T,r;
        FvectorVec v;
        u32 clr=0xffffffff;
        for (float t=min_t; t<max_t; t+=0.1f){
            m_ActiveOMotion->Evaluate(t,T,r);
//            T.add(FPosition);
            v.push_back(T);
        }

        Device.SetShader		(Device.m_WireShader);
        RCache.set_xform_world	(Fidentity);
        DU.DrawPrimitiveL		(D3DPT_LINESTRIP,v.size()-1,v.begin(),v.size(),clr,true,false);
    }
}

void CSceneObject::ResetAnimation(bool upd_t)
{
	ResetActiveOMotion();
}

void CSceneObject::SetActiveOMotion(COMotion* mot)
{
	if (mot){
    	m_OMParam.Set(mot,true);
        m_OMParam.Play();
    }
	m_ActiveOMotion=mot;
    UpdateTransform();
    UI.RedrawScene();
}

COMotion* CSceneObject::FindOMotionByName	(const char* name, const COMotion* Ignore)
{
    OMotionVec& lst = m_OMotions;
    for(OMotionIt m=lst.begin(); m!=lst.end(); m++)
        if ((Ignore!=(*m))&&(stricmp((*m)->Name(),name)==0)) return (*m);
    return 0;
}

void CSceneObject::RemoveOMotion(const char* name)
{
    OMotionVec& lst = m_OMotions;
    for(OMotionIt m=lst.begin(); m!=lst.end(); m++)
        if ((stricmp((*m)->Name(),name)==0)){
        	if (m_ActiveOMotion==*m) SetActiveOMotion(0);
            xr_delete(*m);
        	lst.erase(m);
            break;
        }
}

bool CSceneObject::RenameOMotion(const char* old_name, const char* new_name)
{
	if (stricmp(old_name,new_name)==0) return true;
    if (FindOMotionByName(new_name)) return false;
	COMotion* M = FindOMotionByName(old_name); VERIFY(M);
    M->SetName(new_name);
    return true;
}

void CSceneObject::GenerateOMotionName(char* buffer, const char* start_name, const COMotion* M)
{
	strcpy(buffer,start_name);
    int idx = 0;
	while(FindOMotionByName(buffer,M)){
		sprintf( buffer, "%s_%d", start_name, idx );
    	idx++;
    }
    strlwr(buffer);
}

COMotion* CSceneObject::AppendOMotion(const char* fname)
{
	COMotion* M = LoadOMotion(fname);
    if (M) m_OMotions.push_back(M);
    return M;
}

void CSceneObject::ClearOMotions()
{
	SetActiveOMotion(0);
    for(OMotionIt m_it=m_OMotions.begin(); m_it!=m_OMotions.end();m_it++)xr_delete(*m_it);
    m_OMotions.clear();
}

void CSceneObject::LoadOMotions(const char* fname)
{
	IReader* F = FS.r_open(fname);
    ClearOMotions();
    // object motions
    m_OMotions.resize(F->r_u32());
	SetActiveOMotion(0);
    for (OMotionIt m_it=m_OMotions.begin(); m_it!=m_OMotions.end(); m_it++){
        *m_it = xr_new<COMotion>();
        (*m_it)->Load(*F);
    }
    FS.r_close	(F);
}

void CSceneObject::SaveOMotions(const char* fname)
{
	CMemoryWriter F;
    F.w_u32		(m_OMotions.size());
    for (OMotionIt m_it=m_OMotions.begin(); m_it!=m_OMotions.end(); m_it++) (*m_it)->Save(F);
    F.save_to		(fname);
}

*/
 