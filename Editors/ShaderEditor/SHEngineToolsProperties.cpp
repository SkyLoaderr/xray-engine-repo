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
	TProperties* P = Tools.m_Props;
	P->BeginEditMode();
    if (mode==CMatrix::modeTCM){
        P->AddFlagItem(item,"Scale enabled",	&m->tcm,CMatrix::tcmScale);
        P->AddWaveItem(item,"Scale U",			&m->scaleU);
        P->AddWaveItem(item,"Scale V",			&m->scaleV);
        P->AddFlagItem(item,"Rotate enabled",	&m->tcm,CMatrix::tcmRotate);
        P->AddWaveItem(item,"Rotate",			&m->rotate);
        P->AddFlagItem(item,"Scroll enabled",	&m->tcm,CMatrix::tcmScroll);
        P->AddWaveItem(item,"Scroll U",			&m->scrollU);
        P->AddWaveItem(item,"Scroll V",			&m->scrollV);
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
void __fastcall CSHEngineTools::ModeOnAfterEdit(TElTreeItem* item, PropItem* sender, LPVOID edit_val)
{
	TElTreeItem* parent=item->Parent; R_ASSERT(parent);
    R_ASSERT(parent->Tag==PROP_LIST);
    ListValue* V = (ListValue*)parent->Data;
    string128 nm; strcpy(nm,V->GetValue());
    CMatrix* M = FindMatrix(nm,false); R_ASSERT(M);
    V->ApplyValue(nm);
    DWORD mode=*(DWORD*)edit_val;
    UpdateMatrixModeProps(item,M,mode);
}
//---------------------------------------------------------------------------
void __fastcall CSHEngineTools::AddMatrixProps(TElTreeItem* item, LPSTR name)
{
    CMatrix* M = FindMatrix(name,true);
    R_ASSERT(M);
	Tools.m_Props->BeginEditMode();
    TElTreeItem* node = Tools.m_Props->AddTokenItem(item,"Mode",&M->dwMode,mode_token,&ModeOnAfterEdit)->item;
    UpdateMatrixModeProps(node,M,M->dwMode);
	Tools.m_Props->EndEditMode(item);
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::RemoveMatrixProps(TElTreeItem* parent){
    TProperties* P=Tools.m_Props;
	P->BeginEditMode();
	for (TElTreeItem* item=parent->GetLastChild(); item;){
    	TElTreeItem* node=parent->GetPrevChild(item);
        item->Delete();
        item=node;
	}
	P->EndEditMode(0);
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::MCOnDraw(PropItem* sender, LPVOID draw_val)
{
	AnsiString& V=*(AnsiString*)draw_val;
    VERIFY(V[1]);
	if (V[1]!='$') V="";
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::MatrixOnAfterEdit(TElTreeItem* item, PropItem* sender, LPVOID edit_val)
{
	ListValue* V = (ListValue*)sender;
	LPSTR nm=(LPSTR)edit_val;	VERIFY(nm&&nm[0]);

	if (*nm!='$'){
        if (*V->GetValue()=='$'){
            strcpy(nm,AppendMatrix());
            AddMatrixProps(item,nm);
        }
    }else{
        if (*V->GetValue()!='$'){
        	string128 nmm; strcpy(nmm,V->GetValue());
            RemoveMatrix(nmm);
            V->ApplyValue(nmm);
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
    TProperties* P=Tools.m_Props;
    P->AddWaveItem(item,"R",&C->_R);
    P->AddWaveItem(item,"G",&C->_G);
    P->AddWaveItem(item,"B",&C->_B);
    P->AddWaveItem(item,"A",&C->_A);
	Tools.m_Props->EndEditMode(item);
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::RemoveConstProps(TElTreeItem* parent){
    TProperties* P=Tools.m_Props;
	P->BeginEditMode();
	for (TElTreeItem* item=parent->GetLastChild(); item;){
    	TElTreeItem* node=parent->GetPrevChild(item);
        item->Delete();
        item=node;
	}
	P->EndEditMode(0);
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::ConstOnAfterEdit(TElTreeItem* item, PropItem* sender, LPVOID edit_val)
{
	ListValue* V = (ListValue*)sender;
	LPSTR nm=(LPSTR)edit_val;	VERIFY(nm&&nm[0]);

	if (*nm!='$'){
        if (*V->GetValue()=='$'){
            strcpy(nm,AppendConstant());
            AddConstProps(item,nm);
        }
    }else{
        if (*V->GetValue()!='$'){
        	string128 nmm; strcpy(nmm,V->GetValue());
            RemoveConstant(nmm);
            V->ApplyValue(nmm);
            RemoveConstProps(item);
        }
    }
}
//------------------------------------------------------------------------------
void __fastcall CSHEngineTools::NameOnAfterEdit(TElTreeItem* item, PropItem* sender, LPVOID edit_val)
{
	FOLDER::AfterTextEdit(fraLeftBar->tvEngine->Selected,((TextValue*)sender)->GetValue(),*(AnsiString*)edit_val);
}
//------------------------------------------------------------------------------
void __fastcall CSHEngineTools::NameOnBeforeEdit(TElTreeItem* item, PropItem* sender, LPVOID edit_val)
{
	FOLDER::BeforeTextEdit(((TextValue*)sender)->GetValue(),*(AnsiString*)edit_val);
}
//------------------------------------------------------------------------------
void __fastcall CSHEngineTools::NameOnDraw(PropItem* sender, LPVOID draw_val)
{
	FOLDER::TextDraw(((TextValue*)sender)->GetValue(),*(AnsiString*)draw_val);
}
//------------------------------------------------------------------------------

void CSHEngineTools::UpdateProperties()
{
	if (m_bFreezeUpdate) return;
	TProperties* P=Tools.m_Props;
    P->BeginFillMode("Engine shader");
	if (m_CurrentBlender){ // fill Tree
    	CStream data(m_BlenderStream.pointer(), m_BlenderStream.size());
        CBlender_DESC* desc=(CBlender_DESC*)data.Pointer();
        data.Advance(sizeof(CBlender_DESC));
        DWORD type;
        char key[255];
        TElTreeItem* marker_node=0;
        TElTreeItem* node;

        P->AddMarkerItem(0,"Type",m_CurrentBlender->getComment());
        P->AddTextItem	(0,"Name",(LPSTR)&desc->cName,sizeof(desc->cName),NameOnAfterEdit,NameOnBeforeEdit,NameOnDraw);

        while (!data.Eof()){
            int sz=0;
            type = data.Rdword();     
            data.RstringZ(key);
            switch(type){
            case xrPID_MARKER:
            	marker_node = P->AddMarkerItem(0,key)->item;
            break;
            case xrPID_TOKEN:{
            	xrP_TOKEN* V=(xrP_TOKEN*)data.Pointer();
            	sz=sizeof(xrP_TOKEN)+sizeof(xrP_TOKEN::Item)*V->Count;
                P->AddTokenItem(marker_node,key,&V->IDselected,V->Count,(TokenValue3::Item*)(LPBYTE(data.Pointer()) + sizeof(xrP_TOKEN)));
            }break;
            case xrPID_MATRIX:{
            	sz=sizeof(string64);
                TElTreeItem* node=P->AddListItemA(marker_node,key,(LPSTR)data.Pointer(),MCSTRING_COUNT,MCString,MatrixOnAfterEdit,0,MCOnDraw)->item;
                LPSTR V=(LPSTR)data.Pointer();
				if (V&&V[0]&&(*V!='$')) AddMatrixProps(node,V);
            }break;
            case xrPID_CONSTANT:{
            	sz=sizeof(string64);
                TElTreeItem* node=P->AddListItemA(marker_node,key,(LPSTR)data.Pointer(),MCSTRING_COUNT,MCString,ConstOnAfterEdit,0,MCOnDraw)->item;
                LPSTR V=(LPSTR)data.Pointer();
				if (V&&V[0]&&(*V!='$')) AddConstProps(node,V);
            }break;
            case xrPID_TEXTURE:
            	sz=sizeof(string64);
                P->AddTexture2Item(marker_node,key,(LPSTR)data.Pointer(),sz);
            break;
            case xrPID_INTEGER:{
            	sz=sizeof(xrP_Integer);
                xrP_Integer* V=(xrP_Integer*)data.Pointer();
                P->AddIntItem	(marker_node,key,&V->value,V->min,V->max,1);
            }break;
            case xrPID_FLOAT:{
            	sz=sizeof(xrP_Float);
                xrP_Float* V=(xrP_Float*)data.Pointer();
                P->AddFloatItem	(marker_node,key,&V->value,V->min,V->max,0.01f,2);
            }break;
            case xrPID_BOOL:{
            	sz=sizeof(xrP_BOOL);
                xrP_BOOL* V=(xrP_BOOL*)data.Pointer();
                P->AddBOOLItem	(marker_node,key,&V->value);
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

 