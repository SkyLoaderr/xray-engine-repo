//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "UI_ObjectTools.h"
#include "ui_tools.h"
#include "library.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "SObject2.h"
#include "SelectObject.h"
#include "AddObject.h"
#include "MoveObject.h"
#include "RotateObject.h"
#include "ui_main.h"

//----------------------------------------------------------------------
TUI_ControlObjectSelect    MSC_ControlObjectSelect;
TUI_ControlObjectAdd       MSC_ControlObjectAdd;
TUI_ControlObjectMove      MSC_ControlObjectMove;
TUI_ControlObjectRotate    MSC_ControlObjectRotate;
TUI_ControlObjectScale     MSC_ControlObjectScale;
//---------------------------------------------------------------------------
void InitMSCObject()
{
    UI.m_Tools->AddControlCB(etObject,eaSelect, &MSC_ControlObjectSelect);
    UI.m_Tools->AddControlCB(etObject,eaAdd,    &MSC_ControlObjectAdd);
    UI.m_Tools->AddControlCB(etObject,eaMove,   &MSC_ControlObjectMove);
    UI.m_Tools->AddControlCB(etObject,eaRotate, &MSC_ControlObjectRotate);
    UI.m_Tools->AddControlCB(etObject,eaScale,  &MSC_ControlObjectScale);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlObjectSelect::TUI_ControlObjectSelect()
{
}

bool __fastcall TUI_ControlObjectSelect::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
    if (!Shift.Contains(ssCtrl)) Scene.SelectObjects( false, OBJCLASS_SOBJECT2 );

    SceneObject *obj = Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SOBJECT2);
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

void __fastcall TUI_ControlObjectSelect::Move(TShiftState _Shift)
{
    if (bBoxSelection) UI.UpdateSelectionRect(&UI.m_StartCp,&UI.m_CurrentCp);
}

bool __fastcall TUI_ControlObjectSelect::End(TShiftState _Shift)
{
    if (bBoxSelection){
        UI.EnableSelectionRect( false );
        ICullPlane planes[4];
        if( UI.MouseBox( planes,&UI.m_StartCp,&UI.m_CurrentCp ) )
            Scene.BoxPickSelect( planes,
                OBJCLASS_SOBJECT2,
                true );
    }
    return true;
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlObjectMove::TUI_ControlObjectMove()
{
    mX = 0.1f; mY = -0.1f;
}

bool __fastcall TUI_ControlObjectMove::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
    if( Scene.SelectionCount( OBJCLASS_SOBJECT2, true ) == 0 ) return false;

    Scene.UndoPrepare();

    if (fraMoveObject->sbLockY->Down){
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

void __fastcall TUI_ControlObjectMove::Move(TShiftState _Shift)
{
    if (_Shift.Contains(ssLeft)||_Shift.Contains(ssRight)){
        Fvector amount;
        amount.mul( m_XVector, mX * UI.m_DeltaCpH.x );
        amount.translate( m_YVector, mY * UI.m_DeltaCpH.y );

        if( fraMoveObject->sbSnap->Down ){
            amount.x = snapto( amount.x, UI.pivotsnap().x );
            amount.y = snapto( amount.y, UI.pivotsnap().y );
            amount.z = snapto( amount.z, UI.pivotsnap().z );
        }

        if (!fraMoveObject->sbLockX->Down) amount.m[0] = 0.f;
        if (!fraMoveObject->sbLockY->Down) amount.m[1] = 0.f;
        if (!fraMoveObject->sbLockZ->Down) amount.m[2] = 0.f;

        ObjectIt _F = Scene.FirstObj();
        for(;_F!=Scene.LastObj();_F++)
            if( (*_F)->ClassID() == OBJCLASS_SOBJECT2 && (*_F)->Visible() )
                if( (*_F)->Selected() )
                    (*_F)->Move( amount );
    }
}

bool TUI_ControlObjectMove::KeyDown(WORD Key, TShiftState Shift)
{
    if (Key=='X'||Key=='x'){ fraMoveObject->sbLockX->Down = true; fraMoveObject->sbLockClick(fraMoveObject->sbLockX); return true;}
    if (Key=='Y'||Key=='y'){ fraMoveObject->sbLockY->Down = true; fraMoveObject->sbLockClick(fraMoveObject->sbLockY); return true;}
    if (Key=='Z'||Key=='z'){ fraMoveObject->sbLockZ->Down = true; fraMoveObject->sbLockClick(fraMoveObject->sbLockZ); return true;}
    return false;
}

bool __fastcall TUI_ControlObjectMove::End(TShiftState _Shift)
{
    return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlObjectAdd::TUI_ControlObjectAdd()
{
}
bool __fastcall TUI_ControlObjectAdd::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
    if(!Lib.o()) fraAddObject->sbSelectClick(0);
	if( Lib.o()!=0 ){
		Fvector p;
  	    if( UI.PickGround(&p,&UI.m_CurrentRStart,&UI.m_CurrentRNorm) ){
			char namebuffer[MAX_OBJ_NAME];
			Scene.GenObjectName( namebuffer );

			Scene.UndoPrepare();
			SObject2 *obj = new SObject2( namebuffer );

			if( fraAddObject->sbReference->Down ){
				obj->LibReference( Lib.o() );
			} else {
				obj->CloneFromLib( Lib.o() );
			}

			if( fraAddObject->sbQSnap->Down ){
				p.x = snapto( p.x, Scene.lx()/2.f );
				p.z = snapto( p.z, Scene.lz()/2.f );
				p.y = 0;
			} else if( fraAddObject->sbSnap->Down ){
				p.x = snapto( p.x, Scene.lx()/10.f );
				p.z = snapto( p.z, Scene.lz()/10.f );
				p.y = 0;
			}

			obj->Move( p );
			Scene.AddObject( obj );

            Scene.SelectObjects(false,OBJCLASS_SOBJECT2);
            obj->Select(true);
            if (Shift.Contains(ssCtrl)) fraSelectObject->RunEditor();
            if (!Shift.Contains(ssAlt)) ResetActionToSelect();
		}
	}
    return false;
}

void __fastcall TUI_ControlObjectAdd::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlObjectAdd::End(TShiftState _Shift)
{
    return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlObjectRotate::TUI_ControlObjectRotate()
{
}
bool __fastcall TUI_ControlObjectRotate::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
    if( Scene.SelectionCount( OBJCLASS_SOBJECT2, true ) == 0 ) return false;
    Scene.UndoPrepare();
    m_RCenter.set( UI.pivot() );
    if (fraRotateObject->sbAxisX->Down) m_RVector.set(1,0,0);
    else if (fraRotateObject->sbAxisY->Down) m_RVector.set(0,1,0);
    else if (fraRotateObject->sbAxisZ->Down) m_RVector.set(0,0,1);
    return true;
}

void __fastcall TUI_ControlObjectRotate::Move(TShiftState _Shift)
{
    if (_Shift.Contains(ssLeft)){
        float amount = UI.m_DeltaCpH.y * 0.025f;

        if( fraRotateObject->sbASnap->Down ){
            amount = snapto( amount, UI.anglesnap() );
        }

        ObjectIt _F = Scene.FirstObj();
        for(;_F!=Scene.LastObj();_F++)
            if( (*_F)->ClassID() == OBJCLASS_SOBJECT2 && (*_F)->Visible() )
                if( (*_F)->Selected() ){
                    if( fraRotateObject->sbCSLocal->Down ){
                        (*_F)->LocalRotate( m_RVector, amount );
                    } else {
                        (*_F)->Rotate( m_RCenter, m_RVector, amount );
                    }
                }
    }
}
bool __fastcall TUI_ControlObjectRotate::End(TShiftState _Shift)
{
    return true;
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlObjectScale::TUI_ControlObjectScale()
{
}
bool __fastcall TUI_ControlObjectScale::Start(TShiftState Shift)
{
    return false;
}

void __fastcall TUI_ControlObjectScale::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlObjectScale::End(TShiftState _Shift)
{
    return true;
}
//------------------------------------------------------------------------------



