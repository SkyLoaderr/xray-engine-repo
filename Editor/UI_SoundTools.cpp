//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "ui_soundtools.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "sound.h"
#include "selectsound.h"
#include "addsound.h"
#include "movesound.h"
#include "ui_main.h"
#include "ui_tools.h"
//---------------------------------------------------------------------------
TUI_ControlSoundSelect    MSC_ControlSoundSelect;
TUI_ControlSoundAdd       MSC_ControlSoundAdd;
TUI_ControlSoundMove      MSC_ControlSoundMove;
TUI_ControlSoundRotate    MSC_ControlSoundRotate;
TUI_ControlSoundScale     MSC_ControlSoundScale;
//---------------------------------------------------------------------------
void InitMSCSound()
{
    UI.m_Tools->AddControlCB(etSound,eaSelect, &MSC_ControlSoundSelect);
    UI.m_Tools->AddControlCB(etSound,eaAdd,    &MSC_ControlSoundAdd);
    UI.m_Tools->AddControlCB(etSound,eaMove,   &MSC_ControlSoundMove);
    UI.m_Tools->AddControlCB(etSound,eaRotate, &MSC_ControlSoundRotate);
    UI.m_Tools->AddControlCB(etSound,eaScale,  &MSC_ControlSoundScale);
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlSoundSelect::TUI_ControlSoundSelect()
{
}

bool __fastcall TUI_ControlSoundSelect::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
    if (!Shift.Contains(ssCtrl)) Scene.SelectObjects( false, OBJCLASS_SOUND );

    SceneObject *obj = Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_SOUND);
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

void __fastcall TUI_ControlSoundSelect::Move(TShiftState _Shift)
{
    if (bBoxSelection) UI.UpdateSelectionRect(&UI.m_StartCp,&UI.m_CurrentCp);
}

bool __fastcall TUI_ControlSoundSelect::End(TShiftState _Shift)
{
    if (bBoxSelection){
        UI.EnableSelectionRect( false );
        ICullPlane planes[4];
        if( UI.MouseBox( planes,&UI.m_StartCp,&UI.m_CurrentCp ) )
            Scene.BoxPickSelect( planes,
                OBJCLASS_SOUND,
                true );
    }
    return true;
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlSoundMove::TUI_ControlSoundMove()
{
    mX = 0.1f; mY = -0.1f;
}

bool __fastcall TUI_ControlSoundMove::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
	if( Scene.SelectionCount( OBJCLASS_SOUND, true ) == 0 )
		return false;

	Scene.UndoPrepare();

    if (fraMoveSound->sbLockY->Down){
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

void __fastcall TUI_ControlSoundMove::Move(TShiftState _Shift)
{
    if (_Shift.Contains(ssLeft)||_Shift.Contains(ssRight)){
        Fvector amount;
        amount.mul( m_XVector, mX * UI.m_DeltaCpH.x );
        amount.translate( m_YVector, mY * UI.m_DeltaCpH.y );

        if( fraMoveSound->sbSnap->Down ){
            amount.x = snapto( amount.x, UI.pivotsnap().x );
            amount.y = snapto( amount.y, UI.pivotsnap().y );
            amount.z = snapto( amount.z, UI.pivotsnap().z );
        }

        if (!fraMoveSound->sbLockX->Down) amount.m[0] = 0.f;
        if (!fraMoveSound->sbLockY->Down) amount.m[1] = 0.f;
        if (!fraMoveSound->sbLockZ->Down) amount.m[2] = 0.f;

        ObjectIt _F = Scene.FirstObj();
        for(;_F!=Scene.LastObj();_F++)
            if( (*_F)->ClassID() == OBJCLASS_SOUND && (*_F)->Visible() )
                if( (*_F)->Selected() )
                    (*_F)->Move( amount );
    }
}

bool TUI_ControlSoundMove::KeyDown(WORD Key, TShiftState Shift)
{
    if (Key=='X'||Key=='x'){ fraMoveSound->sbLockX->Down = true; fraMoveSound->sbLockClick(fraMoveSound->sbLockX); return true;}
    if (Key=='Y'||Key=='y'){ fraMoveSound->sbLockY->Down = true; fraMoveSound->sbLockClick(fraMoveSound->sbLockY); return true;}
    if (Key=='Z'||Key=='z'){ fraMoveSound->sbLockZ->Down = true; fraMoveSound->sbLockClick(fraMoveSound->sbLockZ); return true;}
    return false;
}

bool __fastcall TUI_ControlSoundMove::End(TShiftState _Shift)
{
    return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlSoundAdd::TUI_ControlSoundAdd()
{
}
bool __fastcall TUI_ControlSoundAdd::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
	Fvector p;
	if( UI.PickGround(&p,&UI.m_CurrentRStart,&UI.m_CurrentRNorm) ){
		char namebuffer[MAX_OBJ_NAME];
		Scene.GenObjectName( namebuffer );

		Scene.UndoPrepare();
		CSound *sound = new CSound( namebuffer );

		if( fraAddSound->sbSnap->Down ){
			p.x = snapto( p.x, Scene.lx()/10.f );
			p.z = snapto( p.z, Scene.lz()/10.f );
			p.y = 0;
		}

		sound->Move( p );
		Scene.AddObject( sound );

        Scene.SelectObjects(false,OBJCLASS_SOUND);
        sound->Select(true);
        if (Shift.Contains(ssCtrl)) fraSelectSound->RunEditor();
        if (!Shift.Contains(ssAlt)) ResetActionToSelect();
	}
    return false;
}

void __fastcall TUI_ControlSoundAdd::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlSoundAdd::End(TShiftState _Shift)
{
    return true;
}
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlSoundRotate::TUI_ControlSoundRotate()
{
}
bool __fastcall TUI_ControlSoundRotate::Start(TShiftState Shift)
{
    return false;
}

void __fastcall TUI_ControlSoundRotate::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlSoundRotate::End(TShiftState _Shift)
{
    return true;
}
//------------------------------------------------------------------------------
__fastcall TUI_ControlSoundScale::TUI_ControlSoundScale()
{
}
bool __fastcall TUI_ControlSoundScale::Start(TShiftState Shift)
{
    return false;
}

void __fastcall TUI_ControlSoundScale::Move(TShiftState _Shift)
{
}
bool __fastcall TUI_ControlSoundScale::End(TShiftState _Shift)
{
    return true;
}
//------------------------------------------------------------------------------



