#include "stdafx.h"
#pragma hdrstop

#include "UI_SpawnTools.h"
#include "ui_tools.h"
#include "ui_main.h"
#include "FrameSpawn.h"
#include "Scene.h"
#include "SpawnPoint.h"
//---------------------------------------------------------------------------
__fastcall TUI_ControlSpawnAdd::TUI_ControlSpawnAdd(int st, int act, ESceneCustomMTools* parent):TUI_CustomControl(st,act,parent){
}

bool __fastcall TUI_ControlSpawnAdd::AppendCallback(SBeforeAppendCallbackParams* p)
{
	LPCSTR ref_name = ((TfraSpawn*)parent_tool->pFrame)->Current();
    if (!ref_name){
    	ELog.DlgMsg(mtInformation,"Nothing selected.");
    	return false;
    }
	p->name_prefix 	= ref_name;
	p->data 		= p->name_prefix.c_str();
    return !p->name_prefix.IsEmpty();
}

bool __fastcall TUI_ControlSpawnAdd::Start(TShiftState Shift)
{
    TfraSpawn* F = (TfraSpawn*)parent_tool->pFrame;
	if (F->ebAttachObject->Down){
		CCustomObject* from = Scene.RayPickObject(flt_max, UI.m_CurrentRStart, UI.m_CurrentRNorm, OBJCLASS_DUMMY, 0, 0);
        if (from->ClassID!=OBJCLASS_SPAWNPOINT){
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

