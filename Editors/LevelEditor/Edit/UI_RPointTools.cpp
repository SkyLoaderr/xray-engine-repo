#include "stdafx.h"
#pragma hdrstop

#include "ui_RPointtools.h"
#include "ui_tools.h"
#include "ui_main.h"
#include "FrameRPoint.h"
#include "Scene.h"
#include "SpawnPoint.h"
//---------------------------------------------------------------------------

TUI_SpawnPointTools::TUI_SpawnPointTools():TUI_CustomTools(OBJCLASS_SPAWNPOINT,true)
{
    AddControlCB(xr_new<TUI_ControlSpawnPointAdd>(estDefault,eaAdd,	this));
}
void TUI_SpawnPointTools::OnActivate  ()
{
    pFrame = xr_new<TfraSpawnPoint>((TComponent*)0);
	TUI_CustomTools::OnActivate();
    ((TfraSpawnPoint*)pFrame)->OnEnter();
}
void TUI_SpawnPointTools::OnDeactivate()
{
    ((TfraSpawnPoint*)pFrame)->OnExit();
	TUI_CustomTools::OnDeactivate();
    xr_delete(pFrame);
}

//--------------------------------------------------------------------------------------------------
__fastcall TUI_ControlSpawnPointAdd::TUI_ControlSpawnPointAdd(int st, int act, TUI_CustomTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlSpawnPointAdd::AppendCallback(SBeforeAppendCallbackParams* p)
{
	LPCSTR ref_name = ((TfraSpawnPoint*)parent_tool->pFrame)->Current();
    if (!ref_name){
    	ELog.DlgMsg(mtInformation,"Nothing selected.");
    	return false;
    }
	p->name_prefix 	= ref_name;
	p->data 		= p->name_prefix.c_str();
    return !p->name_prefix.IsEmpty();
}

bool __fastcall TUI_ControlSpawnPointAdd::Start(TShiftState Shift)
{
    TfraSpawnPoint* F = (TfraSpawnPoint*)parent_tool->pFrame;
	if (F->ebAttachObject->Down){
		CCustomObject* from = Scene.RayPick(UI.m_CurrentRStart, UI.m_CurrentRNorm, OBJCLASS_DUMMY, 0, false, 0);
        if (IsGroupClassID(from->ClassID)&&(from->ClassID!=OBJCLASS_SPAWNPOINT)){
            ObjectList 	lst;
            int cnt 	= Scene.GetQueryObjects(lst,OBJCLASS_SPAWNPOINT,1,1,0);
            if (1!=cnt)	ELog.DlgMsg(mtError,"Select one shape.");
            else{
                CSpawnPoint* base = dynamic_cast<CSpawnPoint*>(lst.back()); R_ASSERT(base);
                if (base->AttachObject(from)){
                    if (!Shift.Contains(ssAlt)){
                        F->ebAttachObject->Down	= false;
                        ResetActionToSelect		();
                    }
                }else{
		        	ELog.DlgMsg(mtError,"Attach impossible.");
                }
            }
        }else{
        	ELog.DlgMsg(mtError,"Attach impossible.");
        }
    }else{
	    DefaultAddObject(Shift,AppendCallback);
    }
    return false;
}

