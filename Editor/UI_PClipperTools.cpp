#include "pch.h"
#pragma hdrstop

#include "UI_PClipperTools.h"
#include "ui_tools.h"
#include "library.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "SelectPClipper.h"
#include "AddPClipper.h"
#include "MovePClipper.h"
#include "RotatePClipper.h"
#include "ScalePClipper.h"
#include "ui_main.h"
#include "pclipper.h"

//----------------------------------------------------------------------
TUI_ControlPClipperSelect    MSC_ControlPClipperSelect;
TUI_ControlPClipperAdd       MSC_ControlPClipperAdd;
TUI_ControlPClipperMove      MSC_ControlPClipperMove;
TUI_ControlPClipperRotate    MSC_ControlPClipperRotate;
TUI_ControlPClipperScale     MSC_ControlPClipperScale;
//---------------------------------------------------------------------------
void InitMSCPClipper()
{
    UI.m_Tools->AddControlCB(etPClipper,eaSelect, &MSC_ControlPClipperSelect);
    UI.m_Tools->AddControlCB(etPClipper,eaAdd,    &MSC_ControlPClipperAdd);
    UI.m_Tools->AddControlCB(etPClipper,eaMove,   &MSC_ControlPClipperMove);
    UI.m_Tools->AddControlCB(etPClipper,eaRotate, &MSC_ControlPClipperRotate);
    UI.m_Tools->AddControlCB(etPClipper,eaScale,  &MSC_ControlPClipperScale);
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlPClipperSelect::TUI_ControlPClipperSelect()
{
}

bool __fastcall TUI_ControlPClipperSelect::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
    if (!Shift.Contains(ssCtrl)) Scene.SelectObjects( false, OBJCLASS_PCLIPPER );

    SceneObject *obj = Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_PCLIPPER);
    bBoxSelection    = (obj && Shift.Contains(ssCtrl)) || !obj;

    if( bBoxSelection ){
        UI.EnableSelectionRect( true );
        UI.UpdateSelectionRect(&UI.m_StartCp,&UI.m_CurrentCp);
        if( obj ) obj->Select( (obj->Selected())?false:true );// true - select
        return true;
    } else {
        if( obj ) obj->Select( (obj->Selected())?false:true );// true - select
    }
    return false;
}

void __fastcall TUI_ControlPClipperSelect::Move(TShiftState _Shift)
{
    if (bBoxSelection) UI.UpdateSelectionRect(&UI.m_StartCp,&UI.m_CurrentCp);
}

bool __fastcall TUI_ControlPClipperSelect::End(TShiftState _Shift)
{
    if (bBoxSelection){
        UI.EnableSelectionRect( false );
        ICullPlane planes[4];
        if( UI.MouseBox( planes,&UI.m_StartCp,&UI.m_CurrentCp ) )
            Scene.BoxPickSelect( planes,
                OBJCLASS_PCLIPPER,
                true );
    }
    return true;
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlPClipperMove::TUI_ControlPClipperMove()
{
    mX = 0.1f; mY = -0.1f;
}

bool __fastcall TUI_ControlPClipperMove::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
    if( Scene.SelectionCount( OBJCLASS_PCLIPPER, true ) == 0 ) return false;

    Scene.UndoPrepare();

    if (fraMovePClipper->sbLockY->Down){
		m_XVector.set(0,0,0);
		m_YVector.set(0,1,0);
	}else{
		m_XVector.set( UI.camera().i );
		m_XVector.y = 0;
		m_YVector.set( UI.camera().k );
		m_YVector.y = 0;
		m_XVector.normalize_safe();
		m_YVector.normalize_safe();
	}
    return true;
}

void __fastcall TUI_ControlPClipperMove::Move(TShiftState _Shift)
{
    if (_Shift.Contains(ssLeft)||_Shift.Contains(ssRight)){
        Fvector amount;
        amount.mul( m_XVector, mX * UI.m_DeltaCpH.x );
        amount.translate( m_YVector, mY * UI.m_DeltaCpH.y );

        if( fraMovePClipper->sbSnap->Down ){
            amount.x = snapto( amount.x, UI.pivotsnap().x );
            amount.y = snapto( amount.y, UI.pivotsnap().y );
            amount.z = snapto( amount.z, UI.pivotsnap().z );
        }

        if (!fraMovePClipper->sbLockX->Down) amount.m[0] = 0.f;
        if (!fraMovePClipper->sbLockY->Down) amount.m[1] = 0.f;
        if (!fraMovePClipper->sbLockZ->Down) amount.m[2] = 0.f;

        ObjectIt _F = Scene.FirstObj();
        for(;_F!=Scene.LastObj();_F++)
            if( (*_F)->ClassID() == OBJCLASS_PCLIPPER && (*_F)->Visible() )
                if( (*_F)->Selected() )
                    (*_F)->Move( amount );
    }
}

bool TUI_ControlPClipperMove::KeyDown(WORD Key, TShiftState Shift)
{
    if (Key=='X'||Key=='x'){ fraMovePClipper->sbLockX->Down = true; fraMovePClipper->sbLockClick(fraMovePClipper->sbLockX); return true;}
    if (Key=='Y'||Key=='y'){ fraMovePClipper->sbLockY->Down = true; fraMovePClipper->sbLockClick(fraMovePClipper->sbLockY); return true;}
    if (Key=='Z'||Key=='z'){ fraMovePClipper->sbLockZ->Down = true; fraMovePClipper->sbLockClick(fraMovePClipper->sbLockZ); return true;}
    return false;
}

bool __fastcall TUI_ControlPClipperMove::End(TShiftState _Shift)
{
    return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlPClipperAdd::TUI_ControlPClipperAdd()
{
    cur_point=-1;
    l = 0;
}
void TUI_ControlPClipperAdd::CancelAdd()
{
    Scene.RemoveObject(l);
    SAFE_DELETE( l );
    Reset();
}
void TUI_ControlPClipperAdd::Reset()
{
    cur_point = -1;
    l = 0;
    UI.MouseMultiClickCapture(false);
}
bool TUI_ControlPClipperAdd::KeyDown(WORD Key, TShiftState Shift)
{
    if ((Key==VK_ESCAPE)&&(l)){
        CancelAdd();
        return true;
    }
    return false;
}
void TUI_ControlPClipperAdd::OnEnter()
{
    bActionComplete = true;
}
void TUI_ControlPClipperAdd::OnExit ()
{
    if (!bActionComplete)   CancelAdd();
    else                    Reset();
    UI.UpdateScene();
}
bool __fastcall TUI_ControlPClipperAdd::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
	Fvector p;
    if (fraAddPClipper->cbFixed->Checked){
  	    if( UI.PickGround(&p,&UI.m_CurrentRStart,&UI.m_CurrentRNorm) ){
            Fvector vscale;
            vscale.set( fraAddPClipper->seSizeW->Value, fraAddPClipper->seSizeH->Value, fraAddPClipper->seSizeW->Value );

            char namebuffer[MAX_OBJ_NAME];
            Scene.GenObjectName( namebuffer );

            Scene.UndoPrepare();
            l = new PClipper(namebuffer);
            l -> LocalScale( vscale );

            if( fraAddPClipper->sbSnap->Down ){
                p.x = snapto( p.x, Scene.lx()/10.f );
                p.z = snapto( p.z, Scene.lz()/10.f );
                p.y = 0.f;
            }
            l -> Move( p );
            Scene.AddObject( l );
            Scene.SelectObjects(false,OBJCLASS_PCLIPPER);
            l->Select(true);
            l = 0;
            if (Shift.Contains(ssCtrl)) fraSelectPClipper->RunEditor();
            if (!Shift.Contains(ssShift)) ResetActionToSelect();
            bActionComplete = true;
        }
	}else{
        SPickInfo pinf;
        bool bPickObject, bPickGround;
        bPickObject = !!Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SOBJECT2, &pinf);
        if (!bPickObject) bPickGround = UI.PickGround(&pinf.pt,&UI.m_CurrentRStart,&UI.m_CurrentRNorm);
        if (bPickObject||bPickGround){
            if (cur_point==-1){
                char namebuffer[MAX_OBJ_NAME];
                Scene.GenObjectName( namebuffer );
                Scene.UndoPrepare();
                VERIFY(l==0); // не должно быть объекта
                l = new PClipper(namebuffer);
                Scene.AddObject( l );
                Scene.SelectObjects(false,OBJCLASS_PCLIPPER);
                l->Select(true);
                UI.MouseMultiClickCapture(true);
                bActionComplete = false;
            }
        }
        cur_point++;
        if (cur_point==3){
            bActionComplete = true;
            Reset();
            if (Shift.Contains(ssCtrl)) fraSelectPClipper->RunEditor();
            if (!Shift.Contains(ssAlt)) ResetActionToSelect();
            return true;
        }
//        return true;
    }

	return false;
}

void __fastcall TUI_ControlPClipperAdd::Move(TShiftState _Shift)
{
    if (!fraAddPClipper->cbFixed->Checked){
        SPickInfo pinf;
        bool bPickObject, bPickGround;
        bPickObject = !!Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SOBJECT2, &pinf);
        if (!bPickObject) bPickGround = UI.PickGround(&pinf.pt,&UI.m_CurrentRStart,&UI.m_CurrentRNorm);
        if (bPickGround || bPickObject){
            Fvector p;
            p.set(pinf.pt);
            float d = pinf.rp_inf.range;
            if (bPickGround) d = UI.m_CurrentRStart.distance_to(p);
            if (fraAddPClipper->sbSnap->Down){
                if (bPickObject){
                    Fvector pn;
                    float u = pinf.rp_inf.u;
                    float v = pinf.rp_inf.v;
                    float w = 1-(u+v);
                    if ((w>u) && (w>v)) pn.set(pinf.rp_inf.p1);
                    else if ((u>w) && (u>v)) pn.set(pinf.rp_inf.p2);
                    else pn.set(pinf.rp_inf.p3);
                    if (pn.distance_to(pinf.pt) < UI.movesnap()) p.set(pn);
                }else if (bPickGround){
                    p.x = snapto( p.x, Scene.lx()/10.f );
                    p.z = snapto( p.z, Scene.lz()/10.f );
                    p.y = 0.f;
                }
            }
            l->SetPlane(cur_point, p, d);
        }
    }
}

bool __fastcall TUI_ControlPClipperAdd::End(TShiftState Shift)
{
    if (!fraAddPClipper->cbFixed->Checked){
    }
    return false;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlPClipperRotate::TUI_ControlPClipperRotate()
{
}
bool __fastcall TUI_ControlPClipperRotate::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
    if( Scene.SelectionCount( OBJCLASS_PCLIPPER, true ) == 0 ) return false;
    Scene.UndoPrepare();
    m_RCenter.set( UI.pivot() );
    if (fraRotatePClipper->sbAxisX->Down) m_RVector.set(1,0,0);
    else if (fraRotatePClipper->sbAxisY->Down) m_RVector.set(0,1,0);
    else if (fraRotatePClipper->sbAxisZ->Down) m_RVector.set(0,0,1);
    return true;
}

void __fastcall TUI_ControlPClipperRotate::Move(TShiftState _Shift)
{
    if (_Shift.Contains(ssLeft)){
        float amount = UI.m_DeltaCpH.y * 0.025f;

        if( fraRotatePClipper->sbASnap->Down ){
            amount = snapto( amount, UI.anglesnap() );
        }

        ObjectIt _F = Scene.FirstObj();
        for(;_F!=Scene.LastObj();_F++)
            if( (*_F)->ClassID() == OBJCLASS_PCLIPPER && (*_F)->Visible() )
                if( (*_F)->Selected() ){
                    if( fraRotatePClipper->sbCSLocal->Down ){
                        (*_F)->LocalRotate( m_RVector, amount );
                    } else {
                        (*_F)->Rotate( m_RCenter, m_RVector, amount );
                    }
                }
    }
}
bool __fastcall TUI_ControlPClipperRotate::End(TShiftState _Shift)
{
    return true;
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlPClipperScale::TUI_ControlPClipperScale()
{
	m_Center.set(0,0,0);
}
bool __fastcall TUI_ControlPClipperScale::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
	if( Scene.SelectionCount( OBJCLASS_PCLIPPER, true ) == 0 )
		return false;
	Scene.UndoPrepare();
	m_Center.set( UI.pivot() );
	return true;
}

void __fastcall TUI_ControlPClipperScale::Move(TShiftState _Shift)
{
	float dy = - UI.m_DeltaCpH.y * 0.025f;
	Fvector amount;
    amount.set(1,1,1);
    if (fraScalePClipper->sbAxisW->Down){ amount.x = 1.f+dy; amount.z = 1.f+dy;}
    if (fraScalePClipper->sbAxisH->Down) amount.y = 1.f+dy;

	ObjectIt _F = Scene.FirstObj();
	for(;_F!=Scene.LastObj();_F++)
		if( (*_F)->ClassID() == OBJCLASS_PCLIPPER && (*_F)->Visible() )
			if( (*_F)->Selected() ){
				if( fraScalePClipper->sbCSLocal->Down ){
					(*_F)->LocalScale( amount );
				} else {
					(*_F)->Scale( m_Center, amount );
				}
			}
}
bool __fastcall TUI_ControlPClipperScale::End(TShiftState _Shift)
{
    return true;
}
//------------------------------------------------------------------------------



