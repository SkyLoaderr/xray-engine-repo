//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_ToolsCustom.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "ChoseForm.h"
#include "ui_main.h"
#include "PropertiesList.h"               
#include "motion.h"
#include "bone.h"
#include "library.h"
#include "fmesh.h"
#include "folderlib.h"

//------------------------------------------------------------------------------
CToolsCustom* Tools=0;
//------------------------------------------------------------------------------
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}

CToolsCustom::CToolsCustom()
{
    m_bReady			= false;
    m_Action			= etaSelect;
    m_Settings.assign	(etfNormalAlign|etfGSnap|etfOSnap|etfMTSnap|etfVSnap|etfASnap|etfMSnap);
    m_Axis				= etAxisZX;
    fFogness			= 0.9f;
    dwFogColor			= 0xffffffff;

    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);
}
//---------------------------------------------------------------------------

CToolsCustom::~CToolsCustom()        
{
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
}
//---------------------------------------------------------------------------

bool CToolsCustom::OnCreate()
{
    m_bReady 		= true;

	SetAction		(etaSelect);

    return true;
}

void CToolsCustom::OnDestroy()
{
	VERIFY(m_bReady);
    m_bReady			= false;
}
//---------------------------------------------------------------------------

void CToolsCustom::SetAction(ETAction action)
{
	switch(action){
    case etaSelect: m_bHiddenMode=false; break;
    case etaAdd:
    case etaMove:
    case etaRotate:
    case etaScale:  m_bHiddenMode=true; break;
    }
    m_Action = action;
    switch(m_Action){
        case etaSelect:  UI->GetD3DWindow()->Cursor = crCross;     break;
        case etaAdd:     UI->GetD3DWindow()->Cursor = crArrow;     break;
        case etaMove:    UI->GetD3DWindow()->Cursor = crSizeAll;   break;
        case etaRotate:  UI->GetD3DWindow()->Cursor = crSizeWE;    break;
        case etaScale:   UI->GetD3DWindow()->Cursor = crVSplit;    break;
        default:         UI->GetD3DWindow()->Cursor = crHelp;
    }
    UI->RedrawScene(); 
    ExecCommand(COMMAND_REFRESH_UI_BAR);
}

void CToolsCustom::SetAxis(ETAxis axis)
{
	m_Axis=axis;
    UI->RedrawScene();
    ExecCommand(COMMAND_REFRESH_UI_BAR);
}				

void CToolsCustom::SetSettings(u32 mask, BOOL val)
{
	m_Settings.set(mask,val);
    UI->RedrawScene();
    ExecCommand(COMMAND_REFRESH_UI_BAR);
}				


bool __fastcall CToolsCustom::MouseStart(TShiftState Shift)
{
	switch(m_Action){
    case etaSelect:	break;
    case etaAdd:	break;
    case etaMove:
        if (etAxisY==m_Axis){
            m_MoveXVector.set(0,0,0);
            m_MoveYVector.set(0,1,0);
        }else{
            m_MoveXVector.set( Device.m_Camera.GetRight() );
            m_MoveXVector.y = 0;
            m_MoveYVector.set( Device.m_Camera.GetDirection() );
            m_MoveYVector.y = 0;
            m_MoveXVector.normalize_safe();
            m_MoveYVector.normalize_safe();
        }
        m_MoveReminder.set(0,0,0);
    	m_MoveAmount.set(0,0,0);
    break;
    case etaRotate:
        m_RotateCenter.set(0,0,0);
        m_RotateVector.set(0,0,0);
        if (etAxisX==m_Axis) m_RotateVector.set(1,0,0);
        else if (etAxisY==m_Axis) m_RotateVector.set(0,1,0);
        else if (etAxisZ==m_Axis) m_RotateVector.set(0,0,1);
        m_fRotateSnapValue 	= 0;
		m_RotateAmount    	= 0;
	break;
    case etaScale:
		m_ScaleAmount.set	(0,0,0);
    break;
    }
	return m_bHiddenMode;
}

bool __fastcall CToolsCustom::MouseEnd(TShiftState Shift)
{
	switch(m_Action){
    case etaSelect: break;
    case etaAdd: 	break;
    case etaMove:	break;
    case etaRotate:	break;
    case etaScale:	break;
    }
	return true;
}

void __fastcall CToolsCustom::MouseMove(TShiftState Shift)
{
	switch(m_Action){
    case etaSelect: break;
    case etaAdd: 	break;
    case etaMove:{      
        m_MoveAmount.mul( m_MoveXVector, UI->m_MouseSM * UI->m_DeltaCpH.x );
        m_MoveAmount.mad( m_MoveYVector, -UI->m_MouseSM * UI->m_DeltaCpH.y );

        if( m_Settings.is(etfMSnap) ){
        	CHECK_SNAP(m_MoveReminder.x,m_MoveAmount.x,m_MoveSnap);
        	CHECK_SNAP(m_MoveReminder.y,m_MoveAmount.y,m_MoveSnap);
        	CHECK_SNAP(m_MoveReminder.z,m_MoveAmount.z,m_MoveSnap);
        }

        if (!(etAxisX==m_Axis)&&!(etAxisZX==m_Axis)) m_MoveAmount.x = 0.f;
        if (!(etAxisZ==m_Axis)&&!(etAxisZX==m_Axis)) m_MoveAmount.z = 0.f;
        if (!(etAxisY==m_Axis)) m_MoveAmount.y = 0.f;
    }break;
    case etaRotate:{
        m_RotateAmount = -UI->m_DeltaCpH.x * UI->m_MouseSR;
        if( m_Settings.is(etfASnap) ) CHECK_SNAP(m_fRotateSnapValue,m_RotateAmount,m_RotateSnapAngle);
    }break;
    case etaScale:{
        float dy = UI->m_DeltaCpH.x * UI->m_MouseSS;
        if (dy>1.f) dy=1.f; else if (dy<-1.f) dy=-1.f;

        m_ScaleAmount.set( dy, dy, dy );

        if (m_Settings.is(etfNUScale)){
            if (!(etAxisX==m_Axis)&&!(etAxisZX==m_Axis)) m_ScaleAmount.x = 0.f;
            if (!(etAxisZ==m_Axis)&&!(etAxisZX==m_Axis)) m_ScaleAmount.z = 0.f;
            if (!(etAxisY==m_Axis)) m_ScaleAmount.y = 0.f;
        }
    }break;
    }
}

void CToolsCustom::GetCurrentFog(u32& fog_color, float& s_fog, float& e_fog)
{
    s_fog				= psDeviceFlags.is(rsFog)?(1.0f - fFogness)* 0.85f * UI->ZFar():0.99f*UI->ZFar();
    e_fog				= psDeviceFlags.is(rsFog)?0.91f * UI->ZFar():UI->ZFar();
    fog_color 			= dwFogColor;
}

void CToolsCustom::RenderEnvironment()
{
}
