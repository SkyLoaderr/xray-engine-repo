//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SHEngineTools.h"
#include "Blender.h"
#include "UI_Tools.h"
#include "PropertiesList.h"
#include "xr_tokens.h"
#include "xr_trims.h"
#include "leftbar.h"
#include "folderlib.h"

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
	AnsiString& V=*(AnsiString*)draw_val;
    VERIFY(V[1]);
	if (V[1]!='$') V="";
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
            RemoveMatrix(V->val);
            RemoveMatrixProps(item);
        }
    }
}
//------------------------------------------------------------------------------

void __fastcall CSHEngineTools::AddConstProps(TElTreeItem* item, LPSTR name)
{
	Tools.m_Props->BeginEditMode();
	CConstant* C = Tools.SEngine.FindConstant(name,true);
    R_ASSERT(C);
    TfrmProperties* P=Tools.m_Props;
    P->AddItem(item,PROP_WAVE,	"R",	(LPDWORD)&C->_R);
    P->AddItem(item,PROP_WAVE,	"G",	(LPDWORD)&C->_G);
    P->AddItem(item,PROP_WAVE,	"B",	(LPDWORD)&C->_B);
    P->AddItem(item,PROP_WAVE,	"A",	(LPDWORD)&C->_A);
	Tools.m_Props->EndEditMode(item);
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::RemoveConstProps(TElTreeItem* parent){
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

void __fastcall CSHEngineTools::ConstOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	ListValue* V = (ListValue*)sender;
	LPSTR nm=(LPSTR)edit_val;	VERIFY(nm&&nm[0]);

	if (*nm!='$'){
        if (*V->val=='$'){
            strcpy(nm,AppendConstant());
            AddConstProps(item,nm);
        }
    }else{
        if (*V->val!='$'){
            RemoveConstant(V->val);
            RemoveConstProps(item);
        }
    }
}
//------------------------------------------------------------------------------
void __fastcall CSHEngineTools::NameOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	FOLDER::AfterTextEdit(fraLeftBar->tvEngine->Selected,((TextValue*)sender)->val,*(AnsiString*)edit_val);
}
//------------------------------------------------------------------------------
void __fastcall CSHEngineTools::NameOnBeforeEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	FOLDER::BeforeTextEdit(((TextValue*)sender)->val,*(AnsiString*)edit_val);
}
//------------------------------------------------------------------------------
void __fastcall CSHEngineTools::NameOnDraw(PropValue* sender, LPVOID draw_val)
{
	FOLDER::TextDraw(((TextValue*)sender)->val,*(AnsiString*)draw_val);
}
//------------------------------------------------------------------------------

void CSHEngineTools::UpdateProperties()
{
	if (m_bFreezeUpdate) return;
	TfrmProperties* P=Tools.m_Props;
    P->BeginFillMode("Engine shader");
	if (m_CurrentBlender){ // fill Tree
    	CStream data(m_BlenderStream.pointer(), m_BlenderStream.size());
        CBlender_DESC* desc=(CBlender_DESC*)data.Pointer();
        data.Advance(sizeof(CBlender_DESC));
        DWORD type;
        char key[255];
        TElTreeItem* marker_node=0;
        TElTreeItem* node;

        P->AddItem(0,PROP_MARKER2,"Type",(LPVOID)m_CurrentBlender->getComment());
        P->AddItem(0,PROP_TEXT,"Name",P->MakeTextValue(&desc->cName,NameOnAfterEdit,NameOnBeforeEdit,NameOnDraw));

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
                TElTreeItem* node=P->AddItem(marker_node,PROP_LIST,key,P->MakeListValueA(data.Pointer(),MCSTRING_COUNT,MCString,MatrixOnAfterEdit,0,MCOnDraw));
                LPSTR V=(LPSTR)data.Pointer();
				if (V&&V[0]&&(*V!='$')) AddMatrixProps(node,V);
            }break;
            case xrPID_CONSTANT:{
            	sz=sizeof(string64);
                TElTreeItem* node=P->AddItem(marker_node,PROP_LIST,key,P->MakeListValueA(data.Pointer(),MCSTRING_COUNT,MCString,ConstOnAfterEdit,0,MCOnDraw));
                LPSTR V=(LPSTR)data.Pointer();
				if (V&&V[0]&&(*V!='$')) AddConstProps(node,V);
            }break;
            case xrPID_TEXTURE:
            	sz=sizeof(string64);
                P->AddItem(marker_node,PROP_TEXTURE2,key,P->MakeTextValue	(data.Pointer()));
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
        ApplyChanges(true);
    }
    P->EndFillMode();
    P->SetModifiedEvent(Modified);
}
//------------------------------------------------------------------------------

 