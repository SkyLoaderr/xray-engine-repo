//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "ui_lighttools.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "light.h"
#include "selectlight.h"
#include "addlight.h"
#include "movelight.h"
#include "ui_main.h"
#include "ui_tools.h"
//---------------------------------------------------------------------------
TUI_ControlLightSelect    MSC_ControlLightSelect;
TUI_ControlLightAdd       MSC_ControlLightAdd;
TUI_ControlLightMove      MSC_ControlLightMove;
TUI_ControlLightRotate    MSC_ControlLightRotate;
TUI_ControlLightScale     MSC_ControlLightScale;
//---------------------------------------------------------------------------
void InitMSCLight()
{
    UI.m_Tools->AddControlCB(etLight,eaSelect, &MSC_ControlLightSelect);
    UI.m_Tools->AddControlCB(etLight,eaAdd,    &MSC_ControlLightAdd);
    UI.m_Tools->AddControlCB(etLight,eaMove,   &MSC_ControlLightMove);
    UI.m_Tools->AddControlCB(etLight,eaRotate, &MSC_ControlLightRotate);
    UI.m_Tools->AddControlCB(etLight,eaScale,  &MSC_ControlLightScale);
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlLightSelect::TUI_ControlLightSelect()
{
}

bool __fastcall TUI_ControlLightSelect::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
    if (!Shift.Contains(ssCtrl)) Scene.SelectObjects( false, OBJCLASS_LIGHT );

    SceneObject *obj = Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_LIGHT);
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

void __fastcall TUI_ControlLightSelect::Move(TShiftState _Shift)
{
    if (bBoxSelection) UI.UpdateSelectionRect(&UI.m_StartCp,&UI.m_CurrentCp);
}

bool __fastcall TUI_ControlLightSelect::End(TShiftState _Shift)
{
    if (bBoxSelection){
        UI.EnableSelectionRect( false );
        ICullPlane planes[4];
        if( UI.MouseBox( planes,&UI.m_StartCp,&UI.m_CurrentCp ) )
            Scene.BoxPickSelect( planes,
                OBJCLASS_LIGHT,
                true );
    }
    return true;
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlLightMove::TUI_ControlLightMove()
{
    mX = 0.1f; mY = -0.1f;
}

bool __fastcall TUI_ControlLightMove::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
	if( Scene.SelectionCount( OBJCLASS_LIGHT, true ) == 0 )
		return false;

	Scene.UndoPrepare();

    if (fraMoveLight->sbLockY->Down){
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

void __fastcall TUI_ControlLightMove::Move(TShiftState _Shift)
{
    if (_Shift.Contains(ssLeft)||_Shift.Contains(ssRight)){
        Fvector amount;
        amount.mul( m_XVector, mX * UI.m_DeltaCpH.x );
        amount.translate( m_YVector, mY * UI.m_DeltaCpH.y );

        if( fraMoveLight->sbSnap->Down ){
            amount.x = snapto( amount.x, UI.pivotsnap().x );
            amount.y = snapto( amount.y, UI.pivotsnap().y );
            amount.z = snapto( amount.z, UI.pivotsnap().z );
        }

        if (!fraMoveLight->sbLockX->Down) amount.m[0] = 0.f;
        if (!fraMoveLight->sbLockY->Down) amount.m[1] = 0.f;
        if (!fraMoveLight->sbLockZ->Down) amount.m[2] = 0.f;

        ObjectIt _F = Scene.FirstObj();
        for(;_F!=Scene.LastObj();_F++)
            if( (*_F)->ClassID() == OBJCLASS_LIGHT && (*_F)->Visible() )
                if( (*_F)->Selected() )
                    (*_F)->Move( amount );
    }
}

bool TUI_ControlLightMove::KeyDown(WORD Key, TShiftState Shift)
{
    if (Key=='X'||Key=='x'){ fraMoveLight->sbLockX->Down = true; fraMoveLight->sbLockClick(fraMoveLight->sbLockX); return true;}
    if (Key=='Y'||Key=='y'){ fraMoveLight->sbLockY->Down = true; fraMoveLight->sbLockClick(fraMoveLight->sbLockY); return true;}
    if (Key=='Z'||Key=='z'){ fraMoveLight->sbLockZ->Down = true; fraMoveLight->sbLockClick(fraMoveLight->sbLockZ); return true;}
    return false;
}

bool __fastcall TUI_ControlLightMove::End(TShiftState _Shift)
{
    return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlLightAdd::TUI_ControlLightAdd()
{
}
bool __fastcall TUI_ControlLightAdd::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
	Fvector p;
	if( UI.PickGround(&p,&UI.m_CurrentRStart,&UI.m_CurrentRNorm) ){
		char namebuffer[MAX_OBJ_NAME];
		Scene.GenObjectName( namebuffer );

		Scene.UndoPrepare();
		Light *light = new Light( namebuffer );

		if( fraAddLight->sbSnap->Down ){
			p.x = snapto( p.x, Scene.lx()/10.f );
			p.z = snapto( p.z, Scene.lz()/10.f );
			p.y = 0;
		}

		light->Move( p );
		Scene.AddObject( light );

        Scene.SelectObjects(false,OBJCLASS_LIGHT);
        light->Select(true);
        if (Shift.Contains(ssCtrl)) fraSelectLight->RunEditor();
        if (!Shift.Contains(ssAlt)) ResetActionToSelect();
	}
    return false;
}

void __fastcall TUI_ControlLightAdd::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlLightAdd::End(TShiftState _Shift)
{
    return true;
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlLightRotate::TUI_ControlLightRotate()
{
}
bool __fastcall TUI_ControlLightRotate::Start(TShiftState Shift)
{
    return false;
}

void __fastcall TUI_ControlLightRotate::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlLightRotate::End(TShiftState _Shift)
{
    return true;
}
//------------------------------------------------------------------------------
__fastcall TUI_ControlLightScale::TUI_ControlLightScale()
{
}
bool __fastcall TUI_ControlLightScale::Start(TShiftState Shift)
{
    return false;
}

void __fastcall TUI_ControlLightScale::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlLightScale::End(TShiftState _Shift)
{
    return true;
}
//------------------------------------------------------------------------------



