//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "ui_scenetools.h"
#include "scene.h"
#include "ui_main.h"
#include "movescene.h"
#include "ui_tools.h"

TUI_ControlSceneSelect  MSC_SceneSelect;
TUI_ControlSceneMove    MSC_SceneMove;

void InitMSCScene()
{
    UI.m_Tools->AddControlCB(etScene,eaSelect,   &MSC_SceneSelect);
    UI.m_Tools->AddControlCB(etScene,eaMove,     &MSC_SceneMove);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlSceneSelect::TUI_ControlSceneSelect()
{
}

bool __fastcall TUI_ControlSceneSelect::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
    if (!Shift.Contains(ssCtrl)) Scene.SelectObjects( false, OBJCLASS_NONE );

    SceneObject *obj = Scene.RTL_Pick( UI.m_CurrentRStart,UI.m_CurrentRNorm, OBJCLASS_NONE);
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

void __fastcall TUI_ControlSceneSelect::Move(TShiftState Shift)
{
    if (bBoxSelection) UI.UpdateSelectionRect(&UI.m_StartCp,&UI.m_CurrentCp);
}

bool __fastcall TUI_ControlSceneSelect::End(TShiftState Shift)
{
    if (bBoxSelection){
        UI.EnableSelectionRect( false );
        ICullPlane planes[4];
        if( UI.MouseBox( planes,&UI.m_StartCp,&UI.m_CurrentCp ) )
            Scene.BoxPickSelect( planes, -1, true );
    }
    return true;
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
__fastcall TUI_ControlSceneMove::TUI_ControlSceneMove()
{
    mX = 0.1f; mY = -0.1f;
}
bool __fastcall TUI_ControlSceneMove::Start(TShiftState Shift)
{
    if (Shift.Contains(ssRight)) return false;
    if( Scene.SelectionCount( OBJCLASS_NONE, true ) == 0 )
        return false;

    Scene.UndoPrepare();

    if (fraMoveScene->sbLockY->Down){
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

void __fastcall TUI_ControlSceneMove::Move(TShiftState _Shift)
{
    if (_Shift.Contains(ssLeft)||_Shift.Contains(ssRight)){
        Fvector amount;
        amount.mul( m_XVector, mX * UI.m_DeltaCpH.x );
        amount.translate( m_YVector, mY * UI.m_DeltaCpH.y );

        if( fraMoveScene->sbSnap->Down ){
            amount.x = snapto( amount.x, UI.pivotsnap().x );
            amount.y = snapto( amount.y, UI.pivotsnap().y );
            amount.z = snapto( amount.z, UI.pivotsnap().z );
        }

        if (!fraMoveScene->sbLockX->Down) amount.m[0] = 0.f;
        if (!fraMoveScene->sbLockY->Down) amount.m[1] = 0.f;
        if (!fraMoveScene->sbLockZ->Down) amount.m[2] = 0.f;

        ObjectIt _F = Scene.FirstObj();
        for(;_F!=Scene.LastObj();_F++)
            if( (*_F)->Visible() && (*_F)->Selected() ) (*_F)->Move( amount );
    }
}
bool __fastcall TUI_ControlSceneMove::End(TShiftState _Shift)
{
    return true;
}

bool TUI_ControlSceneMove::KeyDown(WORD Key, TShiftState Shift)
{
    if (Key=='X'||Key=='x'){ fraMoveScene->sbLockX->Down = true; fraMoveScene->sbLockClick(fraMoveScene->sbLockX); return true;}
    if (Key=='Y'||Key=='y'){ fraMoveScene->sbLockY->Down = true; fraMoveScene->sbLockClick(fraMoveScene->sbLockY); return true;}
    if (Key=='Z'||Key=='z'){ fraMoveScene->sbLockZ->Down = true; fraMoveScene->sbLockClick(fraMoveScene->sbLockZ); return true;}
    return false;
}

bool TUI_ControlSceneMove::KeyUp  (WORD Key, TShiftState Shift)
{
    return false;
}
bool TUI_ControlSceneMove::KeyPress(WORD Key, TShiftState Shift)
{
/*
    Fvector vr;
    Fvector vmove;
    Fmatrix rmatrix;

    vr.set( 0, 1, 0 );

    dt = 0.001f * ( timeGetTime() - last_t );
    if (dt>0.1f) dt = 0.1f;
    last_t = timeGetTime();

    switch(Key){
    case VK_RIGHT:
        rmatrix.rotation( vr, dt * M_PI * 0.1f );
        rmatrix.transform( UI.m_Camera.i );
        rmatrix.transform( UI.m_Camera.j );
        rmatrix.transform( UI.m_Camera.k );
        UI.UpdateScene();
        break;
    case VK_LEFT:
        rmatrix.rotation( vr, - dt * M_PI * 0.1f );
        rmatrix.transform( UI.m_Camera.i );
        rmatrix.transform( UI.m_Camera.j );
        rmatrix.transform( UI.m_Camera.k );
        UI.UpdateScene();
        break;
    case VK_UP:
        rmatrix.rotation( UI.m_Camera.i, -dt * M_PI * 0.1f );
        rmatrix.transform( UI.m_Camera.i );
        rmatrix.transform( UI.m_Camera.j );
        rmatrix.transform( UI.m_Camera.k );
        UI.UpdateScene();
        break;
    case VK_DOWN:
        rmatrix.rotation( UI.m_Camera.i, dt * M_PI * 0.1f);
        rmatrix.transform( UI.m_Camera.i );
        rmatrix.transform( UI.m_Camera.j );
        rmatrix.transform( UI.m_Camera.k );
        UI.UpdateScene();
        break;
    case 'A':
        if (Shift.Contains(ssCtrl)){
            vmove.set( 0, 1, 0 );
            vmove.mul( dt * 5.f );
            UI.m_Camera.c.add( vmove );
            UI.UpdateScene();
        }else{
            vmove.set( UI.m_Camera.k );
            vmove.y = 0;
            vmove.normalize_safe();
            vmove.mul( dt * 5.f );
            UI.m_Camera.c.add( vmove );
            UI.UpdateScene();
        }
        break;
    case 'Z':
        if (Shift.Contains(ssCtrl)){
            vmove.set( 0, 1, 0 );
            vmove.mul( -dt * 5.f );
            UI.m_Camera.c.add( vmove );
            UI.UpdateScene();
        }else{
            vmove.set( UI.m_Camera.k );
            vmove.y = 0;
            vmove.normalize_safe();
            vmove.mul( -dt * 5.f );
            UI.m_Camera.c.add( vmove );
            UI.UpdateScene();
        }
        break;
    }
    return true;
*/
    return false;
}

