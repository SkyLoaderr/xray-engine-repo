#include "stdafx.h"
#pragma hdrstop

#include "customobject.h"
#include "motion.h"
#include "envelope.h"
#include "d3dutils.h"
#include "ui_main.h"
#include "PropertiesList.h"
#include "PropertiesListHelper.h"
#include "bottombar.h"

void __fastcall CCustomObject::OnMotionableChange(PropValue* sender)
{
	if (m_CO_Flags.is(flMotion)){
    	m_Motion		= xr_new<COMotion>();
        m_MotionParams	= xr_new<SAnimParams>();
        UI.Command	(COMMAND_UPDATE_PROPERTIES);
    }else{
    	xr_delete		(m_Motion);
    	xr_delete		(m_MotionParams);
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

    if (Selected()&&m_MotionParams->bPlay)
    {
    	AnimationUpdate			(m_MotionParams->Frame());
        m_MotionParams->Update	(Device.fTimeDelta,1.f,true);
    }
}

static FvectorVec path_points;
static FvectorVec path_key_points;
void CCustomObject::AnimationDrawPath()
{
    // motion path
	VERIFY (m_Motion);
#ifdef _LEVEL_EDITOR
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
                DU.DrawCross	(T,0.1f,0.1f,0.1f, 0.1f,0.1f,0.1f, clr,false);
                DU.DrawText		(T,AnsiString().sprintf("K: %3.3f",(*k_it)->time).c_str(),0xffffffff,0x00000000);
            }
        }
    }
#endif    
}

void __fastcall	CCustomObject::OnMotionControlClick(PropValue* value, bool& bModif)
{
	ButtonValue* B = dynamic_cast<ButtonValue*>(value); R_ASSERT(B);
	switch(B->btn_num){
    case 0:{
		m_MotionParams->t 	= m_MotionParams->min_t;
		m_MotionParams->bPlay= FALSE;
    }break;
    case 1:{
    	float min_k;
    	float max_k;
		m_Motion->FindNearestKey(m_MotionParams->t, min_k, max_k);
        m_MotionParams->t	= min_k;
		m_MotionParams->bPlay= FALSE;
    }break;
    case 2:{
		m_MotionParams->t	-= 1.f/30.f;
		m_MotionParams->bPlay= FALSE;
    }break;
    case 3:{
		m_MotionParams->bPlay= TRUE;
    }break;
    case 4:{
		m_MotionParams->bPlay= FALSE;
    }break;
    case 5:{
		m_MotionParams->t	+= 1.f/30.f;
		m_MotionParams->bPlay= FALSE;
    }break;
    case 6:{
    	float min_k;
    	float max_k;
		m_Motion->FindNearestKey(m_MotionParams->t, min_k, max_k);
        m_MotionParams->t	= max_k;
		m_MotionParams->bPlay= FALSE;
    }break;
    case 7:{
		m_MotionParams->t 	= m_MotionParams->max_t;
		m_MotionParams->bPlay= FALSE;
    }break;
    }
    AnimationUpdate			(m_MotionParams->Frame());
    UI.Command				(COMMAND_UPDATE_PROPERTIES);
    bModif = false;
}

void __fastcall	CCustomObject::OnMotionCommandsClick(PropValue* value, bool& bModif)
{
	ButtonValue* B = dynamic_cast<ButtonValue*>(value); R_ASSERT(B);
	switch(B->btn_num){
    case 0:
    	AnimationCreateKey	(m_MotionParams->t);
    break;
    case 1:
    	AnimationDeleteKey	(m_MotionParams->t);
    break;
    case 2:{
    	TProperties* P 		= TProperties::CreateModalForm();
        PropItemVec items;
        float from_time=m_MotionParams->min_t,to_time=m_MotionParams->max_t,scale_factor=1.f;
		PHelper.CreateFloat	(items,"From Time", 	&from_time, 	from_time, to_time, 	1.f/30.f, 3);
		PHelper.CreateFloat	(items,"To Time",   	&to_time, 		from_time, to_time, 	1.f/30.f, 3);
		PHelper.CreateFloat	(items,"Scale",			&scale_factor, 	-1000.f, 1000.f);
        P->AssignItems		(items,true,"Scale keys");
        if (mrOk==P->ShowPropertiesModal()){
        	m_Motion->ScaleKeys(from_time,to_time,scale_factor);
        }
        TProperties::DestroyForm(P);
        float mx; m_Motion->GetLength(0,&mx);
        if (m_MotionParams->max_t<mx){ 
        	m_MotionParams->max_t=mx;
			m_Motion->SetParam	(m_MotionParams->min_t*30.f,m_MotionParams->max_t*30.f,30.f);
        }
    }break;
    case 3:{
    	TProperties* P 		= TProperties::CreateModalForm();
        PropItemVec items;
        float from_time=m_MotionParams->min_t,to_time=m_MotionParams->max_t,speed=5.f;
//		PHelper.CreateFloat	(items,"From Time", 	&from_time, 	from_time, to_time, 	1.f/30.f, 3);
//		PHelper.CreateFloat	(items,"To Time",   	&to_time, 		from_time, to_time, 	1.f/30.f, 3);
		PHelper.CreateFloat	(items,"Speed (m/sec)", &speed, 		0.f, 100.f);
        P->AssignItems		(items,true,"Normalize keys");
        if (mrOk==P->ShowPropertiesModal()){
        	m_Motion->NormalizeKeys(from_time,to_time,speed);
        }
        TProperties::DestroyForm(P);
        float mx; m_Motion->GetLength(0,&mx);
        if (m_MotionParams->max_t<mx){ 
        	m_MotionParams->max_t=mx;
			m_Motion->SetParam	(m_MotionParams->min_t*30.f,m_MotionParams->max_t*30.f,30.f);
        }
    }break;
	}
    AnimationUpdate			(m_MotionParams->Frame());
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
            m_MotionParams->Set	(m_Motion);
            AnimationUpdate		(m_MotionParams->Frame());
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
	m_Motion->SetParam	(m_MotionParams->min_t*30.f,m_MotionParams->max_t*30.f,30.f);
    UI.Command			(COMMAND_UPDATE_PROPERTIES);
}

void __fastcall	CCustomObject::OnMotionCurrentFrameChange(PropValue* value)
{
	if (m_MotionParams->t<m_MotionParams->min_t) 		m_MotionParams->min_t = m_MotionParams->t;
    else if (m_MotionParams->t>m_MotionParams->max_t) m_MotionParams->max_t = m_MotionParams->t;
	m_Motion->SetParam	(m_MotionParams->min_t*30.f,m_MotionParams->max_t*30.f,30.f);
    m_MotionParams->bPlay= FALSE;
    AnimationUpdate		(m_MotionParams->Frame());
    UI.Command			(COMMAND_UPDATE_PROPERTIES);
}

void __fastcall	CCustomObject::OnMotionCameraViewChange(PropValue* value)
{
	if (m_CO_Flags.is(flCameraView))
	    AnimationUpdate	(m_MotionParams->Frame());
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
	    V				= PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Motion\\Start Frame (sec)"),	&m_MotionParams->min_t, -10000.f, m_MotionParams->max_t, 	1.f/30.f, 3);
    	V->OnChangeEvent= OnMotionFrameChange;
		V				= PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Motion\\End Frame (sec)"),		&m_MotionParams->max_t, m_MotionParams->min_t, 10000.f, 	1.f/30.f, 3);
    	V->OnChangeEvent= OnMotionFrameChange;
		V				= PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Motion\\Current Frame (sec)"),	&m_MotionParams->t, -10000.f, 10000.f,	1.f/30.f, 3);
    	V->OnChangeEvent= OnMotionCurrentFrameChange;
        				  PHelper.CreateCaption		(items,FHelper.PrepareKey(pref,"Motion\\Key Count"),			m_Motion->KeyCount());
        				  PHelper.CreateCaption		(items,FHelper.PrepareKey(pref,"Motion\\Length (sec)"),			AnsiString().sprintf("%3.2f",m_Motion->GetLength()));
	}
}
 
