#include "stdafx.h"
#pragma hdrstop

#include "ui_control.h"
#include "ui_tools.h"
#include "ui_main.h"
#include "scene.h"
#include "topbar.h"
#include "ui_customtools.h"
#include "bottombar.h"
#include "leftbar.h"

TUI_CustomControl::TUI_CustomControl(int st, int act, TUI_CustomTools* parent){
	parent_tool		= parent; VERIFY(parent);
	sub_target		= st;
    action			= act;
	bBoxSelection	= false;
	m_ScaleCenter.set(0,0,0);
}

bool TUI_CustomControl::Start  (TShiftState _Shift){
	switch(action){
	case eaSelect: 	return SelectStart(_Shift);
	case eaAdd: 	return AddStart(_Shift);
	case eaMove: 	return MovingStart(_Shift);
	case eaRotate: 	return RotateStart(_Shift);
	case eaScale: 	return ScaleStart(_Shift);
    }
    return false;
}
bool TUI_CustomControl::End    (TShiftState _Shift){
	switch(action){
	case eaSelect: 	return SelectEnd(_Shift);
	case eaAdd: 	return AddEnd(_Shift);
	case eaMove: 	return MovingEnd(_Shift);
	case eaRotate: 	return RotateEnd(_Shift);
	case eaScale: 	return ScaleEnd(_Shift);
    }
    return false;
}
void TUI_CustomControl::Move   (TShiftState _Shift){
	switch(action){
	case eaSelect: 	SelectProcess(_Shift); break;
	case eaAdd: 	AddProcess(_Shift);    break;
	case eaMove: 	MovingProcess(_Shift); break;
	case eaRotate: 	RotateProcess(_Shift); break;
	case eaScale: 	ScaleProcess(_Shift);  break;
    }
}
bool TUI_CustomControl::HiddenMode(){
	switch(action){
	case eaSelect: 	return false;
	case eaAdd: 	return false;
	case eaMove: 	return true;
	case eaRotate: 	return true;
	case eaScale: 	return true;
    }
    return false;
}

//------------------------------------------------------------------------------
// add
//------------------------------------------------------------------------------
SceneObject* __fastcall TUI_CustomControl::DefaultAddObject(TShiftState Shift){
    if (Shift==ssRBOnly){ UI->Command(COMMAND_SHOWCONTEXTMENU,parent_tool->objclass); return 0;}
    Fvector p;
    SceneObject* obj=0;
    if (UI->PickGround(p,UI->m_CurrentRStart,UI->m_CurrentRNorm)){
		char namebuffer[MAX_OBJ_NAME];
		Scene->GenObjectName(parent_tool->objclass, namebuffer);
		obj = NewObjectFromClassID(parent_tool->objclass);
        strcpy(obj->GetName(),namebuffer);
		obj->Move( p );
        Scene->SelectObjects(false,parent_tool->objclass);
		Scene->AddObject(obj);
		if (Shift.Contains(ssCtrl)) UI->Command(COMMAND_SHOWPROPERTIES);
        if (!Shift.Contains(ssAlt)) ResetActionToSelect();
    }
    return obj;
}

bool __fastcall TUI_CustomControl::AddStart(TShiftState Shift)
{
	DefaultAddObject(Shift);
    return false;
}
void __fastcall TUI_CustomControl::AddProcess(TShiftState _Shift)
{
}
bool __fastcall TUI_CustomControl::AddEnd(TShiftState _Shift)
{
    return true;
}

//------------------------------------------------------------------------------
// total select 
//------------------------------------------------------------------------------
bool __fastcall TUI_CustomControl::SelectStart(TShiftState Shift){
	EObjClass cls = UI->CurrentClassID();
	
    if (Shift==ssRBOnly){ UI->Command(COMMAND_SHOWCONTEXTMENU,parent_tool->objclass); return false;}
    if (!Shift.Contains(ssCtrl)) Scene->SelectObjects( false, cls);

    SceneObject *obj = Scene->RayPick( UI->m_CurrentRStart,UI->m_CurrentRNorm, cls, 0, true, false);
    bBoxSelection    = (obj && Shift.Contains(ssCtrl)) || !obj;

    if( bBoxSelection ){
        UI->EnableSelectionRect( true );
        UI->UpdateSelectionRect(UI->m_StartCp,UI->m_CurrentCp);
        if(obj){ 
	        if(obj->IsInGroup()&&!fraLeftBar->ebIgnoreGroup->Down) Scene->GroupSelect(obj->GetGroupIndex(),(obj->Selected())?false:true,false);
        	else obj->Select((obj->Selected())?false:true);
        }
        return true;
    } else {
        if( obj ){ 
	        if(obj->IsInGroup()&&!fraLeftBar->ebIgnoreGroup->Down) Scene->GroupSelect(obj->GetGroupIndex(),(obj->Selected())?false:true,false);
            else obj->Select(obj->Selected()?false:true);
        }
    }
    return false;
}

void __fastcall TUI_CustomControl::SelectProcess(TShiftState _Shift){
    if (bBoxSelection) UI->UpdateSelectionRect(UI->m_StartCp,UI->m_CurrentCp);
}

bool __fastcall TUI_CustomControl::SelectEnd(TShiftState _Shift)
{
    if (bBoxSelection){
        UI->EnableSelectionRect( false );
        bBoxSelection = false;
        Scene->FrustumSelect(true,fraLeftBar->ebIgnoreTarget->Down?OBJCLASS_DUMMY:parent_tool->objclass);
    }
    return true;
}

//------------------------------------------------------------------------------------
// moving
//------------------------------------------------------------------------------------
bool __fastcall TUI_CustomControl::MovingStart(TShiftState Shift){
	EObjClass cls = UI->CurrentClassID();

    if(Shift==ssRBOnly){ UI->Command(COMMAND_SHOWCONTEXTMENU,parent_tool->objclass); return false;}
    if(Scene->SelectionCount(true,cls)==0) return false;

    if (fraTopBar->ebAxisY->Down){
		m_MovingXVector.set(0,0,0);
		m_MovingYVector.set(0,1,0);
	}else{
		m_MovingXVector.set( UI->Device.m_Camera.GetRight() );
		m_MovingXVector.y = 0;
		m_MovingYVector.set( UI->Device.m_Camera.GetDirection() );
		m_MovingYVector.y = 0;
		m_MovingXVector.normalize_safe();
		m_MovingYVector.normalize_safe();
	}
	m_MovingReminder.set(0,0,0);
    return true;
}

bool __fastcall TUI_CustomControl::DefaultMovingProcess(TShiftState Shift, Fvector& amount){
    if (Shift.Contains(ssLeft)||Shift.Contains(ssRight)){
        amount.mul( m_MovingXVector, UI->m_MouseSM * UI->m_DeltaCpH.x );
        amount.direct( amount, m_MovingYVector, -UI->m_MouseSM * UI->m_DeltaCpH.y );

        if( fraTopBar->ebMSnap->Down ){
        	CHECK_SNAP(m_MovingReminder.x,amount.x,UI->movesnap());
        	CHECK_SNAP(m_MovingReminder.y,amount.y,UI->movesnap());
        	CHECK_SNAP(m_MovingReminder.z,amount.z,UI->movesnap());
        }

        if (!fraTopBar->ebAxisX->Down&&!fraTopBar->ebAxisZX->Down) amount.x = 0.f;
        if (!fraTopBar->ebAxisZ->Down&&!fraTopBar->ebAxisZX->Down) amount.z = 0.f;
        if (!fraTopBar->ebAxisY->Down) amount.y = 0.f;

        return (amount.square_magnitude()>EPS_S);
	}
    return false;
}

void __fastcall TUI_CustomControl::MovingProcess(TShiftState _Shift)
{
	Fvector amount;
	if (DefaultMovingProcess(_Shift,amount)){
		EObjClass cls = UI->CurrentClassID();
		bool flt = cls!=OBJCLASS_DUMMY;
        for(ObjectPairIt it=Scene->FirstClass(); it!=Scene->LastClass(); it++){
            ObjectList& lst = (*it).second;
            if ((cls==OBJCLASS_DUMMY)||(parent_tool->objclass==(*it).first))
                for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                    if((*_F)->Visible()&&(*_F)->Selected()) if ((flt&&!(*_F)->IsInGroup())||!flt) (*_F)->Move( amount );
        }
    }
}

bool __fastcall TUI_CustomControl::MovingEnd(TShiftState _Shift)
{
	Scene->UndoSave();
    return true;
}

//------------------------------------------------------------------------------------
// rotate
//------------------------------------------------------------------------------------
bool __fastcall TUI_CustomControl::RotateStart(TShiftState Shift)
{
	EObjClass cls = UI->CurrentClassID();

    if(Shift==ssRBOnly){ UI->Command(COMMAND_SHOWCONTEXTMENU,parent_tool->objclass); return false;}
    if(Scene->SelectionCount(true,cls)==0) return false;

    m_RotateCenter.set( UI->pivot() );
    m_RotateVector.set(0,0,0);
    if (fraTopBar->ebAxisX->Down) m_RotateVector.set(1,0,0);
    else if (fraTopBar->ebAxisY->Down) m_RotateVector.set(0,1,0);
    else if (fraTopBar->ebAxisZ->Down) m_RotateVector.set(0,0,1);
	m_fRotateSnapAngle = 0;
    return true;
}

void __fastcall TUI_CustomControl::RotateProcess(TShiftState _Shift)
{
    if (_Shift.Contains(ssLeft)){
        float amount = -UI->m_DeltaCpH.x * UI->m_MouseSR;

        if( fraTopBar->ebASnap->Down ) CHECK_SNAP(m_fRotateSnapAngle,amount,UI->anglesnap());

		bool flt = !fraLeftBar->ebIgnoreTarget->Down;
        bool grp = !fraLeftBar->ebIgnoreGroup->Down;
        for(ObjectPairIt it=Scene->FirstClass(); it!=Scene->LastClass(); it++){
            ObjectList& lst = (*it).second;
            if (!flt||(parent_tool->objclass==(*it).first))
                for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                    if((*_F)->Visible()&&(*_F)->Selected()){
						if (grp&&(*_F)->IsInGroup()){
	                    	if (flt) continue;
                        	int idx = (*_F)->GetGroupIndex();
							Scene->GroupUpdateBox(idx);
                            Fvector C;
                            Scene->GetGroupItem(idx).box.getcenter(C);
                            (*_F)->Rotate( C, m_RotateVector, amount );
                        }else{
                            if( fraTopBar->ebCSLocal->Down ){
                                (*_F)->LocalRotate( m_RotateVector, amount );
                            } else {
                                (*_F)->Rotate( m_RotateCenter, m_RotateVector, amount );
                            }
                        }
                    }
        }
    }
}
bool __fastcall TUI_CustomControl::RotateEnd(TShiftState _Shift)
{
	Scene->UndoSave();
    return true;
}

//------------------------------------------------------------------------------
// scale
//------------------------------------------------------------------------------
bool __fastcall TUI_CustomControl::ScaleStart(TShiftState Shift)
{
	EObjClass cls = UI->CurrentClassID();
    if(Shift==ssRBOnly){ UI->Command(COMMAND_SHOWCONTEXTMENU,parent_tool->objclass); return false;}
    if(Scene->SelectionCount(true,cls)==0) return false;
	m_ScaleCenter.set( UI->pivot() );
	return true;
}

void __fastcall TUI_CustomControl::ScaleProcess(TShiftState _Shift)
{
	float dy = UI->m_DeltaCpH.x * UI->m_MouseSS;
    if (dy>1.f) dy=1.f; else if (dy<-1.f) dy=-1.f;

	Fvector amount;
	amount.set( dy, dy, dy );

    if (fraTopBar->ebNonUniformScale->Down){
	    if (!fraTopBar->ebAxisX->Down && !fraTopBar->ebAxisZX->Down) amount.x = 0.f;
    	if (!fraTopBar->ebAxisZ->Down && !fraTopBar->ebAxisZX->Down) amount.z = 0.f;
	    if (!fraTopBar->ebAxisY->Down) amount.y = 0.f;
    }

    bool flt = !fraLeftBar->ebIgnoreTarget->Down;
	bool grp = !fraLeftBar->ebIgnoreGroup->Down;
    for(ObjectPairIt it=Scene->FirstClass(); it!=Scene->LastClass(); it++){
        ObjectList& lst = (*it).second;
        if (!flt||(parent_tool->objclass==(*it).first))
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                if((*_F)->Visible()&&(*_F)->Selected()){
                    if (grp&&(*_F)->IsInGroup()){
                        int idx = (*_F)->GetGroupIndex();
                        Scene->GroupUpdateBox(idx);
                        Fvector C;
                        Scene->GetGroupItem(idx).box.getcenter(C);
                        (*_F)->Scale( C, amount );
                    }else{
                        if( fraTopBar->ebCSLocal->Down ){
                            (*_F)->LocalScale( amount );
                        } else {
                            (*_F)->Scale( UI->pivot(), amount );
                        }
                    }
                }
    }
}
bool __fastcall TUI_CustomControl::ScaleEnd(TShiftState _Shift)
{
	Scene->UndoSave();
    return true;
}
//------------------------------------------------------------------------------



