//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SHEngineTools.h"
#include "Blender.h"
#include "UI_Tools.h"
#include "PropertiesList.h"
#include "xr_tokens.h"

#define MCSTRING_COUNT 	11
LPCSTR MCString[MCSTRING_COUNT]={"Custom...","-","$null","$base0","$base1","$base2","$base3","$base4","$base5","$base6","$base7"};
//---------------------------------------------------------------------------
xr_token							mode_token					[ ]={
	{ "Programmable",			  	CMatrix::modeProgrammable  	},
	{ "TCM",					  	CMatrix::modeTCM		  	},
	{ "Spherical Reflection",	  	CMatrix::modeS_refl	   		},
	{ "Cube Reflection",			CMatrix::modeC_refl	  		},
	{ 0,							0							}           
};
//---------------------------------------------------------------------------
void __fastcall CSHEngineTools::UpdateMatrixModeProps(TElTreeItem* item, CMatrix* m, DWORD mode)
{
	TfrmProperties* P = Tools.m_Props;
	P->BeginEditMode();
    if (mode==CMatrix::modeTCM){
        P->AddItem(item,PROP_FLAG,	"Scale enabled",	P->MakeFlagValue(&m->tcm,CMatrix::tcmScale));
        P->AddItem(item,PROP_WAVE,	"Scale U",			&m->scaleU);
        P->AddItem(item,PROP_WAVE,	"Scale V",			&m->scaleV);
        P->AddItem(item,PROP_FLAG,	"Rotate enabled",	P->MakeFlagValue(&m->tcm,CMatrix::tcmRotate));
        P->AddItem(item,PROP_WAVE,	"Rotate",			&m->rotate);
        P->AddItem(item,PROP_FLAG,	"Scroll enabled",	P->MakeFlagValue(&m->tcm,CMatrix::tcmScroll));
        P->AddItem(item,PROP_WAVE,	"Scroll U",			&m->scrollU);
        P->AddItem(item,PROP_WAVE,	"Scroll V",			&m->scrollV);
    }else{
        for (TElTreeItem* itm=item->GetLastChild(); itm;){
            TElTreeItem* node=item->GetPrevChild(itm);
            itm->Delete();
            itm=node;
        }
    }
	P->EndEditMode();
}
//---------------------------------------------------------------------------
void __fastcall CSHEngineTools::ModeOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	TElTreeItem* parent=item->Parent; R_ASSERT(parent);
    R_ASSERT(parent->Tag==PROP_LIST);
    CMatrix* M = FindMatrix(((ListValue*)parent->Data)->val,false); R_ASSERT(M);
    DWORD mode=*(DWORD*)edit_val;
    UpdateMatrixModeProps(item,M,mode);
}
//---------------------------------------------------------------------------
void __fastcall CSHEngineTools::AddMatrixProps(TElTreeItem* item, LPSTR name)
{
    CMatrix* M = FindMatrix(name,true);
    R_ASSERT(M);
	Tools.m_Props->BeginEditMode();
    TElTreeItem* node = Tools.m_Props->AddItem(item,PROP_TOKEN,"Mode",Tools.m_Props->MakeTokenValue(&M->dwMode,mode_token,&ModeOnAfterEdit));
    UpdateMatrixModeProps(node,M,M->dwMode);
	Tools.m_Props->EndEditMode(item);
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::RemoveMatrixProps(TElTreeItem* parent){
    TfrmProperties* P=Tools.m_Props;
	P->BeginEditMode();
	for (TElTreeItem* item=parent->GetLastChild(); item;){
    	TElTreeItem* node=parent->GetPrevChild(item);
        item->Delete();
        item=node;
	}
	P->EndEditMode(0);
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::MCOnDraw(PropValue* sender, LPVOID draw_val)
{
	LPSTR V=(LPSTR)draw_val;
    VERIFY(V&&V[0]);
	if (*V!='$') *V=0;
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::MatrixOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	ListValue* V = (ListValue*)sender;
	LPSTR nm=(LPSTR)edit_val;	VERIFY(nm&&nm[0]);

	if (*nm!='$'){
        if (*V->val=='$'){
            strcpy(nm,AppendMatrix());
            AddMatrixProps(item,nm);
        }
    }else{
        if (*V->val!='$'){
            RemoveMatrix(nm);
            RemoveMatrixProps(item);
        }
    }
}

void CSHEngineTools::UpdateProperties()
{
	if (m_bFreezeUpdate) return;
	TfrmProperties* P=Tools.m_Props;
    P->BeginFillMode("Engine shader");
	if (m_CurrentBlender){ // fill Tree
    	CStream data(m_BlenderStream.pointer(), m_BlenderStream.size());
        data.Advance(sizeof(CBlender_DESC));
        DWORD type;
        char key[255];
        TElTreeItem* marker_node=0;
        TElTreeItem* node;

        P->AddItem(0,PROP_MARKER2,"Type",(LPVOID)m_CurrentBlender->getComment());

        while (!data.Eof()){
            int sz=0;
            type = data.Rdword();     
            data.RstringZ(key);
            switch(type){
            case xrPID_MARKER:
            	marker_node = P->AddItem(0,PROP_MARKER,key,data.Pointer());
            break;
            case xrPID_TOKEN:{
            	xrP_TOKEN* V=(xrP_TOKEN*)data.Pointer();
            	sz=sizeof(xrP_TOKEN)+sizeof(xrP_TOKEN::Item)*V->Count;
                P->AddItem(marker_node,PROP_TOKEN3,key,P->MakeTokenValue3(&V->IDselected,V->Count,(TokenValue3::Item*)(LPBYTE(data.Pointer()) + sizeof(xrP_TOKEN))));
            }break;
            case xrPID_MATRIX:{
            	sz=sizeof(string64);
                TElTreeItem* node=P->AddItem(marker_node,PROP_LIST,key,P->MakeListValueA(data.Pointer(),MCSTRING_COUNT,MCString,&MatrixOnAfterEdit,0,&MCOnDraw));
                LPSTR V=(LPSTR)data.Pointer();
				if (V&&V[0]&&(*V!='$')) AddMatrixProps(node,V);
            }break;
            case xrPID_CONSTANT:
            	sz=sizeof(string64);
                P->AddItem(marker_node,PROP_LIST,key,P->MakeListValueA(data.Pointer(),MCSTRING_COUNT,MCString));
            break;
            case xrPID_TEXTURE:
            	sz=sizeof(string64);
                P->AddItem(marker_node,PROP_TEXTURE2,key,data.Pointer());
            break;
            case xrPID_INTEGER:{
            	sz=sizeof(xrP_Integer);
                xrP_Integer* V=(xrP_Integer*)data.Pointer();
                P->AddItem	(marker_node,PROP_INTEGER,key,P->MakeIntValue	(&V->value,V->min,V->max,1));
            }break;
            case xrPID_FLOAT:{
            	sz=sizeof(xrP_Float);
                xrP_Float* V=(xrP_Float*)data.Pointer();
                P->AddItem	(marker_node,PROP_FLOAT,key,P->MakeFloatValue	(&V->value,V->min,V->max,0.01f,2));
            }break;
            case xrPID_BOOL:{
            	sz=sizeof(xrP_BOOL);
                xrP_BOOL* V=(xrP_BOOL*)data.Pointer();
                P->AddItem	(marker_node,PROP_BOOL,key,&V->value);
            }break;
            default: THROW2("UNKNOWN xrPID_????");
            }
            data.Advance(sz);
        }
    }
    P->EndFillMode();
    P->SetModifiedEvent(Modified);
}


 