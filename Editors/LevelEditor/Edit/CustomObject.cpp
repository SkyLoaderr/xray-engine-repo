//----------------------------------------------------
// file: CustomObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "bottombar.h"
#include "topbar.h"
#include "editorpref.h"
#include "customobject.h"
#include "ui_main.h"
#include "d3dutils.h"
#include "motion.h"

#define CUSTOMOBJECT_CHUNK_PARAMS 		0xF900
#define CUSTOMOBJECT_CHUNK_LOCK	 		0xF902
#define CUSTOMOBJECT_CHUNK_TRANSFORM	0xF903
#define CUSTOMOBJECT_CHUNK_GROUP		0xF904
#define CUSTOMOBJECT_CHUNK_MOTION		0xF905
#define CUSTOMOBJECT_CHUNK_FLAGS		0xF906
#define CUSTOMOBJECT_CHUNK_NAME			0xF907
#define CUSTOMOBJECT_CHUNK_MOTION_PARAM	0xF908
//----------------------------------------------------

CCustomObject::CCustomObject(LPVOID data, LPCSTR name)
{
    ClassID 	= OBJCLASS_DUMMY;
    FName[0] 	= 0;
    if (name) 	strcpy(FName, name);
    m_CO_Flags.set	(flVisible);
    m_RT_Flags.set	(flRT_Valid);
    m_pOwnerObject	= 0;
    ResetTransform	();
    m_RT_Flags.set(flRT_UpdateTransform,FALSE);
    m_Motion		= NULL;
    m_MotionParams 	= NULL;
}

CCustomObject::CCustomObject(CCustomObject* source)
{
    ClassID 	= source->ClassID;
    Name		= source->Name;
    m_CO_Flags.set	(flVisible);
    m_RT_Flags.set	(flRT_Valid);
    m_pOwnerObject	= 0;
    m_Motion		= NULL;
    m_MotionParams 	= NULL;
}
CCustomObject::~CCustomObject()
{
	xr_delete				(m_Motion);
    xr_delete				(m_MotionParams);
}

bool CCustomObject::IsRender()
{
    return (Selected()&&m_CO_Flags.is(flMotion));
}

void CCustomObject::OnUpdateTransform()
{
	m_RT_Flags.set			(flRT_UpdateTransform,FALSE);
    // update transform matrix
	FTransformR.setHPB		(PRotation.y, PRotation.x, PRotation.z);

	FTransformS.scale		(PScale);
	FTransformP.translate	(PPosition);
	FTransformRP.mul		(FTransformP,FTransformR);
	FTransform.mul			(FTransformRP,FTransformS);
    FITransformRP.invert	(FTransformRP);
    FITransform.invert		(FTransform);

    if (Motionable()&&Visible()&&Selected()&&m_CO_Flags.is(flAutoKey)) AnimationCreateKey(m_MotionParams->Frame());
}

void CCustomObject::Select( int flag )
{
    if (m_CO_Flags.is(flVisible)){
        m_CO_Flags.set			(flSelected,(flag==-1)?(m_CO_Flags.is(flSelected)?FALSE:TRUE):flag);
        UI.RedrawScene		();
        UI.Command			(COMMAND_UPDATE_PROPERTIES);
    }
}

void CCustomObject::Show( BOOL flag )
{
	m_CO_Flags.set	   	(flVisible,flag);
    if (!m_CO_Flags.is(flVisible)) m_CO_Flags.set(flSelected, FALSE);
    UI.RedrawScene();
};

void CCustomObject::Lock( BOOL flag )
{
	m_CO_Flags.set(flLocked,flag);
};

bool CCustomObject::Load(IReader& F)
{
    if (F.find_chunk(CUSTOMOBJECT_CHUNK_FLAGS)){
        m_CO_Flags.set 	(F.r_u32());
    	
        R_ASSERT(F.find_chunk(CUSTOMOBJECT_CHUNK_NAME));
        F.r_stringZ		(FName);
    }else{
    	
        R_ASSERT(F.find_chunk(CUSTOMOBJECT_CHUNK_PARAMS));
        m_CO_Flags.set	(flSelected,F.r_u16());
        m_CO_Flags.set 	(flVisible,	F.r_u16());
        F.r_stringZ		(FName);

        if(F.find_chunk(CUSTOMOBJECT_CHUNK_LOCK))
            m_CO_Flags.set	(flLocked, F.r_u16());
    }

	if(F.find_chunk(CUSTOMOBJECT_CHUNK_TRANSFORM)){
        F.r_fvector3(FPosition);
        F.r_fvector3(FRotation);
        F.r_fvector3(FScale);
    }

    // object motion
    if (F.find_chunk(CUSTOMOBJECT_CHUNK_MOTION)){
        m_Motion 	= xr_new<COMotion>();
        if (!m_Motion->Load(F)){
            ELog.Msg(mtError,"CustomObject: '%s' - motion has different version. Load failed.",Name);
            xr_delete(m_Motion);
        }
        m_MotionParams = xr_new<SAnimParams>();
	    m_MotionParams->Set(m_Motion);
        AnimationUpdate(m_MotionParams->Frame());
    }

    if (F.find_chunk(CUSTOMOBJECT_CHUNK_MOTION_PARAM)){
    	m_MotionParams->t = F.r_float();
        AnimationUpdate(m_MotionParams->Frame());
    }

//	UpdateTransform	(true); // нужно для секторов, иначе неправильный бокс
	UpdateTransform	();
//	m_bUpdateTransform = TRUE;

	return true;
}

void CCustomObject::Save(IWriter& F)
{
	F.open_chunk	(CUSTOMOBJECT_CHUNK_FLAGS);
	F.w_u32			(m_CO_Flags.get());
	F.close_chunk	();

	F.open_chunk	(CUSTOMOBJECT_CHUNK_NAME);
	F.w_stringZ		(FName);
	F.close_chunk	();

	F.open_chunk	(CUSTOMOBJECT_CHUNK_TRANSFORM);
    F.w_fvector3 	(FPosition);
    F.w_fvector3 	(FRotation);
    F.w_fvector3 	(FScale);
	F.close_chunk	();

    // object motion
    if (m_CO_Flags.is(flMotion)){
    	VERIFY			(m_Motion);
		F.open_chunk	(CUSTOMOBJECT_CHUNK_MOTION);
		m_Motion->Save	(F);
		F.close_chunk	();

        F.open_chunk	(CUSTOMOBJECT_CHUNK_MOTION_PARAM);
        F.w_float		(m_MotionParams->t);
        F.close_chunk	();
    }
}
//----------------------------------------------------

void CCustomObject::OnFrame()
{
    if (m_Motion) 			AnimationOnFrame();
	if (m_RT_Flags.is(flRT_UpdateTransform)) OnUpdateTransform();
}

void CCustomObject::Render(int priority, bool strictB2F)
{
	if ((1==priority)&&(false==strictB2F)){
        if (fraBottomBar->miDrawObjectsPivot->Checked&&Selected()){
            Device.SetShader(Device.m_WireShader);
            DU.DrawObjectAxis(FTransformRP);
        }
        if (m_Motion&&Visible()&&Selected())
            AnimationDrawPath();
    }
}

bool CCustomObject::RaySelect(int flag, const Fvector& start, const Fvector& dir, bool bRayTest){
	float dist = UI.ZFar();
	if ((bRayTest&&RayPick(dist,start,dir))||!bRayTest){
		Select(flag);
        return true;
    }
    return false;
};

bool CCustomObject::FrustumSelect(int flag, const CFrustum& frustum){
	if (FrustumPick(frustum)){
    	Select(flag);
        return true;
    }
	return false;
};


